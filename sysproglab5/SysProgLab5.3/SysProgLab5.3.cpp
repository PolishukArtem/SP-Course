// SysProgLab5.3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <ctime>
#include "iostream"
#include "windows.h"
#include "locale.h"

#define ARRAY_SIZE 100000000
#define ARRAY_VALUES_MIN 0
#define ARRAY_VALUES_MAX 50000
#define COUNT_THREADS 3

DWORD WINAPI NewStreamWITHCritSect(LPVOID param);
DWORD WINAPI NewStreamWITHOUTCritSect(LPVOID param);
void GenereteArr(int* iArray);
int SumArr(int* iArray); 
void MinMaxArr(int* iArray);
LONG64 AvgArr(int* iArray);
CRITICAL_SECTION CritSect;
HANDLE* hThreads;
int iArray[ARRAY_SIZE]{ 0 };

using namespace std;

void GenereteArr(int* iArray)
{
	DWORD randRange = ARRAY_VALUES_MAX - ARRAY_VALUES_MIN;
	for (DWORD i = 0; i < ARRAY_SIZE; i++)
		iArray[i] = (rand() % randRange) + ARRAY_VALUES_MIN;
}

int SumArr(int* iArray)
{
	int sum = iArray[0];
	for (int i = 1; i < ARRAY_SIZE; i++)
		sum += iArray[i];
	cout << "SUM: " << sum << endl;
	return sum;
}

LONG64 AvgArr(int* iArray)
{
	LONG64 res = 0;
	for (int i = 0; i < ARRAY_SIZE; i++)
		res += iArray[i];
	res /= ARRAY_SIZE;
	cout << "AVG: " << res << endl;
	return res;
}

void MinMaxArr(int* iArray)
{
	int max = iArray[0], min = iArray[0];
	for (int i = 1; i < ARRAY_SIZE; i++)
	{
		if (max < iArray[i]) max = iArray[i];
		if (min > iArray[i]) min = iArray[i];
	}
	cout << "MAX: " << max << "  MIN: " << min << endl;
}

int main()
{
	clock_t start, end;

	InitializeCriticalSection(&CritSect);

	hThreads = (HANDLE*)malloc(sizeof(HANDLE) * COUNT_THREADS);

	GenereteArr(iArray);
 
	cout << "===============| With mutual exclusion |===============" << endl;
	start = clock();
	for (int i = 0; i < COUNT_THREADS; i++)
	{
		hThreads[i] = CreateThread(NULL, NULL,
			NewStreamWITHCritSect,
			(LPVOID)i, NULL, NULL);
	}

	WaitForMultipleObjects(COUNT_THREADS, hThreads, TRUE, INFINITE);
	end = clock();
	cout << "Work time: " << end - start << " ms" << endl;

	cout << "\n\n===============| Without mutual exclusion |===============" << endl;
	start = clock();
	for (int i = 0; i < COUNT_THREADS; i++)
	{
		hThreads[i] = CreateThread(NULL, NULL,
			NewStreamWITHOUTCritSect,
			LPVOID(i), NULL, NULL);
	}

	WaitForMultipleObjects(COUNT_THREADS, hThreads, TRUE, INFINITE);
	end = clock();
	cout << "Work time: " << end - start << " ms" << endl;

	for (int i = 0; i < COUNT_THREADS; i++)
		CloseHandle(hThreads[i]);
	DeleteCriticalSection(&CritSect);
	return 0;
}

DWORD WINAPI NewStreamWITHCritSect(LPVOID param)
{
	switch ((int)param)
	{
	case 0:
	{
		EnterCriticalSection(&CritSect);
		SumArr(iArray);
		LeaveCriticalSection(&CritSect);
	} break;
	case 1:
	{
		EnterCriticalSection(&CritSect);
		MinMaxArr(iArray);
		LeaveCriticalSection(&CritSect);
	} break;
	case 2:
	{
		EnterCriticalSection(&CritSect);
		AvgArr(iArray);
		LeaveCriticalSection(&CritSect);
	} break;
	default: break;
	}
	return 0;
}

DWORD WINAPI NewStreamWITHOUTCritSect(LPVOID param)
{
	switch ((int)param)
	{
	case 0: { SumArr(iArray); } break;
	case 1: { MinMaxArr(iArray); } break;
	case 2: { AvgArr(iArray); } break;
	default: break;
	}
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
