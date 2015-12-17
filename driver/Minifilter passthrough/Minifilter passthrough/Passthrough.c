#include <fltKernel.h>

#include "Passthrough.h"

PFLT_FILTER gFilterHandle;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(RegistryPath);

	KdPrint(("Passthrough!DriverEntry: entered\n"));

	status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);

	FLT_ASSERT(NT_SUCCESS(status));

	if (!NT_SUCCESS(status))
	{
		FltUnregisterFilter(gFilterHandle);
	}

	return status;
}