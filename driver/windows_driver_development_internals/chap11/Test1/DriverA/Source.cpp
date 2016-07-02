#include "Header.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	KdPrint(("Driver A: enter A driver entry\n"));

	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;

	NTSTATUS status = CreateDevice(pDriverObject);

	KdPrint(("driver A: leave A driver entry\n"));
	return status;
}

#pragma LOCKEDCODE
VOID OnTimerDpc(
	IN PKDPC pDpc,
	IN PVOID pContext,
	IN PVOID SysArg1,
	IN PVOID SysArg2
	)
{
	UNREFERENCED_PARAMETER(pDpc);
	UNREFERENCED_PARAMETER(SysArg1);
	UNREFERENCED_PARAMETER(SysArg2);


	PDEVICE_OBJECT pDevObj = (PDEVICE_OBJECT)pContext;
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	PIRP currentPendingIRP = pDevExt->currentPendingIRP;

	KdPrint(("driver A: complete the driver A irp_mj_read irp\n"));

	currentPendingIRP->IoStatus.Status = STATUS_SUCCESS;
	currentPendingIRP->IoStatus.Information = 0;

	IoCompleteRequest(currentPendingIRP, IO_NO_INCREMENT);
}

#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDeviceA");

	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

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

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	KeInitializeTimer(&pDevExt->pollingTimer);
	
	KeInitializeDpc(&pDevExt->pollingDPC,
		OnTimerDpc,
		(PVOID)pDevObj
		);

	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDKA");
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
	KdPrint(("driver a: enter a driver unload\n"));

	PDEVICE_OBJECT pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}

	KdPrint(("driver a: leave a driver unload\n"));
}


#pragma PAGEDCODE
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	KdPrint(("driver a: enter A helloddkread\n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	IoMarkIrpPending(pIrp);

	pDevExt->currentPendingIRP = pIrp;

	ULONG ulMicroSecond = 3000000;

	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMicroSecond);
	
	KeSetTimer(
		&pDevExt->pollingTimer,
		timeout,
		&pDevExt->pollingDPC);

	KdPrint(("driver A: Leave A helloddkread\n"));

	return STATUS_PENDING;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("driver A: enter A helloddk dispatch routine\n"));

	NTSTATUS status = STATUS_SUCCESS;

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("driver A: leave A hello ddk dispatch routine\n"));

	return status;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKCreate(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("DriverA:Enter A HelloDDKCreate\n"));
	NTSTATUS status = STATUS_SUCCESS;
	//
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("DriverA:Leave A HelloDDKCreate\n"));
	return status;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKClose(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("DriverA:Enter A HelloDDKClose\n"));
	NTSTATUS status = STATUS_SUCCESS;
	// 
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("DriverA:Leave A HelloDDKClose\n"));
	return status;
}