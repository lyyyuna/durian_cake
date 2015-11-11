#include <ntddk.h>

VOID DriverUnload(PDRIVER_OBJECT driver)
{
	DbgPrint("first: our driver is unloading.\r\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	_asm int 3

	DbgPrint("first: hello, my salary! lalalalalalalalalalasdfsdfwefwe\n\r");

	driver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}