#include "Driver.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status;

	KdPrint(("Enter DriverEntry\n"));

	pDriverObject->DriverUnload = HelloDDKUnload;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = HelloDDKCleanUp;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HelloDDKDispatchRoutin;

	status = CreateDevice(pDriverObject);

	KdPrint(("leave DriverEntry\n"));


	return status;

}


#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	NTSTATUS status;
	
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");

	PDEVICE_OBJECT pDevObj;
	status = IoCreateDevice(
		pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj
		);
	if (!NT_SUCCESS(status))
		return status;

	PDEVICE_EXTENSION pDevExt;

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	pDevExt->pIRPLinkListHead = (PLIST_ENTRY)ExAllocatePool(PagedPool, sizeof(LIST_ENTRY));
	InitializeListHead(pDevExt->pIRPLinkListHead);

	//  创建符号链接
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
	
	KdPrint(("Enter DriverUnload\n"));
	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		// delete the symbol link
		UNICODE_STRING plLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&plLinkName);

		ExFreePool(pDevExt->pIRPLinkListHead);

		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);

	}

}


#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("Enter HelloDDKDispatchRoutin\n"));

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	//建立一个字符串数组与IRP类型对应起来
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


	//对一般IRP的简单操作，
	NTSTATUS status = STATUS_SUCCESS;
	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDispatchRoutin\n"));

	return status;
}


NTSTATUS HelloDDKRead(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	KdPrint(("enter helloddk read\n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	PMY_IRP_ENTRY pIrp_entry = (PMY_IRP_ENTRY)ExAllocatePool(PagedPool, sizeof(MY_IRP_ENTRY));

	pIrp_entry->pIRP = pIrp;

	// insert into the list
	InsertHeadList(pDevExt->pIRPLinkListHead, &pIrp_entry->ListEntry);

	// mark as pending
	IoMarkIrpPending(pIrp);

	KdPrint(("leave helloddk read.\n"));

	return STATUS_PENDING;
}

NTSTATUS HelloDDKCleanUp(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	KdPrint(("enter helloddk cleanup\n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//(1)将存在队列中的IRP逐个出队列，并处理
	PMY_IRP_ENTRY my_irp_entry;
	while (!IsListEmpty(pDevExt->pIRPLinkListHead))
	{
		PLIST_ENTRY pEntry = RemoveHeadList(pDevExt->pIRPLinkListHead);

		my_irp_entry = CONTAINING_RECORD(pEntry, MY_IRP_ENTRY, ListEntry);
		my_irp_entry->pIRP->IoStatus.Status = STATUS_SUCCESS;
		my_irp_entry->pIRP->IoStatus.Information = 0; // bytes xfered
		IoCompleteRequest(my_irp_entry->pIRP, IO_NO_INCREMENT);

		ExFreePool(my_irp_entry);
	}

	//(2)处理IRP_MJ_CLEANUP的IRP
	NTSTATUS status = STATUS_SUCCESS;
	// complete irp
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;  // bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("leave hello ddk cleanup\n"));
	
	return STATUS_SUCCESS;
}

