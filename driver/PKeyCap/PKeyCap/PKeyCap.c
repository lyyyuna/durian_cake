#include <wdm.h>

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"


typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;
	PDEVICE_OBJECT pFilterDeviceObject;
	PDEVICE_OBJECT pTargetDeviceObject;
	PDEVICE_OBJECT pLowerDeviceObject;

	KSPIN_LOCK IoRequestsSpinLock;
	KEVENT IoInProgressEvent;
} C2P_DEV_EXT, *PC2P_DEV_EXT;


extern POBJECT_TYPE * IoDriverObjectType;

NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectTypePointer, KPROCESSOR_MODE AccessMode,
	PVOID ParseContext, PVOID * Object);

ULONG gc2pKeyCount = 0;

NTSTATUS c2pDevExtInit(IN PC2P_DEV_EXT devExt, IN PDEVICE_OBJECT pFilterDeviceObject, 
	IN PDEVICE_OBJECT pTargetDeviceObject, IN PDEVICE_OBJECT pLowerDeviceObject)
{
	memset(devExt, 0, sizeof(C2P_DEV_EXT));
	devExt->NodeSize = sizeof(C2P_DEV_EXT);
	devExt->pFilterDeviceObject = pFilterDeviceObject;
	
	KeInitializeSpinLock(&(devExt->IoRequestsSpinLock));
	KeInitializeEvent(&(devExt->IoInProgressEvent), NotificationEvent, FALSE); 

	devExt->pTargetDeviceObject = pTargetDeviceObject;
	devExt->pLowerDeviceObject = pLowerDeviceObject;

	return STATUS_SUCCESS;
}

NTSTATUS c2pAttachDevice(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	UNICODE_STRING uni_DriverName;
	PDRIVER_OBJECT kbdDriverObject = NULL;

	PDEVICE_OBJECT pTargetDeviceObject;
	PDEVICE_OBJECT pLowerDeviceObject;
	PDEVICE_OBJECT pFilterDeviceObject;

	// dev extension
	PC2P_DEV_EXT devExt;

	(VOID)RegistryPath;

	DbgPrint("myattach!\r\n");
	RtlInitUnicodeString(&uni_DriverName, KBD_DRIVER_NAME);

	status = ObReferenceObjectByName(&uni_DriverName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &kbdDriverObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("lyy's attach: cannot get the kbd driver object. \r\n");
		return status;
	}
	else
	{
		// de-refer
		ObDereferenceObject(kbdDriverObject);
	}

	// get the device object
	pTargetDeviceObject = kbdDriverObject->DeviceObject;
	// all devices of the driver form a chain
	while (pTargetDeviceObject)
	{
		// create filter device
		status = IoCreateDevice(IN DriverObject, IN sizeof(C2P_DEV_EXT), IN NULL, IN pTargetDeviceObject->DeviceType, IN pTargetDeviceObject->Characteristics, IN FALSE, OUT & pFilterDeviceObject);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("lyy's attach: cannot create filter device object! \r\n");
			return status;
		}
		// attach filter to lower device
		pLowerDeviceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
		if (!pLowerDeviceObject)
		{
			DbgPrint("lyy's attach: filter cannot attach to the device object. \r\n");
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}

		// device extension, store information
		devExt = (PC2P_DEV_EXT)(pFilterDeviceObject->DeviceExtension);
		c2pDevExtInit(devExt, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);

		pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize;
		pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags &(DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);
		
		// next device in the chain
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}

	return status;
}

NTSTATUS c2pDispatchGeneral(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint("other general dispatch. \r\n");
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PC2P_DEV_EXT)DeviceObject->DeviceExtension)->pLowerDeviceObject, Irp);
}

NTSTATUS c2pReadComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	PIO_STACK_LOCATION irpStack;
	PUCHAR buf = NULL;
	ULONG buf_len = 0;
	ULONG i;

	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);

	irpStack = IoGetCurrentIrpStackLocation(Irp);

	// if irp is success
	// then retrieve data from the buffer.
	if (NT_SUCCESS(Irp->IoStatus.Status))
	{
		buf = Irp->AssociatedIrp.SystemBuffer;
		buf_len = Irp->IoStatus.Information;

		// print
		for (i = 0; i < buf_len; i++)
		{
			DbgPrint("capture key: %2x\r\n", buf[i]);
		}
	}

	gc2pKeyCount--;

	// If a driver sets an IoCompletion routine for an IRP and then 
	// passes the IRP down to a lower driver, the IoCompletion routine 
	// should check the IRP->PendingReturned flag. If the flag is set, 
	// the IoCompletion routine must call IoMarkIrpPending with the IRP.
	if (Irp->PendingReturned)
	{
		IoMarkIrpPending(Irp);
	}
	return Irp->IoStatus.Status;
}

NTSTATUS c2pDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;

	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION currentIrpStack;
	// KEVENT waitEvent;

	// KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);
	if (Irp->CurrentLocation == 1)
	{
		ULONG ReturnedInformation = 0;
		DbgPrint("Dispatch encountered ");
		status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = ReturnedInformation;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	gc2pKeyCount++;
	devExt = (PC2P_DEV_EXT)DeviceObject->DeviceExtension;

	// set callback
	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	IoCopyCurrentIrpStackLocationToNext(Irp);
	IoSetCompletionRoutine(Irp, c2pReadComplete, DeviceObject, TRUE, TRUE, TRUE);
	return IoCallDriver(devExt->pLowerDeviceObject, Irp);
}

NTSTATUS c2pPower(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PC2P_DEV_EXT devExt;

	devExt = (PC2P_DEV_EXT)DeviceObject->DeviceExtension;

	PoStartNextPowerIrp(Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return PoCallDriver(devExt->pLowerDeviceObject, Irp);
}

NTSTATUS c2pPnP(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;

	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION irpStack;


	devExt = (PC2P_DEV_EXT)(DeviceObject->DeviceExtension);
	irpStack = IoGetCurrentIrpStackLocation(Irp);

	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("irp minor: remove device.\r\n");
		IoSkipCurrentIrpStackLocation(Irp);
		IoCallDriver(devExt->pLowerDeviceObject, Irp);
		IoDetachDevice(devExt->pLowerDeviceObject);
		// delete
		IoDeleteDevice(DeviceObject);
		status = STATUS_SUCCESS;
		break;
	default:
		// others, just skip
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(devExt->pLowerDeviceObject, Irp);
		break;
	}

	return status;
}


VOID c2pDetach(IN PDEVICE_OBJECT pDeviceObject)
{
	// UNREFERENCED_PARAMETER(pDeviceObject);

	PC2P_DEV_EXT devExt;
	// BOOLEAN
	
	devExt = (PC2P_DEV_EXT)pDeviceObject->DeviceExtension;
	__try
	{
		__try
		{
			IoDetachDevice(devExt->pTargetDeviceObject);
			devExt->pTargetDeviceObject = NULL;
			IoDeleteDevice(pDeviceObject);
			devExt->pFilterDeviceObject = NULL;
			DbgPrint("detach finished ... \r\n");
		}
		__except (EXCEPTION_EXECUTE_HANDLER){}
	}
	__finally{}
	return;
	
}

#define  DELAY_ONE_MICROSECOND  (-10)
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

VOID c2pUnload(IN PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT DeviceObject;


	LARGE_INTEGER iDelay;
	PRKTHREAD CurrentThread;

	// UNREFERENCED_PARAMETER(DriverObject);

	iDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MICROSECOND);
	CurrentThread = KeGetCurrentThread();
	// change the thread to low realtime mode
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);
	DbgPrint("driver is unloading ... \r\n");

	DeviceObject = DriverObject->DeviceObject;
	while (DeviceObject)
	{
		c2pDetach(DeviceObject);
		DeviceObject = DeviceObject->NextDevice;
	}
	ASSERT(NULL == DriverObject->DeviceObject);

	while (gc2pKeyCount)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &iDelay);
	}
	DbgPrint("driver unloaded ... ");

	return;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	ULONG i;
	NTSTATUS status;

	UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint("c2p capture entering driver entry.\r\n");

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = c2pDispatchGeneral;
	}

	DriverObject->MajorFunction[IRP_MJ_READ] = c2pDispatchRead;
	DriverObject->MajorFunction[IRP_MJ_POWER] = c2pPower;
	DriverObject->MajorFunction[IRP_MJ_PNP] = c2pPnP;

	DriverObject->DriverUnload = c2pUnload;

	status = c2pAttachDevice(DriverObject, RegistryPath);

	return status;
}