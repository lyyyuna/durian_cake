#include "Header.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	KdPrint(("enter driver entry\n"));

	pDriverObject->DriverUnload = HelloDDKUnload;

	for (int i = 0; i < arraysize(pDriverObject->MajorFunction); i++)
	{
		pDriverObject->MajorFunction[i] = HelloDDKDispatchRoutin;
	}

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloDDKDeviceIOControl;

	NTSTATUS status = CreateDevice(pDriverObject);

	KdPrint(("leave driver entry\n"));

	return status;
}

#pragma LOCKEDCODE
VOID OnTimer(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PVOID Context
	)
{
	UNREFERENCED_PARAMETER(Context);

	KdPrint(("enter timer \n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;

	InterlockedDecrement(&pDevExt->lTimerCount);

	LONG previousCount = InterlockedCompareExchange(&pDevExt->lTimerCount, TIMER_OUT, 0);

	if (previousCount == 0)
	{
		KdPrint(("%d seconds time out\n", TIMER_OUT));
	}

	PEPROCESS pEProcess = IoGetCurrentProcess();

	PTSTR ProcessName = (PTSTR)((ULONG)pEProcess + 0x174);

	KdPrint(("the current process is %s\n", ProcessName));
}


#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");

	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	pDevObj->Flags |= DO_DIRECT_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	// init timer
	IoInitializeTimer(pDevObj, OnTimer, NULL);

	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDK");

	pDevExt->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;

}

#pragma PAGEDCODE
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject)
{
	PDEVICE_OBJECT pNextObj;

	KdPrint(("enter driver unload\n"));

	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}



#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutin(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("Enter HelloDDKDispatchRoutin\n"));

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	static char* irpname[] =
	{
		"IRP_MJ_CREATE",
		"IRP_MJ_CREATE_NAMED_PIPE",
		"IRP_MJ_CLOSE",
		"IRP_MJ_READ",
		"IRP_MJ_WRITE",
		"IRP_MJ_QUERY_INFORMATION",
		"IRP_MJ_SET_INFORMATION",
		"IRP_MJ_QUERY_EA",
		"IRP_MJ_SET_EA",
		"IRP_MJ_FLUSH_BUFFERS",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION",
		"IRP_MJ_DIRECTORY_CONTROL",
		"IRP_MJ_FILE_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CONTROL",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL",
		"IRP_MJ_SHUTDOWN",
		"IRP_MJ_LOCK_CONTROL",
		"IRP_MJ_CLEANUP",
		"IRP_MJ_CREATE_MAILSLOT",
		"IRP_MJ_QUERY_SECURITY",
		"IRP_MJ_SET_SECURITY",
		"IRP_MJ_POWER",
		"IRP_MJ_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CHANGE",
		"IRP_MJ_QUERY_QUOTA",
		"IRP_MJ_SET_QUOTA",
		"IRP_MJ_PNP",
	};

	UCHAR type = stack->MajorFunction;
	if (type >= arraysize(irpname))
		KdPrint((" - Unknown IRP, major type %X\n", type));
	else
		KdPrint(("\t%s\n", irpname[type]));

	NTSTATUS status = STATUS_SUCCESS;

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDispatchRoutin\n"));

	return status;
}


#pragma PAGEDCODE
NTSTATUS HelloDDKDeviceIOControl(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	KdPrint(("enter hello ddk device io control\n"));
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	//ULONG cbin = stack->Parameters.DeviceIoControl.InputBufferLength;

	//ULONG cbout = stack->Parameters.DeviceIoControl.OutputBufferLength;

	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	ULONG info = 0;

	switch (code)		
	{

	case IOCTL_START_TIMER:
		KdPrint(("ioctl start timer\n"));
		pDevExt->lTimerCount = TIMER_OUT;
		IoStartTimer(pDevObj);
		break;

	case IOCTL_STOP:
		KdPrint(("ioctl stop\n"));
		IoStopTimer(pDevObj);
		break;

	default:
		status = STATUS_INVALID_VARIANT;
		break;
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = info;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("leave hello ddk device iocontrol\n"));

	return status;
}