#include <ntddk.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#ifndef SetFlag
#define SetFlag(_F,_SF)       ((_F) |= (_SF))
#endif
#ifndef ClearFlag
#define ClearFlag(_F,_SF)     ((_F) &= ~(_SF))
#endif
#define CCP_MAX_COM_ID 32

static PDEVICE_OBJECT s_fltobj[CCP_MAX_COM_ID] = { 0 };
static PDEVICE_OBJECT s_nextobj[CCP_MAX_COM_ID] = { 0 };

PDEVICE_OBJECT 
ccpOpenCom(
		ULONG id, 
		NTSTATUS *status)
{
	UNICODE_STRING name_str;
	static WCHAR name[32] = { 0 };
	PFILE_OBJECT fileobj = NULL;
	PDEVICE_OBJECT devobj = NULL;

	memset(name, 0, sizeof(WCHAR) * 32);
	RtlStringCchPrintfW(
		name,
		32,
		L"\\Device\\Serial%d",
		id
		);
	RtlInitUnicodeString(&name_str, name);

	*status = IoGetDeviceObjectPointer(&name_str, FILE_ALL_ACCESS, &fileobj, &devobj);
	if (*status == STATUS_SUCCESS)
		ObDereferenceObject(fileobj);

	return devobj;
}

NTSTATUS 
ccpAttachDevice(
PDRIVER_OBJECT driver,
PDEVICE_OBJECT oldobj,
PDEVICE_OBJECT *fltobj,
PDEVICE_OBJECT *next
)
{
	NTSTATUS status;
	PDEVICE_OBJECT topdev = NULL;

	// 生成过滤设备
	status = IoCreateDevice(
		driver,
		0,
		NULL,
		oldobj->DeviceType,
		0,
		FALSE,
		fltobj);
	if (status != STATUS_SUCCESS)
		return status;

	// 拷贝一些重要标志位
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Flags & DO_DIRECT_IO)
		(*fltobj)->Flags |= DO_DIRECT_IO;
	if (oldobj->Characteristics & FILE_DEVICE_SECURE_OPEN)
		(*fltobj)->Characteristics |= FILE_DEVICE_SECURE_OPEN;

	(*fltobj)->Flags |= DO_POWER_PAGABLE;
	// 绑定到真实设备上
	topdev = IoAttachDeviceToDeviceStack(*fltobj, oldobj);
	if (topdev == NULL)
	{
		// 绑定失败，销毁
		IoDeleteDevice(*fltobj);
		*fltobj = NULL;
		status = STATUS_UNSUCCESSFUL;
		return status;
	}

	*next = topdev;
	// set as initialized
	(*fltobj)->Flags = (*fltobj)->Flags & ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

// 绑定所有的串口
VOID
ccpAttachAllComs(
PDRIVER_OBJECT driver
)
{
	ULONG i;
	PDEVICE_OBJECT com_ob;
	NTSTATUS status;

	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		//  获得object引用
		com_ob = ccpOpenCom(i, &status);
		if (com_ob == NULL)
			continue;
		ccpAttachDevice(driver, com_ob, &s_fltobj[i], &s_nextobj[i]);
	}
}


#define  DELAY_ONE_MICROSECOND  (-10)
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

VOID 
ccpUnload(
	PDRIVER_OBJECT driver
)
{
	ULONG i;
	LARGE_INTEGER interval;
	DbgPrint("\r\bye bye\r\n");
	// detach all devices
	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_nextobj[i] != NULL)
			IoDetachDevice(s_nextobj[i]);
	}

	// sleep 5 seconds,  for all irps have been handled
	interval.QuadPart = (5 * 1000 * DELAY_ONE_MILLISECOND);
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	// delete all devices
	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_fltobj[i] != NULL)
			IoDeleteDevice(s_fltobj[i]);
	}
}

NTSTATUS
ccpDispatch(
PDEVICE_OBJECT device,
PIRP irp
)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status;
	ULONG i, j; 
	
	
	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		// first, find the exact device the irp is sent to
		if (s_fltobj[i] == device)
		{
			// all power action, passed directly
			if (irpsp->MajorFunction == IRP_MJ_POWER)
			{
				// sent to next directly
				PoStartNextPowerIrp(irp);
				IoSkipCurrentIrpStackLocation(irp);
				return PoCallDriver(s_nextobj[i], irp);
			}


			// 只过滤写请求
			// 获得写请求缓冲区
			if (irpsp->MajorFunction == IRP_MJ_WRITE)
			{
				ULONG len = irpsp->Parameters.Write.Length;
				PUCHAR buf = NULL;
				if (irp->MdlAddress != NULL)
					buf = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				else
					buf = (PUCHAR)irp->UserBuffer;
				if (buf == NULL)
					buf = (PUCHAR)irp->AssociatedIrp.SystemBuffer;

				for (j = 0; j < len; j++)
				{
					DbgPrint("comcap: send data: %2x\r\n", buf[j]);
				}
			}

			// 直接将这些请求发送给真实设备，不改变
			IoSkipCurrentIrpStackLocation(irp);
			return IoCallDriver(s_nextobj[i], irp);
		}
	}

	// it seems that it is not in the attached device
	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
PDRIVER_OBJECT driver,
PUNICODE_STRING reg_path
)
{
	ULONG i;
	DbgPrint("\r\nhello\r\n");
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = ccpDispatch;
	}

	driver->DriverUnload = ccpUnload;

	ccpAttachAllComs(driver);

	return STATUS_SUCCESS;
}