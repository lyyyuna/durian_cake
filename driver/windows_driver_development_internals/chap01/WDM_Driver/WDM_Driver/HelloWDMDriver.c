#include <ntddk.h>

typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT fdo;
	PDEVICE_OBJECT NextStackDevice;
	PUNICODE_STRING ustrDeviceName;
	PUNICODE_STRING ustrSymlinkName;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	);

NTSTATUS HelloWDMAddDevice(
	IN PDRIVER_OBJECT pDriverObject,
	IN PDEVICE_OBJECT pDeviceObject
	);

NTSTATUS HelloWDMPnp(
	IN PDEVICE_OBJECT fdo,
	IN PIRP Irp
	);

NTSTATUS DefaultPnpHandler(
	PDEVICE_EXTENSION pdx, PIRP pIrp
	);

NTSTATUS HandleRemoveDevice(
	PDEVICE_EXTENSION pdx,
	PIRP pIrp
	);

NTSTATUS HelloWDMDispatchRoutine(
	IN PDEVICE_OBJECT fdo,
	IN PIRP pIrp
	);

NTSTATUS HelloWDMUnload(
	IN PDRIVER_OBJECT pDriverObject
	);

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, HelloWDMAddDevice)
#pragma alloc_text(PAGE, HelloWDMPnp)
#pragma alloc_text(PAGE, DefaultPnpHandler)
#pragma alloc_text(PAGE, HandleRemoveRevice)
#pragma alloc_text(PAGE, HelloWDMDispatchRoutine)
#pragma alloc_text(PAGE, HelloWDMUnload)

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
	)
{
	KdPrint(("Enter DriverEntry! \n"));

	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;
	pDriverObject->MajorFunction[IRP_MJ_PNP] = HelloWDMPnp;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloWDMDispatchRoutine;

	pDriverObject->DriverUnload = HelloWDMUnload;

	KdPrint(("Leave DriverEntry \n"));

	return STATUS_SUCCESS;

}

NTSTATUS HelloWDMUnload(
	IN PDRIVER_OBJECT pDriverObject
	)
{
	PAGED_CODE();
	KdPrint(("Enter HelloWDMUnload \n"));
	KdPrint(("Leave HelloWDMUnload \n"));
}

NTSTATUS HelloWDMDispatchRoutine(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
	)
{
	PAGED_CODE();
	KdPrint(("Enter HelloWDMDispatchRoutine \n"));

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0; // no bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloWDMDispatchRoutine \n"));

	return STATUS_SUCCESS;
}

NTSTATUS HelloWDMAddDevice(
	IN PDRIVER_OBJECT pDriverObject,
	IN PDEVICE_OBJECT pDeviceObject // physical device object
	)
{

	
	NTSTATUS status;
	UNICODE_STRING devName;
	PDEVICE_OBJECT fdo;
	UNICODE_STRING symLinkName;


	PAGED_CODE();
	KdPrint(("Enter HelloWDMAddDevice! \n"));

	RtlInitUnicodeString(&devName, L"\\Device\\MyWDMDevice");
	
	status = IoCreateDevice(
		pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&fdo);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	pDevExt->fdo = fdo;
	pDevExt->NextStackDevice = IoAttachDeviceToDeviceStack(fdo, pDeviceObject);

	RtlInitUnicodeString(&symLinkName, L"\\DosDevices\\HelloWDM");
	pDevExt->ustrDeviceName = &devName;
	pDevExt->ustrSymlinkName = &symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteSymbolicLink(&pDevExt->ustrSymlinkName);
		status = IoCreateSymbolicLink(&symLinkName, &devName);
		if (!NT_SUCCESS(status))
		{
			return status;
		}
	}

	fdo->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
	fdo->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Leave HelloWDMAddDevice! \n"));
	return STATUS_SUCCESS;
}

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))
NTSTATUS HelloWDMPnp(
	IN PDEVICE_OBJECT fdo,
	IN PIRP pIrp
	)
{
	NTSTATUS status = STATUS_SUCCESS;


	PAGED_CODE();
	KdPrint(("Enter HelloWDMPnp. \n"));
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	static NTSTATUS(*fcntab[])(PDEVICE_EXTENSION pdx, PIRP Irp) =
	{
		DefaultPnpHandler,		// IRP_MN_START_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_REMOVE_DEVICE
		HandleRemoveDevice,		// IRP_MN_REMOVE_DEVICE
		DefaultPnpHandler,		// IRP_MN_CANCEL_REMOVE_DEVICE
		DefaultPnpHandler,		// IRP_MN_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_CANCEL_STOP_DEVICE
		DefaultPnpHandler,		// IRP_MN_QUERY_DEVICE_RELATIONS
		DefaultPnpHandler,		// IRP_MN_QUERY_INTERFACE
		DefaultPnpHandler,		// IRP_MN_QUERY_CAPABILITIES
		DefaultPnpHandler,		// IRP_MN_QUERY_RESOURCES
		DefaultPnpHandler,		// IRP_MN_QUERY_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,		// IRP_MN_QUERY_DEVICE_TEXT
		DefaultPnpHandler,		// IRP_MN_FILTER_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,		// 
		DefaultPnpHandler,		// IRP_MN_READ_CONFIG
		DefaultPnpHandler,		// IRP_MN_WRITE_CONFIG
		DefaultPnpHandler,		// IRP_MN_EJECT
		DefaultPnpHandler,		// IRP_MN_SET_LOCK
		DefaultPnpHandler,		// IRP_MN_QUERY_ID
		DefaultPnpHandler,		// IRP_MN_QUERY_PNP_DEVICE_STATE
		DefaultPnpHandler,		// IRP_MN_QUERY_BUS_INFORMATION
		DefaultPnpHandler,		// IRP_MN_DEVICE_USAGE_NOTIFICATION
		DefaultPnpHandler,		// IRP_MN_SURPRISE_REMOVAL
	};

	ULONG fcn = stack->MinorFunction;
	if (fcn >= arraysize(fcntab))
	{
		status = DefaultPnpHandler(pdx, pIrp);
		return status;
	}

	static char* fcnname[] =
	{
		"IRP_MN_START_DEVICE",
		"IRP_MN_QUERY_REMOVE_DEVICE",
		"IRP_MN_REMOVE_DEVICE",
		"IRP_MN_CANCEL_REMOVE_DEVICE",
		"IRP_MN_STOP_DEVICE",
		"IRP_MN_QUERY_STOP_DEVICE",
		"IRP_MN_CANCEL_STOP_DEVICE",
		"IRP_MN_QUERY_DEVICE_RELATIONS",
		"IRP_MN_QUERY_INTERFACE",
		"IRP_MN_QUERY_CAPABILITIES",
		"IRP_MN_QUERY_RESOURCES",
		"IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
		"IRP_MN_QUERY_DEVICE_TEXT",
		"IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
		"",
		"IRP_MN_READ_CONFIG",
		"IRP_MN_WRITE_CONFIG",
		"IRP_MN_EJECT",
		"IRP_MN_SET_LOCK",
		"IRP_MN_QUERY_ID",
		"IRP_MN_QUERY_PNP_DEVICE_STATE",
		"IRP_MN_QUERY_BUS_INFORMATION",
		"IRP_MN_DEVICE_USAGE_NOTIFICATION",
		"IRP_MN_SURPRISE_REMOVAL",
	};

	KdPrint(("PNP Request (%s)\n", fcnname[fcn]));
	status = (*fcntab[fcn])(pdx, pIrp);
	KdPrint(("Leave HelloWDMPnp \n"));
	return status;
}


NTSTATUS DefaultPnpHandler(
	PDEVICE_EXTENSION pdx,
	PIRP pIrp
	)
{
	PAGED_CODE();
	KdPrint(("Enter DefaultPnpHandler \n"));

	IoSkipCurrentIrpStackLocation(pIrp);

	KdPrint(("Leave DefaultPnpHandler \n"));
	return IoCallDriver(pdx->NextStackDevice, pIrp);
}


NTSTATUS HandleRemoveDevice(
	PDEVICE_EXTENSION pdx,
	PIRP pIrp
	)
{
	NTSTATUS status = DefaultPnpHandler(pdx, pIrp);

	PAGED_CODE();
	KdPrint(("Enter HandleRemoveDevice \n"));

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoDeleteSymbolicLink(pdx->ustrSymlinkName);

	if (pdx->NextStackDevice)
	{
		IoDetachDevice(pdx->NextStackDevice);
	}

	IoDeleteDevice(pdx->fdo);
	KdPrint(("Leave HandleRemoveDevice \n"));
	return status;
}
