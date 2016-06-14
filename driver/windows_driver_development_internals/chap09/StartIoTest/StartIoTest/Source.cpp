#include "Header.h"

#pragma LOCKEDCODE
VOID HelloDDKStartIo(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
	)
{
	KdPrint(("enter helloddk start io\n"));

	KIRQL oldirql;
	IoAcquireCancelSpinLock(&oldirql);
	if (pIrp != pDeviceObject->CurrentIrp || pIrp->Cancel)
	{
		IoReleaseCancelSpinLock(oldirql);
		KdPrint(("leave helloddk start io\n"));
		return;
	}
	else
	{
		IoSetCancelRoutine(pIrp, NULL);
		IoReleaseCancelSpinLock(oldirql);
	}

	KEVENT event;
	KeInitializeEvent(&event, NotificationEvent, FALSE);

	LARGE_INTEGER timeout;
	timeout.QuadPart = -3 * 1000 * 1000 * 10;

	KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, &timeout);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	IoStartNextPacket(pDeviceObject, TRUE);

	KdPrint(("leave helloddk start io\n"));
}


#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status;
	KdPrint(("enter driver entry\n"));

	pDriverObject->DriverUnload = HelloDDKUnload;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = HelloDDKDispatchRoutin;
	pDriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HelloDDKDispatchRoutin;


	pDriverObject->DriverStartIo = HelloDDKStartIo;

	status = CreateDevice(pDriverObject);
	KdPrint(("leave driverentry\n"));

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
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");

	status = IoCreateDevice(
		pDriverObject,
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

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDK");
	pDevExt->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);

	if (!NT_SUCCESS(status))
	{
		IoDetachDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}


#pragma PAGEDCODE
VOID HelloDDKUnload(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	PDEVICE_OBJECT pNextObj;
	KdPrint(("enter driverunload\n"));
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
NTSTATUS HelloDDKDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
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


VOID OnCancelIRP(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
	)
{
	KdPrint(("enter cancel read irp\n"));

	if (pIrp == pDeviceObject->CurrentIrp)
	{
		KIRQL oldirql = pIrp->CancelIrql;

		IoReleaseCancelSpinLock(pIrp->CancelIrql);

		IoStartNextPacket(pDeviceObject, TRUE);

		KeLowerIrql(oldirql);
	}
	else
	{
		KeRemoveEntryDeviceQueue(&pDeviceObject->DeviceQueue, &pIrp->Tail.Overlay.DeviceQueueEntry);

		IoReleaseCancelSpinLock(pIrp->CancelIrql);
	}

	pIrp->IoStatus.Status = STATUS_CANCELLED;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("leave cancel read irp\n"));
}

NTSTATUS HelloDDKRead(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	KdPrint(("enter hello ddk read\n"));

	UNREFERENCED_PARAMETER(pDevObj);
	//PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	IoMarkIrpPending(pIrp);

	IoStartPacket(pDevObj, pIrp, 0, OnCancelIRP);

	KdPrint(("leave hello ddk read\n"));

	return STATUS_SUCCESS;
}