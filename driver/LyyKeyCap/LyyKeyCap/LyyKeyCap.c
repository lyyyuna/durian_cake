#include <wdm.h>

#define KBDCLASS_DRIVER_NAME	L"\\Driver\\Kbdclass"

typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;
	// 过滤设备对象
	PDEVICE_OBJECT pFilterDeviceObject;
	// 同时调用时的保护锁
	KSPIN_LOCK IoRequestsSpinLock;
	// 进程间同步事件
	KEVENT IoInProgressEvent;
	// 绑定的设备对象
	PDEVICE_OBJECT TargetDeviceObject;
	// 绑定前底层设备对象
	PDEVICE_OBJECT LowerDeviceObject;
} C2P_DEV_EXT, *PC2P_DEV_EXT;

NTSTATUS 
c2pDevExtInit(
IN PC2P_DEV_EXT devExt,
IN PDEVICE_OBJECT pFilterDeviceObject,
IN PDEVICE_OBJECT pTargetDeviceObject,
IN PDEVICE_OBJECT pLowerDeviceObject
)
{
	memset(devExt, 0, sizeof(C2P_DEV_EXT));
	devExt->NodeSize = sizeof(C2P_DEV_EXT);
	devExt->pFilterDeviceObject = pFilterDeviceObject;
	KeInitializeSpinLock(&(devExt->IoRequestsSpinLock));
	KeInitializeEvent(&(devExt->IoInProgressEvent), NotificationEvent, FALSE);
	devExt->TargetDeviceObject = pTargetDeviceObject;
	devExt->LowerDeviceObject = pLowerDeviceObject;
	return STATUS_SUCCESS;
}

// 声明
NTSTATUS
ObReferenceObjectByName(
PUNICODE_STRING ObjectName,
ULONG Attributes,
PACCESS_STATE AccessState,
ACCESS_MASK DesiredAccess,
POBJECT_TYPE ObjectType,
KPROCESSOR_MODE AccessMode,
PVOID ParseContext,
PVOID *Object
);

extern POBJECT_TYPE *IoDriverObjectType;
ULONG gC2pKeyCount = 0;
PDRIVER_OBJECT gDriverObject = NULL;

NTSTATUS
c2pAttachDevice(
IN PDRIVER_OBJECT DriverObject,
IN PUNICODE_STRING RegistryPath
)
{
	NTSTATUS status = 0;
	UNICODE_STRING uniNtNameString;
	PC2P_DEV_EXT devExt;
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDEVICE_OBJECT pLowerDeivceObject = NULL;

	PDRIVER_OBJECT KbdDriverObject = NULL;

	KdPrint(("lyy's Attach\n"));

	// 初始化字符串
	RtlInitUnicodeString(&uniNtNameString, KBDCLASS_DRIVER_NAME);

	status = ObReferenceObjectByName(
		&uniNtNameString, 
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		&KbdDriverObject
		);
	// 如果失败就返回
	if (!NT_SUCCESS(status))
	{
		KdPrint(("lyy's attach: cannot get the device object. \n"));
		return status;
	}
	else
	{
		// 这个打开需要解引用
		ObDereferenceObject(KbdDriverObject);
	}


	pTargetDeviceObject = KbdDriverObject->DeviceObject;
	while (pTargetDeviceObject)
	{
		status = IoCreateDevice(
			IN DriverObject,
			IN sizeof(C2P_DEV_EXT),
			IN NULL,
			IN pTargetDeviceObject->DeviceType,
			IN pTargetDeviceObject->Characteristics,
			IN FALSE,
			OUT &pFilterDeviceObject
			);
		// 如果失败=直接退出
		if (!NT_SUCCESS(status))
		{
			KdPrint(("lyy's attach: cannot create the filter device object. \n"));
			return status;
		}

		// 绑定后的真实设备
		// The IoAttachDeviceToDeviceStack routine attaches the caller's 
		// device object to the highest device object in the chain and returns
		// a pointer to the previously highest device object
		pLowerDeivceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
		if (!pLowerDeivceObject)
		{
			KdPrint(("lyy's attach: cannot attach to the device object. \n"));
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}

		// 设备扩展，放置一些有用信息
		devExt = (PC2P_DEV_EXT)(pFilterDeviceObject->DeviceExtension);
		c2pDevExtInit(
			devExt,
			pFilterDeviceObject,
			pTargetDeviceObject,
			pLowerDeivceObject
			);

		pFilterDeviceObject->DeviceType = pLowerDeivceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeivceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeivceObject->StackSize;
		pFilterDeviceObject->Flags |= pLowerDeivceObject->Flags & 
							(DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);

		// move to next device
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}

	return status;
}

VOID
c2pDetach(
IN PDEVICE_OBJECT pDeviceObject
)
{
	PC2P_DEV_EXT devExt;
	BOOLEAN NoRequestOutstanding = FALSE;
	devExt = (PC2P_DEV_EXT)pDeviceObject->DeviceExtension;
	__try
	{
		__try
		{
			IoDetachDevice(devExt->TargetDeviceObject);
			devExt->TargetDeviceObject = NULL;
			IoDeleteDevice(pDeviceObject);
			devExt->pFilterDeviceObject = NULL;
			KdPrint(("detach finished...\n"));
		}
		__except (EXCEPTION_EXECUTE_HANDLER){}
	}
	__finally{}
	return;
}

#define  DELAY_ONE_MICROSECOND  (-10)
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

VOID
c2pUnload(
IN PDRIVER_OBJECT DriverObject
)
{
	PDEVICE_OBJECT DeviceObject;
	PDEVICE_OBJECT OldDeviceObject;
	PC2P_DEV_EXT devExt;

	LARGE_INTEGER iDelay;
	PRKTHREAD CurrentThread;
	
	iDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MILLISECOND);
	CurrentThread = KeGetCurrentThread();
	// 将该线程改为低实时模式，因为后面要延时，不改会影响其他程序
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);

	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("driver entry unloading .... \n"));

	// 遍历所有设备并解除绑定
	DeviceObject = DriverObject->DeviceObject;
	while (DeviceObject)
	{
		c2pDetach(DeviceObject);
		DeviceObject = DeviceObject->NextDevice;
	}
	ASSERT(NULL == DriverObject->DeviceObject);

	while (gC2pKeyCount)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &iDelay);
	}
	KdPrint(("driver unload ok .... \n"));
	return;
}

NTSTATUS
c2pDispatchGeneral(
IN PDEVICE_OBJECT DeviceObject,
IN PIRP Irp
)
{
	KdPrint(("other dispatch\n"));
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PC2P_DEV_EXT)DeviceObject->DeviceExtension)->LowerDeviceObject, Irp);
}

NTSTATUS
c2pPower(
IN PDEVICE_OBJECT DeviceObject,
IN PIRP Irp
)
{
	PC2P_DEV_EXT devExt;
	devExt = (PC2P_DEV_EXT)DeviceObject->DeviceExtension;

	PoStartNextPowerIrp(Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return PoCallDriver(devExt->LowerDeviceObject, Irp);
}

NTSTATUS
c2pPnP(
IN PDEVICE_OBJECT DeviceObject,
IN PIRP Irp
)
{
	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION irpStack;
	NTSTATUS status = STATUS_SUCCESS;
	KIRQL oldIrql;
	KEVENT event;

	// 获取真实设备
	devExt = (PC2P_DEV_EXT)(DeviceObject->DeviceExtension);
	irpStack = IoGetCurrentIrpStackLocation(Irp);

	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		KdPrint(("irp mn remove device \n"));
		// 首先把请求下发
		IoSkipCurrentIrpStackLocation(Irp);
		IoCallDriver(devExt->LowerDeviceObject, Irp);
		// 然后解除绑定
		IoDetachDevice(devExt->LowerDeviceObject);
		// 删除生成的虚拟设备
		IoDeleteDevice(DeviceObject);
		status = STATUS_SUCCESS;
		break;

	default:
		// 其他的irp直接下发
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(devExt->LowerDeviceObject, Irp);
		break;
	}

	return status;
}


// 回调函数
NTSTATUS
c2pReadComplete(
IN PDEVICE_OBJECT DeviceObject,
IN PIRP Irp,
IN PVOID Context
)
{
	PIO_STACK_LOCATION irpStack;
	ULONG buf_len = 0;
	PUCHAR buf = NULL;
	size_t i;

	irpStack = IoGetCurrentIrpStackLocation(Irp);

	// 首先判断irp请求是否成功，
	// 若成功，则从请求中取出缓冲区信息，打印按键
	if (NT_SUCCESS(Irp->IoStatus.Status))
	{
		buf = Irp->AssociatedIrp.SystemBuffer;
		buf_len = Irp->IoStatus.Information;

		// 打印
		for (i = 0; i < buf_len; i++)
		{
			DbgPrint("capture key: %2x\r\n", buf[i]);
		}
	}
	gC2pKeyCount--;

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

NTSTATUS
c2pDispatchRead(
IN PDEVICE_OBJECT DeviceObject,
IN PIRP Irp
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PC2P_DEV_EXT devExt;
	PIO_STACK_LOCATION currentIrpStack;
	KEVENT waitEvent;
	KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);

	if (Irp->CurrentLocation == 1)
	{
		ULONG ReturnedInformation = 0;
		KdPrint(("dispatch encountered bogus current location. \n"));
		status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = ReturnedInformation;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	gC2pKeyCount++;
	devExt = (PC2P_DEV_EXT)DeviceObject->DeviceExtension;

	// 设置回掉函数
	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	IoCopyCurrentIrpStackLocationToNext(Irp);
	IoSetCompletionRoutine(Irp, c2pReadComplete, DeviceObject, TRUE, TRUE, TRUE);
	return IoCallDriver(devExt->LowerDeviceObject, Irp);
}

NTSTATUS
DriverEntry(
IN PDRIVER_OBJECT DriverObject,
IN PUNICODE_STRING RegistryPath
)
{
	ULONG i;
	NTSTATUS status;
	KdPrint(("c2p capture.sys entering driver entry\n"));

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = c2pDispatchGeneral;
	}

	// 
	DriverObject->MajorFunction[IRP_MJ_READ] = c2pDispatchRead;
	// 这类请求和电源有关
	DriverObject->MajorFunction[IRP_MJ_POWER] = c2pPower;

	DriverObject->MajorFunction[IRP_MJ_PNP] = c2pPnP;

	DriverObject->DriverUnload = c2pUnload;
	gDriverObject = DriverObject;

	status = c2pAttachDevice(DriverObject, RegistryPath);

	return status;
}