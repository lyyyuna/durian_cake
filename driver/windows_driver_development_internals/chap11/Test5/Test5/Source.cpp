#include "Header.h"

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	NTSTATUS ntStatus;
	KdPrint(("DriverB:Enter B DriverEntry\n"));

	//
	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;

	//
	ntStatus = CreateDevice(pDriverObject);

	KdPrint(("DriverB:Leave B DriverEntry\n"));
	return ntStatus;
}


#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT	pDriverObject)
{
	NTSTATUS ntStatus;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	//
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevicB");

	//
	ntStatus = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	//
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDKB");
	pDevExt->ustrSymLinkName = symLinkName;
	NTSTATUS status = IoCreateSymbolicLink(&symLinkName, &devName);
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
	PDEVICE_OBJECT	pNextObj;
	KdPrint(("DriverB:Enter B DriverUnload\n"));
	pNextObj = pDriverObject->DeviceObject;

	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		//
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
	KdPrint(("DriverB:Enter B DriverUnload\n"));
}


#pragma PAGEDCODE
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Driver B: enter B hello k read\n"));

	UNICODE_STRING DeviceName;
	RtlInitUnicodeString(&DeviceName, L"\\Device\\MyDDKDeviceA");

	PDEVICE_OBJECT DeviceObject = NULL;
	PFILE_OBJECT FileObject = NULL;

	NTSTATUS status = IoGetDeviceObjectPointer(&DeviceName, FILE_ALL_ACCESS, &FileObject, &DeviceObject);

	KdPrint(("DriverB:FileObject:%x\n", FileObject));
	KdPrint(("DriverB:DeviceObject:%x\n", DeviceObject));

	if (!NT_SUCCESS(status))
	{
		KdPrint(("DriverB:IoGetDeviceObjectPointer() 0x%x\n", status));

		status = STATUS_UNSUCCESSFUL;
		// 
		pIrp->IoStatus.Status = status;
		pIrp->IoStatus.Information = 0;	// bytes xfered
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		KdPrint(("DriverB:Leave B HelloDDKRead\n"));

		return status;
	}


	KEVENT event;
	KeInitializeEvent(&event, NotificationEvent, FALSE);
	IO_STATUS_BLOCK status_block;
	LARGE_INTEGER offset = RtlConvertLongToLargeInteger(0);

	PIRP pNewIrp = IoBuildSynchronousFsdRequest(IRP_MJ_READ,
		DeviceObject,
		NULL, 0,
		&offset, &event, &status_block);
	KdPrint(("DriverB:pNewIrp:%x\n", pNewIrp));

	PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(pNewIrp);
	stack->FileObject = FileObject;

	status = IoCallDriver(DeviceObject, pNewIrp);
	if (status == STATUS_PENDING)
	{
		status = KeWaitForSingleObject(
			&event,
			Executive,
			KernelMode,
			FALSE,
			NULL
			);
		status = status_block.Status;
	}

	ObDereferenceObject(FileObject);

	status = STATUS_SUCCESS;

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("driver B: leave B hello ddk read\n"));

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
