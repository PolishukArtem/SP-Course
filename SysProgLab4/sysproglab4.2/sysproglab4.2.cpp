// sysproglab4.2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "windows.h"

#define ARRAY_VALUES_MIN 10
#define ARRAY_VALUES_MAX 100
#define ARRAY_SIZE 7 

DWORD WINAPI StartNewThread(LPVOID param);

void GenereteArray(int* arr);
int CalculateLargestDivisor(int num);
int CalculateSumOfArrayValues(int* arr);
void PrintArray(int* arr, int length);

using namespace std;

int gdwTlsIndex;
CRITICAL_SECTION gCriticalSection;
HANDLE* threadHandles;


DWORD WINAPI StartNewThread(LPVOID param)
{
	EnterCriticalSection(&gCriticalSection); //Пропуск только одного потока на выполнение
	int maxDivisors[ARRAY_SIZE] = { 1 }; //выделяем память под значения
	TlsSetValue(gdwTlsIndex, maxDivisors);

		//Вывод всей инфы про то что в потоке
		cout << "\n+===================| ID - " << GetCurrentThreadId() << " |===================-" << endl;
		cout << "|Source array:\t\t ";
		PrintArray((int*)param, ARRAY_SIZE);

			for (int i = 0; i < ARRAY_SIZE; i++)
				maxDivisors[i] = CalculateLargestDivisor(((int*)param)[i]);
			cout << "|Greatest divisor:\t";
			PrintArray((int*)TlsGetValue(gdwTlsIndex), ARRAY_SIZE);

		cout << "|Sum of all elements:\t\t\t " << CalculateSumOfArrayValues((int*)param) << endl;
		cout << "+====================================================-" << endl;
	//Разрешение другим процессам на выполнение
	LeaveCriticalSection(&gCriticalSection);
	return 0;
}

//функция генерации массива
void GenereteArray(int* arr)
{
	int randRange = ARRAY_VALUES_MAX - ARRAY_VALUES_MIN;

		for (int i = 0; i < ARRAY_SIZE; i++)
			arr[i] = (rand() % randRange) + ARRAY_VALUES_MIN;
}

//функция подсчёта максимального делителя
int CalculateLargestDivisor(int num)
{
	int halfNum = ceil(num / 2), res = 1;

		for (int i = 2; i <= halfNum; i++)
			{
				if (num % i == 0)
					res = i;
			}

	return res;
}

//функция подсчёта суммы
int CalculateSumOfArrayValues(int* arr)
{
	int sum = arr[0];

		for (int i = 1; i < ARRAY_SIZE; i++)
			sum += arr[i];
				
	return sum;
}

//функция вывода
void PrintArray(int* arr, int length)
{
	for (int i = 0; i < length; i++)
		cout << " " << arr[i] << " ";

	cout << endl;
}


int main()
{
	int countOfThreads = 0;
	cout << "Set up number of streams: ";
	cin >> countOfThreads;

		if (countOfThreads > 0)
		{
			int** arrOfArraysForNewThread;
			arrOfArraysForNewThread = (int**)malloc(sizeof(int**) * countOfThreads);

				for (int i = 0; i < countOfThreads; i++)
					{
						arrOfArraysForNewThread[i] = (int*)malloc(sizeof(int) * ARRAY_SIZE);
						GenereteArray(arrOfArraysForNewThread[i]);
					}

						InitializeCriticalSection(&gCriticalSection);
						gdwTlsIndex = TlsAlloc();
						threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * countOfThreads);
						DWORD* dwThreadsID = (DWORD*)malloc(sizeof(DWORD) * countOfThreads);
		
			for (int i = 0; i < countOfThreads; i++)
				threadHandles[i] = CreateThread(NULL, NULL, StartNewThread,
					arrOfArraysForNewThread[i], NULL, &dwThreadsID[i]);

		WaitForMultipleObjects(countOfThreads, threadHandles, TRUE, INFINITE);
		}
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
