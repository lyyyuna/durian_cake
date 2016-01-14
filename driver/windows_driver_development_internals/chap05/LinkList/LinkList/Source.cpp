#include "Header.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status;

	KdPrint(("Enter DriverEntry \n"));

	// register callback
	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatchRoutine;

	// create device object
	status = CreateDevice(pDriverObject);

	LinkListTest();

	KdPrint(("End DriverEntry \n"));
	return status;
}

#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{

	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");
	
	PDEVICE_OBJECT pDevObj;
	NTSTATUS status;
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&pDevObj
		);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	PDEVICE_EXTENSION pDevExt;
	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	// create symlink
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HEEEEEE");
	pDevExt->ustrSymlinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}

#pragma PAGEDCODE
VOID HelloDDKUnload(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	KdPrint(("Enter DriverUnload. \n"));

	PDEVICE_OBJECT pNextObj;
	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;
		// delete symlinkname
		UNICODE_STRING symlinkName = pDevExt->ustrSymlinkName;
		IoDeleteSymbolicLink(&symlinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}

	KdPrint(("Leave DriverUnload. \n"));
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutine(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("Enter HelloDDKdispatchRoutine  \n"));

	NTSTATUS status = STATUS_SUCCESS;
	// finish irp
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKDispatchRoutine \n"));

	return status;
}




#pragma INITCODE
VOID LinkListTest()
{
	LIST_ENTRY linkListHead;
	InitializeListHead(&linkListHead);

	KdPrint(("Begin insert to link list. \n"));

	PMYDATASTRUCT pData;
	for (ULONG i = 0; i < 10; i++)
	{
		pData = (PMYDATASTRUCT)ExAllocatePool(PagedPool, sizeof(MYDATASTRUCT));
		pData->number = i;
		InsertHeadList(&linkListHead, &pData->ListEntry);
	}

	// retrieve
	KdPrint(("Begin to remove from the link list \n"));
	while (!IsListEmpty(&linkListHead))
	{
		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry, MYDATASTRUCT, ListEntry);
		KdPrint(("%d \n", pData->number));
		ExFreePool(pData);
	}

}