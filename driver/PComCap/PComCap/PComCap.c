#include <ntddk.h>
//#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

PDEVICE_OBJECT ccpOpenCom(ULONG id, NTSTATUS * ntstatus)
{
	static WCHAR name[32] = { 0 };
	UNICODE_STRING name_str;
	PFILE_OBJECT fileobj = NULL;
	PDEVICE_OBJECT devobj = NULL;

	memset(name, 0, sizeof(WCHAR) * 32);
	RtlStringCchPrintfW(name, 32, L"\\Device\\Serial%d", id);
	RtlInitUnicodeString(&name_str, name);

	*ntstatus = IoGetDeviceObjectPointer(&name_str, FILE_ALL_ACCESS, &fileobj, &devobj);
	if (*ntstatus == STATUS_SUCCESS)
	{
		ObDereferenceObject(fileobj);
	}

	return devobj;
}

NTSTATUS ccpAttachDevice(PDRIVER_OBJECT driver,
	PDEVICE_OBJECT oldobj,
	PDEVICE_OBJECT * fltobj,
	PDEVICE_OBJECT * next)
{
	NTSTATUS status;
	PDEVICE_OBJECT topdev = NULL;

	status = IoCreateDevice(driver, 0, NULL, oldobj->DeviceType, 0, FALSE, fltobj);

	if (status != STATUS_SUCCESS)
	{
		return status;
	}

	// copy important flags
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Characteristics & FILE_DEVICE_SECURE_OPEN)
		(*fltobj)->Characteristics |= FILE_DEVICE_SECURE_OPEN;

	(*fltobj)->Flags |= DO_POWER_PAGABLE;
	// attach device
	topdev = IoAttachDeviceToDeviceStack(*fltobj, oldobj);

	if (topdev == NULL)
	{
		// attach failed
		IoDeleteDevice(*fltobj);
		*fltobj = NULL;
		status = STATUS_UNSUCCESSFUL;
		return status;
	}

	*next = topdev;
	// have finished init
	(*fltobj)->Flags = (*fltobj)->Flags & ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}


#define CCP_MAX_COM_ID 32
static PDEVICE_OBJECT s_fltobj[CCP_MAX_COM_ID] = { 0 };
static PDEVICE_OBJECT s_nextobj[CCP_MAX_COM_ID] = { 0 };

VOID ccpAttachAllComs(PDRIVER_OBJECT driver)
{
	ULONG i;
	PDEVICE_OBJECT com_ob;
	NTSTATUS status;

	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		com_ob = ccpOpenCom(i, &status);
		if (com_ob == NULL)
			continue;
		ccpAttachDevice(driver, com_ob, &s_fltobj[i], &s_nextobj[i]);
	}
}

#define  DELAY_ONE_MICROSECOND  (-10)
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

VOID ccpUnload(PDRIVER_OBJECT driver)
{
	ULONG i;
	LARGE_INTEGER interval;

	(VOID)driver;
	DbgPrint("\r\nbye bye\r\n");

	// detach all devices
	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_nextobj[i] != NULL)
			IoDetachDevice(s_nextobj[i]);
	}

	// sleep for a while
	interval.QuadPart = (5 * 1000 * DELAY_ONE_MILLISECOND);
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	// delete all devices
	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_fltobj[i] != NULL)
		{
			IoDeleteDevice(s_fltobj[i]);
		}
	}
}

NTSTATUS ccpDispatch(PDEVICE_OBJECT device, PIRP irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	// NTSTATUS status;

	ULONG i, j;

	for (i = 0; i < CCP_MAX_COM_ID; i++)
	{
		// first determine which device
		if (s_fltobj[i] == device)
		{
			// pass power action directly
			if (irpsp->MajorFunction == IRP_MJ_POWER)
			{
				// send to next stack location directly
				PoStartNextPowerIrp(irp);
				IoSkipCurrentIrpStackLocation(irp);
				return PoCallDriver(s_nextobj[i], irp);
			}

			// filter the write
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

			// skip to real device
			IoSkipCurrentIrpStackLocation(irp);
			return IoCallDriver(s_nextobj[i], irp);
		}
	}

	// oop, not in the attached device
	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	ULONG i;
	_asm int 3;
	(VOID)reg_path;
	DbgPrint("\r\nhello\t\n");
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = ccpDispatch;
	}

	driver->DriverUnload = ccpUnload;

	ccpAttachAllComs(driver);

	return STATUS_SUCCESS;
}