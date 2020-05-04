#include "pch.h"
#include <iostream>
#include "windows.h"
#include "stdio.h"
#include "string.h"
#include <locale.h>

void ANSI_To_Unicode(LPCSTR);
void Unicode_To_ANSI(LPCSTR);
void ShowError();

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");
	printf("Введите ключ необходимой операции:\n");
	printf("\t* ключи исходной кодировки файла:\n\t\t-a – ANSI файл;\n \t\t-u – Unicode файл.\n");
	printf("\t* Полный путь к файлу с занесенными данными.\n");
	printf("Пример: -x 'C:\\path\\to\\file' \n");

		if (argc != 3)
			{
				printf("Ошибка! При вводе данных!\n");
				printf("\t* ключи исходной кодировки файла:\n\t\t-a – ANSI файл;\n \t\t-u – Unicode файл.\n");
				printf("\t* Полный путь к файлу с занесенными данными.\n");
				printf("Пример: -x 'C:\\path\\to\\file' \n");
				return 0;
			}
		else
		{
			if (strcmp(argv[1], "-a") == 0)
				ANSI_To_Unicode(argv[2]);
			else if (strcmp(argv[1], "-u") == 0)
				Unicode_To_ANSI(argv[2]);
			else
				printf("Ошибка ввода ключа\n");
		}
	return 0;
}

void ANSI_To_Unicode(LPCSTR fileName)
{
	WCHAR* unicodeBuff;
	DWORD countRdChars, countWrWChars;
	HANDLE handleSourceFile, handleDestinationFile;
	CHAR ansiBuff[1024];
	BOOL resultFile = true;
	CHAR nameDestinationFile[] = "..\\ANSI_Text_To_Unicode.txt";
	printf("\nРезультат можно увидеть -> %s\n", nameDestinationFile);

			handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (handleSourceFile == INVALID_HANDLE_VALUE)
					{
						ShowError();
						exit(EXIT_FAILURE);
					}

			handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (handleDestinationFile == INVALID_HANDLE_VALUE)
					{
						ShowError();
						exit(EXIT_FAILURE);
					}

			while (((resultFile = ReadFile(handleSourceFile, &ansiBuff, sizeof(ansiBuff), &countRdChars, NULL)) != false)
				&& (countRdChars != 0))
					{
						int sizeToAllocate = MultiByteToWideChar(CP_UTF8, 0, ansiBuff, countRdChars, NULL, 0);
						unicodeBuff = (WCHAR*)calloc(sizeToAllocate, sizeof(WCHAR));
						MultiByteToWideChar(CP_UTF8, 0, ansiBuff, countRdChars, unicodeBuff, sizeToAllocate);
						if ((!WriteFile(handleDestinationFile, unicodeBuff, sizeToAllocate * sizeof(WCHAR), &countWrWChars, NULL)) ||
							(countWrWChars != sizeToAllocate * sizeof(WCHAR)))
								{
									ShowError();
									exit(EXIT_FAILURE);
								}
						free(unicodeBuff);
					}

				if (resultFile == false && countRdChars != 0)
					{
						ShowError();
						exit(EXIT_FAILURE);
					}

			CloseHandle(handleSourceFile);
			CloseHandle(handleDestinationFile);
}

void Unicode_To_ANSI(LPCSTR fileName)
{
	WCHAR* unicodeBuff;
	DWORD countReadBytes, countWriteBytes;
	HANDLE handleSourceFile, handleDestinationFile;
	CHAR rawBytesBuff[1024];
	CHAR* ansiBuff; 
	BOOL resultFile = true;
	CHAR nameDestinationFile[] = "..\\Unicode_Text_To_ANSI.txt";
	printf("\nРезультат можно увидеть -> %s\n", nameDestinationFile);

			handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				
				if (handleSourceFile == INVALID_HANDLE_VALUE)
					{
						ShowError();
						exit(EXIT_FAILURE);
					}
			handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					
				if (handleDestinationFile == INVALID_HANDLE_VALUE)
						{
							ShowError();
							exit(EXIT_FAILURE);
						}
							if (!ReadFile(handleSourceFile, &rawBytesBuff, 2, &countReadBytes, NULL))
								{
									ShowError();
									exit(EXIT_FAILURE);
								}

			while (((resultFile = ReadFile(handleSourceFile, &rawBytesBuff, sizeof(rawBytesBuff), &countReadBytes, NULL)) != false)
				&& (countReadBytes != 0))
					{
						int unicodeLenght = MultiByteToWideChar(CP_UTF8, 0, rawBytesBuff, countReadBytes, NULL, 0);
						unicodeBuff = (WCHAR*)calloc(unicodeLenght, sizeof(WCHAR));
						MultiByteToWideChar(CP_UTF8, 0, rawBytesBuff, countReadBytes, unicodeBuff, unicodeLenght);
						int asciLenght = WideCharToMultiByte(CP_ACP, 0, unicodeBuff, unicodeLenght, NULL, 0, NULL, NULL);
						ansiBuff = (CHAR*)calloc(asciLenght, sizeof(CHAR));
						WideCharToMultiByte(CP_ACP, 0, unicodeBuff, unicodeLenght, ansiBuff, asciLenght, NULL, NULL);
						if ((!WriteFile(handleDestinationFile, ansiBuff, asciLenght, &countWriteBytes, NULL)) || (countWriteBytes != asciLenght))
							{
								ShowError();
								exit(EXIT_FAILURE);
							}
						free(unicodeBuff);
						free(ansiBuff);
					}

			if (resultFile == false && countReadBytes != 0)
				{
					ShowError();
					exit(EXIT_FAILURE);
				}

			CloseHandle(handleSourceFile);
			CloseHandle(handleDestinationFile);
}
void ShowError()
{
	setlocale(LC_ALL, "rus");
	LPVOID error_message;
	DWORD error_code = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&error_message, 0, NULL);
	printf("*************** ERROR ***************\n");
	wprintf(L"Error code: 0x%x\n", error_code);
	wprintf(L"About error: %s\n", (char*)error_message);
}