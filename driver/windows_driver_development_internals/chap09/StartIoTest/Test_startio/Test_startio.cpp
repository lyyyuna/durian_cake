// Test_startio.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include <process.h>


UINT WINAPI Thread(LPVOID context)
{
	printf("enter thread\n");

	OVERLAPPED overlap = { 0 };
	overlap.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	UCHAR buffer[10];
	ULONG ulRead;

	BOOL bRead = ReadFile(*(PHANDLE)context, buffer, 10, &ulRead, &overlap);
	WaitForSingleObject(overlap.hEvent, INFINITE);

	return 0;
}

int main()
{
	HANDLE hDevice = CreateFile(
		L"\\\\.\\HelloDDK",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
		);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("open device failed\n");
	}

	HANDLE hThread[2];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, Thread, &hDevice, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, Thread, &hDevice, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	CloseHandle(hDevice);
	return 0;
}


