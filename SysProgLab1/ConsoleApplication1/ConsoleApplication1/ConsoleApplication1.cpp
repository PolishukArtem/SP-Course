#include "pch.h"
#include <iostream>
#include "windows.h"
#include "stdio.h"
#include "string.h"
#include <locale.h>

void PrintError();
void PrintSystemInfo();

int main( int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	printf("*************** KEYS ***************\n");
	printf("-e - show error code and description\n");
	printf("-s - show system information\n");
	if (argc != 2)
	{
		printf_s("No input! Try again\n");
	}
	else
	{
		if (strcmp(argv[1], "-e") == 0)
		{
			PrintError();
		}
		else if (strcmp(argv[1], "-s") == 0)
		{
			PrintSystemInfo();
		}
		else
		{
			printf("No such key. Input the right one and try again!\n");
		}
	}
	system("pause");
	return 0;
}

void PrintError()
{
	MEMORYSTATUS memory_status;
	GlobalMemoryStatus(&memory_status);
	if (LocalAlloc(LPTR, memory_status.dwTotalPhys * (3)) == NULL) {
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
}

void PrintSystemInfo()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	printf("*********** SYSTEM INFO ***********\n");
	printf("Oem Id: %u\n", system_info.dwOemId);
	printf("The processor architecture: %u\n", system_info.wProcessorArchitecture);
	printf("Reserverd member for future use: %u\n", system_info.wReserved);
	printf("Page size: %u\n", system_info.dwPageSize);
	printf("MIN application address: %lx\n", system_info.lpMinimumApplicationAddress);
	printf("MAX application address: %lx\n", system_info.lpMaximumApplicationAddress);
	printf("Active processor mask: %u\n", system_info.dwActiveProcessorMask);
	printf("Number of processors: %u\n", system_info.dwNumberOfProcessors);
	printf("Processor type: %u\n", system_info.dwProcessorType);
	printf("Allocation granularity address: %u\n", system_info.dwAllocationGranularity);
}