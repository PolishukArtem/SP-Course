// sysproglab2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "windows.h"
#include "stdio.h"
#include "string.h"

using namespace std;

#define TEXTFIELD_SIZE 80

CONST WCHAR RECORDFILE[] = L"RecordBase.rb";
//Структура для записи в файл
typedef struct
{
	WORD wRecordId; // запись айди
	FILETIME fltmCreationTime; // запись тайминга создания
	CHAR cText[TEXTFIELD_SIZE + 1]; // запись текста
	WORD wChangeRecordCounter; // счётчик
} USERRECORD, *P_USERRECORD;

// Структура заголовка
typedef struct
{
	WORD wNotNullRecordsCount; //НеНулевой счётчик
	WORD wRecordFileSize; //размер файла
} RECORDFILE_HEADER, *P_RECORDFILE_HEADER;

bool InitProgData(HANDLE* hRecordFile, DWORD* dwFileSize, DWORD* dwCountRecords);
bool GetHead(HANDLE hRecordFile, DWORD dwCreateOrGetHeader, P_RECORDFILE_HEADER pHeader, BOOL bClearFile);
bool CreateRec(HANDLE hRecordsFile, DWORD* dwFileSize, CHAR* recText, DWORD* dwRecordId);
bool WriteRecInFile(HANDLE hRecordFile, USERRECORD usRecord, DWORD dwOffset);
bool DeleteRec(HANDLE hRecordFile, DWORD dwIdToDelete, DWORD* dwCountRecords, DWORD* dwFileSize);
bool ShowRec(HANDLE hRecordFile, DWORD dwCountRecords);
bool UpdateRec(HANDLE hRecordFile, DWORD  dwIdToModify);

//Initialize data
bool InitProgData(HANDLE* hRecordFile, DWORD* dwFileSize, DWORD* dwCountRecords)
{
	//открыть файл
	*hRecordFile = CreateFile(RECORDFILE,
		GENERIC_ALL,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (*hRecordFile == INVALID_HANDLE_VALUE)
		return false;

	//получить размер файла
	*dwFileSize = GetFileSize(*hRecordFile, NULL);
	if (*dwFileSize == INVALID_FILE_SIZE)
		return false;

	if (*dwFileSize <= sizeof(RECORDFILE_HEADER))
	{
		RECORDFILE_HEADER header;
		header.wNotNullRecordsCount = 0;
		header.wRecordFileSize = sizeof(RECORDFILE_HEADER);
		if (!GetHead(*hRecordFile, 1, &header, TRUE)) //создать заголовок
		{
			return false;
		}
		*dwFileSize = sizeof(RECORDFILE_HEADER);
	}
	*dwCountRecords = (*dwFileSize - sizeof(RECORDFILE_HEADER)) / sizeof(USERRECORD);
	return true;
}

//Filling file
bool WriteRecInFile(HANDLE hRecordFile, USERRECORD usRecord, DWORD dwOffset)
{
	if (SetFilePointer(hRecordFile, dwOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return false;

	DWORD dwCountWritenBytes = 0;
	if ((WriteFile(hRecordFile, &usRecord, sizeof(usRecord), &dwCountWritenBytes, NULL) == false) || (dwCountWritenBytes != sizeof(usRecord)))
		return false;

	return true;
}


//Create record
bool CreateRec(HANDLE hRecordsFile, DWORD* dwFileSize, CHAR* recText, DWORD* dwRecordId)
{
	BOOL isNullRecord = false;
	USERRECORD usRecord;
	if (strcmp(recText, "0") == 0)
	{
		memset(usRecord.cText, '\0', sizeof(usRecord.cText));
		isNullRecord = true;
	}
	else
	{
		strcpy_s(usRecord.cText, TEXTFIELD_SIZE, recText);
		usRecord.cText[TEXTFIELD_SIZE] = '\0';
		isNullRecord = false;
	}
	usRecord.wChangeRecordCounter = 0;
	usRecord.wRecordId = *dwRecordId;

	SYSTEMTIME st = { 0 };
	FILETIME ft = { 0 };
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	usRecord.fltmCreationTime = ft;

	if (!WriteRecInFile(hRecordsFile, usRecord, sizeof(RECORDFILE_HEADER) + (*dwRecordId) * sizeof(USERRECORD)))
		return false;

	RECORDFILE_HEADER header = { 0 };
	GetHead(hRecordsFile, 2, &header, FALSE);
	header.wRecordFileSize += sizeof(USERRECORD);
	header.wNotNullRecordsCount = isNullRecord ? header.wNotNullRecordsCount : ++header.wNotNullRecordsCount;
	GetHead(hRecordsFile, 1, &header, FALSE);

	(*dwRecordId)++;
	(*dwFileSize) += sizeof(USERRECORD);

	return true;
}

//Update record
bool UpdateRec(HANDLE hRecordFile, DWORD  dwIdToModify)
{
	RECORDFILE_HEADER header;
	CHAR cBuff[TEXTFIELD_SIZE + 1] = { 0 };

	if (!GetHead(hRecordFile, 2, &header, FALSE))
		return false;

	cout << "Input NEW information (<80 symbs):\n";
	cin >> cBuff;
	cBuff[TEXTFIELD_SIZE] = '\0';

	USERRECORD urBuff = { 0 };
	DWORD dwBytes = 0;
	BOOL isNullRecord = false;

	if (SetFilePointer(hRecordFile, sizeof(RECORDFILE_HEADER) + sizeof(USERRECORD) * dwIdToModify, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return false;

	if ((ReadFile(hRecordFile, &urBuff, sizeof(USERRECORD), &dwBytes, NULL) == false) || (dwBytes != sizeof(USERRECORD)))
		return false;

	isNullRecord = urBuff.cText[0] == '\0' ? true : false;

	if (strcmp(cBuff, "0") == 0)
		memset(urBuff.cText, '\0', sizeof(urBuff.cText));
	else
	{
		strcpy_s(urBuff.cText, TEXTFIELD_SIZE, cBuff);
		urBuff.cText[80] = '\0';
	}
	urBuff.wChangeRecordCounter++;

	if (!WriteRecInFile(hRecordFile, urBuff, (sizeof(RECORDFILE_HEADER) + sizeof(USERRECORD) * dwIdToModify)))
		return false;

	if (isNullRecord && (strcmp(cBuff, "0") != 0)) header.wNotNullRecordsCount++;

	if (!isNullRecord && (strcmp(cBuff, "0") == 0)) header.wNotNullRecordsCount--;

	if (!GetHead(hRecordFile, 1, &header, FALSE))
		return false;

	return true;
}

//Delete record
bool DeleteRec(HANDLE hRecordFile, DWORD dwIdToDelete, DWORD* dwCountRecords, DWORD* dwFileSize)
{
	RECORDFILE_HEADER header;
	DWORD dwLastId = *dwCountRecords - 1;
	CHAR cBuff[TEXTFIELD_SIZE + 1] = { 0 };

	if (!GetHead(hRecordFile, 2, &header, FALSE))
		return false;

	USERRECORD urBuff = { 0 };
	DWORD dwBytes = 0;

	if (SetFilePointer(hRecordFile, sizeof(RECORDFILE_HEADER) + sizeof(USERRECORD) * dwLastId, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return false;

	if ((ReadFile(hRecordFile, &urBuff, sizeof(USERRECORD), &dwBytes, NULL) == false) || (dwBytes != sizeof(USERRECORD)))
		return false;

	urBuff.wRecordId = dwIdToDelete;

	if (!WriteRecInFile(hRecordFile, urBuff, (sizeof(RECORDFILE_HEADER) + sizeof(USERRECORD) * dwIdToDelete)))
		return false;

	header.wNotNullRecordsCount = urBuff.cText[0] == '\0' ? header.wNotNullRecordsCount : header.wNotNullRecordsCount - 1;
	header.wRecordFileSize = header.wRecordFileSize - sizeof(USERRECORD);
	(*dwCountRecords)--;
	(*dwFileSize) -= sizeof(USERRECORD);

	if (!GetHead(hRecordFile, 1, &header, FALSE))
		return false;
	return true;
}

//Get Header
bool GetHead(HANDLE hRecordFile, DWORD dwCreateOrGetHeader, P_RECORDFILE_HEADER pHeader, BOOL bClearFile)
{
	DWORD dwBytes = 0;
	if (SetFilePointer(hRecordFile, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return false;

	if (dwCreateOrGetHeader == 1) //Создать заголовок
	{
		if ((WriteFile(hRecordFile, pHeader, sizeof(RECORDFILE_HEADER), &dwBytes, NULL) == false) ||
			(dwBytes != sizeof(RECORDFILE_HEADER)))
			return false;

		if (bClearFile)
		{
			if (SetEndOfFile(hRecordFile) == false)
				return false;
		}
	}
	else if (dwCreateOrGetHeader == 2) //Считать заголовок
	{
		if ((ReadFile(hRecordFile, pHeader, sizeof(RECORDFILE_HEADER), &dwBytes, NULL) == false) ||
			(dwBytes != sizeof(RECORDFILE_HEADER)))
			return false;
	}
	else
		return false;

	return true;
}

bool ShowRec(HANDLE hRecordFile, DWORD dwCountRecords)
{
	RECORDFILE_HEADER header = { 0 };
	USERRECORD recordsBuff = { 0 };
	DWORD dwReadedBytes = 0;

	if (!GetHead(hRecordFile, 2, &header, FALSE))
		return false;

	cout << "\n============= HEADER =============" << endl;
	cout << "* Number of NotNull records: " << header.wNotNullRecordsCount << endl;
	cout << "* File size: " << header.wRecordFileSize << " Bytes" << endl;

	while ((ReadFile(hRecordFile, &recordsBuff, sizeof(USERRECORD), &dwReadedBytes, NULL) != false) && (dwReadedBytes == sizeof(USERRECORD)))
	{
		SYSTEMTIME st;
		FileTimeToSystemTime(&recordsBuff.fltmCreationTime, &st);
		cout << "============= RECORD =============" << endl;
		cout << "* ID:" << recordsBuff.wRecordId << endl;
		printf("* Creation date: %d-%d-%d, %d:%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
		cout << "* Number of updates: " << recordsBuff.wChangeRecordCounter << endl;
		cout << "* Text: " << recordsBuff.cText << endl;
	}
	return true;
}

int main()
{

	DWORD dwCountRecords = 0, dwFileSize = 0;
	HANDLE hRecordFile = NULL;
	BOOL bDeleteFlag = FALSE;

	if (!InitProgData(&hRecordFile, &dwFileSize, &dwCountRecords))
	{
		cout << "Can't initialize file for record!" << endl;
		return 1;
	}

	CHAR cKey;
	for (;;) {
		cout << "\nChoose operation:" << endl;
		cout << " 1 - Create record" << endl;
		cout << " 2 - Update record" << endl;
		cout << " 3 - Delete record" << endl;
		cout << " 4 - Get header" << endl;
		cout << " 5 - ShutDown" << endl;
		cout << "Your choice is: ";
		cin >> cKey;
		switch (cKey)
		{
		case '1': //Создать запись
		{
			CHAR Buff[TEXTFIELD_SIZE] = { 0 };
			cout << "\nInput (<80 symbols) information:\n";
			cin >> Buff;
			if (sizeof(Buff) <= TEXTFIELD_SIZE) {
				if (!CreateRec(hRecordFile, &dwFileSize, Buff, &dwCountRecords))
					cout << "\nError! Can't add a new record";
			}
			else
				cout << "\nError! Too many symbs!";
		} break;

		case '2': //Апдейт записи
		{
			DWORD dwIdToModify;
			cout << "\nInput record ID to update: ";
			cin >> dwIdToModify;
			if (dwIdToModify >= dwCountRecords)
			{
				cout << "\nError! No such ID";
				continue;
			}
			else
			{
				if (!UpdateRec(hRecordFile, dwIdToModify))
				{
					cout << "\nError! Can't update record";
					continue;
				}
			}
		} break;

		case '3': //Удалить запись
		{
			DWORD dwIdToDelete;
			cout << "\nInput record ID to delete: ";
			cin >> dwIdToDelete;
			if (dwIdToDelete >= dwCountRecords)
			{
				cout << "\nError! No such ID";
				continue;
			}
			else
			{
				if (!DeleteRec(hRecordFile, dwIdToDelete, &dwCountRecords, &dwFileSize))
					cout << "\nError! Can't delete record";
				continue;
			}
		} break;

		case '4': //Вывод заголовка и прочей инфы
		{
			if (!ShowRec(hRecordFile, dwCountRecords))
			{
				cout << "\nError! Can't show header";
				continue;
			}
		} break;

		case '5': //выход
		{
			CloseHandle(hRecordFile);
			return 0;
		} break;

		default: {
			cout << "\nWrong key value!";
		} continue;
		}
	}
}