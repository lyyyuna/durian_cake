// Test.cpp : Defines the entry point for the console application.
//

 
#include <Windows.h>
#include <stdio.h>

int main()
{
	HANDLE hDevice = CreateFile(
		L"\\\\.\\HelloDDK",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
		);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Open device failed!\n");
		return 1;
	}

	OVERLAPPED overlap1 = { 0 };
	OVERLAPPED overlap2 = { 0 };

	UCHAR buffer[10];
	ULONG ulRead;

	BOOL bRead = ReadFile(hDevice, buffer, 10, &ulRead, &overlap1);
	if (!bRead && GetLastError() == ERROR_IO_PENDING)
	{
		printf("the operation is pending.\n");
	}

	bRead = ReadFile(hDevice, buffer, 10, &ulRead, &overlap2);
	if (!bRead && GetLastError() == ERROR_IO_PENDING)
	{
		printf("the operation is pending!\n");
	}

	Sleep(2000);
	CloseHandle(hDevice);

	return 0;
}

