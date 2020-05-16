#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "windows.h"

	void ArchieveFile(TCHAR* tFilePath, TCHAR* tArchivePath);
	void GetFileFromArchieve(TCHAR* tArchivePath, TCHAR* tExtractArchivePath);
	void Create7z(TCHAR* tCmdForNewProcess);
	void ShowError();

using namespace std;

//Функция архивации файла
void ArchieveFile(TCHAR* tFilePath, TCHAR* tArchivePath)
{
	TCHAR tCmdForNewProcess[MAX_PATH] = { '\0' };

		printf("Command: ");
		snprintf(tCmdForNewProcess, MAX_PATH, "%s%s %s", TEXT("7z.exe a "), tArchivePath, tFilePath);
		cout << tCmdForNewProcess;

	Create7z(tCmdForNewProcess);
}

//Функция создания 7z файла
void Create7z(TCHAR* tCmdForNewProcess)
{
	HANDLE hServer = { NULL };
	HANDLE hClient = { NULL };
	SECURITY_ATTRIBUTES sa;

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

			if ((!CreatePipe(&hClient, &hServer, &sa, NULL)) ||
				(hServer == INVALID_HANDLE_VALUE) || (hClient == INVALID_HANDLE_VALUE))
				{
					CloseHandle(hClient);
					CloseHandle(hServer);
					ShowError();
					return;
				}

					STARTUPINFO si = { NULL };
					si.cb = sizeof(si);
					si.dwFlags |= STARTF_USESTDHANDLES;
					si.hStdError = hServer;
					si.hStdOutput = hServer;

						PROCESS_INFORMATION pi = { NULL };

							if (!CreateProcessA(NULL, tCmdForNewProcess, NULL, NULL, TRUE,
								ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
								{
									CloseHandle(hClient);
									CloseHandle(hServer);
									ShowError();
									return;
								}

									WaitForSingleObject(pi.hProcess, INFINITE);
									TerminateProcess(pi.hProcess, 0);

										CloseHandle(hServer);

											CloseHandle(pi.hThread);
											CloseHandle(pi.hProcess);

											DWORD dwReadedBytes = 0;
											TCHAR tBuff[1024] = { '\0' };
											BOOLEAN bAlrightFlag = TRUE;

	while (ReadFile(hClient, tBuff, sizeof(tBuff) - 1, &dwReadedBytes, NULL) || (dwReadedBytes != 0))
		{
			printf("%s", tBuff);
			if (strstr(tBuff, "Operation is SUCCESSFULL") == NULL) bAlrightFlag = FALSE;
			memset(tBuff, '\0', sizeof(tBuff));
		}

			cout << "Parent process executioning: ";
			if (!bAlrightFlag)
				cout << "ERROR" << endl;
			else
				cout << "SUCCESS" << endl;

					CloseHandle(hClient);
}

//Функция разорхивирования
void GetFileFromArchieve(TCHAR* tArchivePath, TCHAR* tExtractArchivePath)
{
	TCHAR tCmdForNewProcess[MAX_PATH] = { '\0' };

		printf("Command: ");
		snprintf(tCmdForNewProcess, MAX_PATH, "%s%s -o\"%s\"", TEXT("7z.exe x "), tArchivePath, tExtractArchivePath);
		cout << tCmdForNewProcess;

	Create7z(tCmdForNewProcess);
}

int main()
{
	CHAR cKey;
	for (;;)
	{
		cout << "\n============= MENU =============" << endl;
		cout << " \t1 - Archieve" << endl;
		cout << " \t2 - Get from archieve" << endl;
		cout << " \t3 - Shut down" << endl;
		cout << "Input key value: ";
		cin >> cKey;

				switch (cKey)
				{

						case '1': //Archieve [PATH\FILE] into [PATH]
						{
							TCHAR tPathToFiles[MAX_PATH] = { '\0' };
							TCHAR tPathToNewArchive[MAX_PATH] = { '\0' };
							cout << "\nInput file path to archieve: ";
							scanf_s("%s", tPathToFiles, MAX_PATH);
							cout << "Input path to save archieve: ";
							scanf_s("%s", tPathToNewArchive, MAX_PATH);

							ArchieveFile(tPathToFiles, tPathToNewArchive);
						} break;

						case '2': //Get file from [PATH\FILE] archieve into [PATH] 
						{
							TCHAR tPathToArchive[MAX_PATH] = { '\0' };
							TCHAR tPathToExtractArchive[MAX_PATH] = { '\0' };
							cout << "\nInput path to archieve file: ";
							scanf_s("%s", tPathToArchive, MAX_PATH);
							cout << "Input path to get file from archieve: ";
							scanf_s("%s", tPathToExtractArchive, MAX_PATH);

							GetFileFromArchieve(tPathToArchive, tPathToExtractArchive);
						} break;

						case '3': //Shut down
						{ return 0; } break;


						default: 
						{
							cout << "\nError! Wrong key value!";
						} continue;
				}
	}
}

//Функция вывода ошибки
void ShowError()
{
	LPVOID lpMsgBuf;
	DWORD dwLastError = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

			printf("\n%s\n", lpMsgBuf);
			LocalFree(lpMsgBuf);

	return;
}