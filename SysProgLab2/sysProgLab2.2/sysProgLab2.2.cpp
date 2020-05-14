// sysProgLab2.2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "locale.h" 
#include <stdio.h>
#include "TCHAR.h"
#include "windows.h"
#include <shlwapi.h>
#include <wchar.h>

using namespace std;

bool PrintDirectory(TCHAR tCurrentDirectory[]);
bool ChangeDirectory(TCHAR* tCurrentDirectory, TCHAR* tNewPath);
bool CopySomeFile(TCHAR* tExistFilePath, TCHAR* tNewFilePath);
bool CreateSomeDirectory(TCHAR* tNewPath);
bool DeleteEmptyDirectoryOrFile(TCHAR* tPathToDeleting);
bool PrintInfoAboutFile(TCHAR* tFilePath);
void ShowError();
void FixPath(TCHAR tNewPath[]);

//функция изменения текущей директории
bool ChangeDirectory(TCHAR* tCurrentDirectory, TCHAR* tNewPath)
{
	cout << "\nOld path: ";
	wcout << tCurrentDirectory << endl;
	FixPath(tNewPath);
	DWORD dw = GetFileAttributes(tNewPath);
	if (dw != FILE_ATTRIBUTE_DIRECTORY)
	{
		cout << "Your path isn't for a directory" << endl;
		return false;
	}
	wcscpy(tCurrentDirectory, tNewPath);
	cout << "New path: ";
	wcout << tCurrentDirectory << endl;
	return true;
}

//функция вывода файлов в текущей папке
bool PrintDirectory(TCHAR tCurrentDirectory[])
{
	TCHAR tBuff[MAX_PATH];
	WIN32_FIND_DATAW id;
	SYSTEMTIME st;
	_sntprintf(tBuff, MAX_PATH, L"%s\\*\0", tCurrentDirectory);
	HANDLE hFiles = FindFirstFile(tBuff, &id);
	if (hFiles != INVALID_HANDLE_VALUE)
	{
		wcout << "\n" << tCurrentDirectory << endl;
		do
		{
			if ((wcscmp(L".", id.cFileName) == 0) || (wcscmp(L"..", id.cFileName) == 0))
				continue;
			wcout << " - " << id.cFileName;
			cout << "  (File size: " << ((id.nFileSizeHigh * MAXDWORD) + id.nFileSizeLow) / 1024 << " KB  ";
			if (FileTimeToSystemTime(&id.ftCreationTime, &st))
				wprintf_s(L"Creation time: %d-%d-%d, %d:%d)\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
		} while (FindNextFile(hFiles, &id));
		FindClose(hFiles);
		return true;
	}
	return false;
}

//функция создания директории
bool CreateSomeDirectory(TCHAR* tNewPath)
{
	FixPath(tNewPath);
	if (!CreateDirectory(tNewPath, NULL))
		return false;
	return true;
}

//функция удаления
bool DeleteEmptyDirectoryOrFile(TCHAR* tPathToDeleting) // function delete empty directory or file
{
	FixPath(tPathToDeleting);
	if (!RemoveDirectory(tPathToDeleting))
	{
		if (!DeleteFile(tPathToDeleting))
			return false;
	}
	return true;
}

//функция копирования файла
bool CopySomeFile(TCHAR* tExistFilePath, TCHAR* tNewFilePath)
{
	FixPath(tExistFilePath);
	DWORD dw = GetFileAttributes(tExistFilePath);
	if (dw == INVALID_FILE_ATTRIBUTES)
	{
		cout << "No such file!" << endl;
		return false;
	}
	FixPath(tNewFilePath);
	if (!CopyFile(tExistFilePath, tNewFilePath, TRUE))
	{
		ShowError();
		return false;
	}
	return true;
}

//вывод инфы про файл
bool PrintInfoAboutFile(TCHAR* tFilePath)
{
	WIN32_FIND_DATA id;
	SYSTEMTIME st;
	FixPath(tFilePath);
	if (!FindFirstFile(tFilePath, &id))
		return false;
	cout << "\n\t File info: "; wcout << id.cFileName << endl;
	cout << "Alter file name: "; wcout << id.cAlternateFileName << endl;
	cout << "file size: " << ((id.nFileSizeHigh * MAXDWORD) + id.nFileSizeLow) / 1024 << " KB" << endl;
	if (FileTimeToSystemTime(&id.ftCreationTime, &st))
		wprintf_s(L"Create date: %d-%d-%d, %d:%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	if (FileTimeToSystemTime(&id.ftLastAccessTime, &st))
		wprintf_s(L"Last access date: %d-%d-%d, %d:%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	if (FileTimeToSystemTime(&id.ftLastWriteTime, &st))
		wprintf_s(L"Last record date: %d-%d-%d, %d:%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	cout << "File attributes: " << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE)
		cout << " - Archive" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_COMPRESSED)
		cout << " - Compressed" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_HIDDEN)
		cout << " - Hidden" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_NORMAL)
		cout << " - Normal" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_OFFLINE)
		cout << " - Unenable file data" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_READONLY)
		cout << " - Readonly";
	if (id.dwFileAttributes == FILE_ATTRIBUTE_SYSTEM)
		cout << " - System" << endl;
	if (id.dwFileAttributes == FILE_ATTRIBUTE_TEMPORARY)
		cout << " - Temporary" << endl;
	return true;
}

int main()
{
	TCHAR tCurrentPath[MAX_PATH]{ '\0' };
	GetCurrentDirectory(MAX_PATH, tCurrentPath);
	TCHAR tCommand[MAX_PATH]{ '\0' };
	TCHAR* pch;
	TCHAR tCmd[MAX_PATH]{ '\0' };
	TCHAR tFirstPath[MAX_PATH]{ '\0' };
	TCHAR tSecondPath[MAX_PATH]{ '\0' };

	for (;;) {
		cout << "\n============= File Manager =============" << endl;
		cout << "changedir [directoryPath] - Change directory" << endl;
		cout << "printdir - Show curent directory's files" << endl;
		cout << "copyfile [file] [newPathToFile] - Copy file" << endl;
		cout << "createdir [directoryPath] - Create new directory" << endl;
		cout << "delete [directoryPath/filePath] - Delete directory" << endl;
		cout << "printinfo [file] - Show information about file" << endl;

		cout << "Input command to work with file manager: " << endl;

		fgetws(tCommand, MAX_PATH, stdin);
		pch = _wcstok(tCommand, L" ");
		int i = 0;
		while (pch != NULL) {
			if (i == 0)_tcscpy(tCmd, pch);
			if (i == 1)_tcscpy(tFirstPath, pch);
			if (i == 2)_tcscpy(tSecondPath, pch);
			pch = _wcstok(NULL, L" ");
			i++;
		}
		FixPath(tCmd);
		if (_tcscmp(L"changedir", tCmd) == 0)
		{
			if (!ChangeDirectory(tCurrentPath, tFirstPath))
				ShowError();
		}
		else if (_tcscmp(L"printdir", tCmd) == 0)
		{
			if (!PrintDirectory(tCurrentPath))
				ShowError();
		}
		else if (_tcscmp(L"copyfile", tCmd) == 0)
		{
			if (!CopySomeFile(tFirstPath, tSecondPath))
				ShowError();
		}
		else if (_tcscmp(L"createdir", tCmd) == 0)
		{
			if (!CreateSomeDirectory(tFirstPath))
				ShowError();
		}
		else if (_tcscmp(L"delete", tCmd) == 0)
		{
			if (!DeleteEmptyDirectoryOrFile(tFirstPath))
				ShowError();
		}
		else if (_tcscmp(L"printinfo", tCmd) == 0)
		{
			if (!PrintInfoAboutFile(tFirstPath))
				ShowError();
		}
		else {
			cout << "No such command" << endl;
			continue;
		}
	}
}

//функция фикса пути
void FixPath(TCHAR tNewPath[])
{
	if (wcschr(tNewPath, '\n'))
		tNewPath[(wcschr(tNewPath, '\n') - tNewPath)] = '\0'; //изменяет последний "\n" на "\0"
}

//функция вывода ошибки
void ShowError()
{
	LPVOID lpMsgBuf;
	DWORD dwLastError = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwLastError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("\n%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	return;
}