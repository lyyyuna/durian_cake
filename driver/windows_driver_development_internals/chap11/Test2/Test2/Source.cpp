#include "Header.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	KdPrint(("dirver b: enter b driver entry\n"));

	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;

	NTSTATUS status = CreateDevice(pDriverObject);

	KdPrint(("driver B: leave B driver entry\n"));

	return status;
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
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevicB");

	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDKB");
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
	KdPrint(("driver B: enter B driver unload\n"));
	
	PDEVICE_OBJECT pNextObj = pDriverObject->DeviceObject;

	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}

	KdPrint(("driver B: leave B driver unload\n"));
}

VOID CompleteDriverA_read(PVOID context, PIO_STATUS_BLOCK pStatus_block, ULONG i)
{
	UNREFERENCED_PARAMETER(i);
	UNREFERENCED_PARAMETER(pStatus_block);
	KdPrint(("driver b: the driver a read completed now!~!!!\n"));
	KeSetEvent((PKEVENT)context, IO_NO_INCREMENT, FALSE);
}

#pragma PAGEDCODE
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("driver B: enter B hello read\n"));

	UNICODE_STRING DeviceName;
	RtlInitUnicodeString(&DeviceName, L"\\Device\\MyDDKDeviceA");

	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes,
		&DeviceName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	HANDLE hDevice;
	IO_STATUS_BLOCK status_block;

	NTSTATUS status = ZwCreateFile(&hDevice,
		FILE_READ_ATTRIBUTES,//
		&objectAttributes,
		&status_block,
		NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ,
		FILE_OPEN_IF, 0, NULL, 0);

	KEVENT event;
	KeInitializeEvent(&event, SynchronizationEvent, FALSE);

	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);
	if (NT_SUCCESS(status))
	{
		status = ZwReadFile(hDevice, NULL, (PIO_APC_ROUTINE)CompleteDriverA_read, &event, &status_block, NULL, 0, &offset, NULL);

	}

	if (status == STATUS_PENDING)
	{
		KdPrint(("Driver B:ZwReadFile return STATUS_PENDING!\n"));
		KdPrint(("Driver B:Waiting..."));
		KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
	}
	ZwClose(hDevice);

	status = STATUS_SUCCESS;
	// 
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("DriverB:Leave B HelloDDKRead\n"));
	return status;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("DriverB:Enter B HelloDDKDispatchRoutine\n"));
	NTSTATUS ntStatus = STATUS_SUCCESS;
	// 
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("DriverB:Leave B HelloDDKDispatchRoutine\n"));
	return ntStatus;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKCreate(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("DriverB:Enter B HelloDDKCreate\n"));
	NTSTATUS ntStatus = STATUS_SUCCESS;
	// 
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("DriverB:Leave B HelloDDKCreate\n"));

	return ntStatus;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKClose(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("DriverB:Enter B HelloDDKClose\n"));
	NTSTATUS ntStatus = STATUS_SUCCESS;

	//PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	// 
	pIrp->IoStatus.Status = ntStatus;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("DriverB:Leave B HelloDDKClose\n"));

	return ntStatus;
}