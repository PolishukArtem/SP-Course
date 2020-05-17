// 5.2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <ctime>
#include "windows.h" 
#include "tchar.h"

#define MIN_THREAD_SLEEPING_TIME 1
#define MAX_THREAD_SLEEPING_TIME 3
#define FILE_HANDLES_COUNT 3
#define BUFF_FOR_WRITING_TO_LOG_FILE_SIZE 32

DWORD WINAPI LaunchNewStream(LPVOID param);

typedef struct {
	DWORD dwId;
	DWORD dwSleepingTime;
} NewThreaParams, *LPNewThreadParams;

using namespace std;

HANDLE hFilesAccessSemaphore;
CONST TCHAR ctsFileForSaving[] = L"log.txt";

int main()
{
	HANDLE* hStreams;
	DWORD dwCountStreams;

		cout << "Number of file pointers: " << FILE_HANDLES_COUNT << endl;
		cout << "Input the number of threads: ";
		scanf_s("%d", &dwCountStreams);
		cout << endl;

	hStreams = (HANDLE*)malloc(sizeof(HANDLE) * dwCountStreams);

	hFilesAccessSemaphore = CreateSemaphore(NULL, FILE_HANDLES_COUNT, FILE_HANDLES_COUNT, L"FilesSemaphore");
		if (!hFilesAccessSemaphore || hFilesAccessSemaphore == INVALID_HANDLE_VALUE)
			{
				cout << "Error! Can't create a semaphore.\n";
				return 1;
			}

				DWORD dwRandRange = MAX_THREAD_SLEEPING_TIME - MIN_THREAD_SLEEPING_TIME + 1;

					for (int i = 0; i < dwCountStreams; i++)
					{
							srand(clock());
							NewThreaParams param = { 0 };
							param.dwId = i;
							param.dwSleepingTime = (rand() % dwRandRange) + MIN_THREAD_SLEEPING_TIME;
							hStreams[i] = CreateThread(NULL, NULL, LaunchNewStream, (LPVOID)&param, NULL, NULL);
								if ((!hStreams[i]) || (hStreams[i] == INVALID_HANDLE_VALUE))
								{
									cout << "Error! Can't launch stream: " << i << endl;
									return 2;
								}
						Sleep(100);
					}

			WaitForMultipleObjects(dwCountStreams, hStreams, TRUE, INFINITE);

		CloseHandle(hFilesAccessSemaphore);
	for (int i = 0; i < dwCountStreams; i++)
		CloseHandle(hStreams[i]);
	free(hStreams);

	CloseHandle(hFilesAccessSemaphore);
	cout << "\nAll streams have done their work" << endl;
	getchar();
	return 0;
}

DWORD WINAPI LaunchNewStream(LPVOID param)
{
	DWORD dwThreadUserId = ((LPNewThreadParams)param)->dwId;
	DWORD dwThreadUserSleepingTime = ((LPNewThreadParams)param)->dwSleepingTime;
	HANDLE hFileForSaving;
	DWORD dwWaitingRes = ERROR_SUCCESS;
	clock_t  startTime = clock(), endTime;

	WaitForSingleObject(hFilesAccessSemaphore, INFINITE);

	printf_s("Thread %d get resourse\n", dwThreadUserId);
	printf_s("Thread %d will sleep for %d sec\n", dwThreadUserId, dwThreadUserSleepingTime);
	Sleep(dwThreadUserSleepingTime * 1000);

		if ((hFileForSaving = CreateFile(ctsFileForSaving, GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
				{
					cout << "Error with stream " << dwThreadUserId << ", can't open file" << endl;
					ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
					ExitThread(1);
				}

					if ((dwWaitingRes = SetFilePointer(hFileForSaving, 0, NULL, FILE_END)) == INVALID_SET_FILE_POINTER)
						{
							cout << "Error with stream " << dwThreadUserId << ", can't set pointer on file" << endl;
							ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
							ExitThread(2);
						}

							endTime = clock();

	TCHAR buff[BUFF_FOR_WRITING_TO_LOG_FILE_SIZE]{ '\0' };
	DWORD dwWritenBytes;

	_sntprintf_s(buff, BUFF_FOR_WRITING_TO_LOG_FILE_SIZE, L"Thread %d ends in %d sec.\n", dwThreadUserId, ((endTime - startTime) / 1000));

		if (!WriteFile(hFileForSaving, buff, _tcslen(buff) * sizeof(TCHAR), &dwWritenBytes, NULL))
			{
				cout << "Error with stream " << dwThreadUserId << ", cannot write to file" << endl;
				ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
				ExitThread(3);
			}

	CloseHandle(hFileForSaving);
	printf_s("Thread %d complete, [%d] bytes writen to file\n", dwThreadUserId, dwWritenBytes);

	ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
	return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
