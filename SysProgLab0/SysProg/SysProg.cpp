// SysProg.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <limits.h>

int main()
{
	printf("Char:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(char), SCHAR_MIN, SCHAR_MAX);
	printf("Bool:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(bool), 0, __BOOL_DEFINED);
	printf("Unsigned char:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(char), 0, UCHAR_MAX);
	printf("Short:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(short), SHRT_MIN, SHRT_MAX);
	printf("Unsigned short:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(short), 0, USHRT_MAX);
	printf("Integer:\n %d bytes | Min. value: %d | Max. value: %d\n", sizeof(int), INT32_MIN, INT32_MAX);
	printf("Unsigned int:\n %d bytes | Min. value: %d | Max. value: %lu\n", sizeof(int), 0, UINT32_MAX);
	printf("Long:\n %d bytes | Min. value: %lld | Max. value: %lld\n", sizeof(long long), LLONG_MIN, LLONG_MAX);
	printf("Unsigned long:\n %d bytes | Min. value: %ld | Max. value: %llu\n", sizeof(long long), 0, ULLONG_MAX);
	printf("Double:\n %d bytes | Min. value: %fl | Max. value: %fl\n", sizeof(double), DBL_MIN, DBL_MAX);
	printf("Float:\n %d bytes | Min. value: %fl | Max. value: %fl\n", sizeof(float), FLT_MIN, FLT_MAX);
	system("pause");
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
