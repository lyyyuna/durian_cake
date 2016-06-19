// Test_iotimer.cpp : Defines the entry point for the console application.
//



#include <Windows.h>
#include <stdio.h>

#include <winioctl.h>
#include "..\IoTimerTest\Header1.h"

int main()
{
	HANDLE hDevice =
		CreateFile(L"\\\\.\\HelloDDK",
		GENERIC_READ | GENERIC_WRITE,
		0,		// share mode none
		NULL,	// no security
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);		// no template

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to obtain file handle to device: "
			"%s with Win32 error code: %d\n",
			"MyWDMDevice", GetLastError());
		return 1;
	}

	DWORD dwOutput;

	DeviceIoControl(hDevice, IOCTL_START_TIMER, NULL, 0, NULL, 0, &dwOutput, NULL);

	Sleep(10000);

	DeviceIoControl(hDevice, IOCTL_STOP, NULL, 0, NULL, 0, &dwOutput, NULL);

	CloseHandle(hDevice);

	return 0;
}