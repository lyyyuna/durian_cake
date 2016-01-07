#include <ntddk.h>


typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;  // device name
	UNICODE_STRING ustrSymLinkName;	// symlink name
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

VOID HelloDDKUnload(
	IN PDRIVER_OBJECT pDriverObject
	);

NTSTATUS HelloDDKDispatchRoutine(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp);

NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	);

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	);


#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, HelloDDKUnload)
#pragma alloc_text(PAGE, HelloDDKDispatchRoutine)
#pragma alloc_text(INIT, CreateDevice)


VOID HelloDDKUnload(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	PDEVICE_OBJECT pNextObj;

	KdPrint(("enter DriverUnload!\n"));
	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		// delete symlink
		
		//UNICODE_STRING pSymLinkName = pDevExt->ustrSymLinkName;
		//IoDeleteSymbolicLink(&pSymLinkName);
		// delete self
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}

NTSTATUS HelloDDKDispatchRoutine(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	UNREFERENCED_PARAMETER(pDevObj);

	KdPrint(("Enter dispatch routine. \n"));
	// complete irp
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0; // bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave dispatch routine. \n"));

	return status;

}

NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	NTSTATUS status;
	UNICODE_STRING devName;

	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;
	// 
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");
	// create device
	status = IoCreateDevice(
		pDriverObject,
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

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	// create symlink
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\MyDDKHello");
	pDevExt->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;


}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(pRegistryPath);

	KdPrint(("enter driverentry \n"));

	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatchRoutine;

	status = CreateDevice(pDriverObject);

	KdPrint(("leave driverentry \n"));
	return status;
}


