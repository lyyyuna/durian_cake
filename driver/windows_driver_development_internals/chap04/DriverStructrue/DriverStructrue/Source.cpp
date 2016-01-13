#include "Header.h"


#pragma INITCODE
VOID Dump(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	KdPrint(("----------------------------------------------\n"));
	KdPrint(("Begin Dump...\n"));
	KdPrint(("Driver Address:0X%08X\n", pDriverObject));
	KdPrint(("Driver name:%wZ\n", &pDriverObject->DriverName));
	KdPrint(("Driver HardwareDatabase:%wZ\n", pDriverObject->HardwareDatabase));
	KdPrint(("Driver first device:0X%08X\n", pDriverObject->DeviceObject));

	PDEVICE_OBJECT pDevice = pDriverObject->DeviceObject;
	int i = 1;
	for (; pDevice != NULL; pDevice = pDevice->NextDevice)
	{
		KdPrint(("The %d device\n", i++));
		KdPrint(("Device AttachedDevice:0X%08X\n", pDevice->AttachedDevice));
		KdPrint(("Device NextDevice:0X%08X\n", pDevice->NextDevice));
		KdPrint(("Device StackSize:%d\n", pDevice->StackSize));
		KdPrint(("Device's DriverObject:0X%08X\n", pDevice->DriverObject));
	}

	KdPrint(("Dump over!\n"));
	KdPrint(("----------------------------------------------\n"));
}

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status;
	KdPrint(("Enter DriverEntry\n"));

	//__asm int 3

	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatchRoutine;

	status = CreateDevice(pDriverObject);
	status = CreateDevice2(pDriverObject);
	
	Dump(pDriverObject);

	KdPrint(("Leave DriverEntry\n"));
	return status;
}

#pragma PAGEDCODE
VOID HelloDDKUnload(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	PDEVICE_OBJECT pNextObj;
	KdPrint(("Enter DriverUnload\n"));

	//__asm int 3

	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		//delete symlink
		UNICODE_STRING symlink = pDevExt->ustrSymLinkName;
		 IoDeleteSymbolicLink(&symlink);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutine(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
	)
{
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("Enter HelloDDKDispatchRoutine! \n"));

	NTSTATUS status = STATUS_SUCCESS;

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDispatchRoutine! \n"));

	return status;
}

#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice11111");
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&pDevObj);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	//__asm int 3

	PDEVICE_EXTENSION pDevExt;
	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	// create symlink
	UNICODE_STRING symlinkName;
	RtlInitUnicodeString(&symlinkName, L"\\??\\HelloDDK11111");
	pDevExt->ustrSymLinkName = symlinkName;
	status = IoCreateSymbolicLink(&symlinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}


#pragma PAGEDCODE
NTSTATUS CreateDevice2(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice22222");
	
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&pDevObj);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	//__asm int 3

	PDEVICE_EXTENSION pDevExt;
	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	// create symlink
	UNICODE_STRING symlinkName;
	RtlInitUnicodeString(&symlinkName, L"\\??\\HelloDDK22222");
	pDevExt->ustrSymLinkName = symlinkName;
	status = IoCreateSymbolicLink(&symlinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	return STATUS_SUCCESS;

}

