#include <ntddk.h>
#include <wdf.h>
#include <ntintsafe.h>

#include "pramdisk.h"

// allocate func into different location
#pragma	alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, RamdiskEvtDeviceAdd);




NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	WDF_DRIVER_CONFIG config;

	KdPrint(("ramdisk driver\r\n"));
	KdPrint(("built %s, %s", __DATE__, __TIME__));

	// bind EvtDrvierDeviceAdd to wdfconfig
	WDF_DRIVER_CONFIG_INIT(&config, RamdiskEvtDeviceAdd);

	return WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

}

NTSTATUS RamdiskEvtDeviceAdd(IN WDFDRIVER Drvier, IN PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES deviceAttributes;
	WDFDEVICE device;

	PDEVICE_EXTENSION pDeviceExtension;
	
	WDF_IO_QUEUE_CONFIG ioQueueConfig;
	WDF_OBJECT_ATTRIBUTES queueAttributes;
	WDFQUEUE queue;
	PQUEUE_EXTENSION pQueueExtension;


	DECLARE_CONST_UNICODE_STRING(ntDeviceName, NT_DEVICE_NAME);

	PAGED_CODE();

	status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("assign name failed!!!!\r\n"));
		return status;
	}
	// set some properties
	WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_DISK);
	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);
	WdfDeviceInitSetExclusive(DeviceInit, FALSE);

	// set device extension
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_EXTENSION);
	deviceAttributes.EvtCleanupCallback = RamdiskEvtDeviceContextCleanup;

	status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("device create failed!!! \r\n"));
		return status;
	}
	// get device extension
	pDeviceExtension = DeviceGetExtension(device);

	// init the queue
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchSequential);

	ioQueueConfig.EvtIoDeviceControl = RamdiskEvtIoDeviceControl;
	ioQueueConfig.EvtIoRead = RamdiskEvtIoRead;
	ioQueueConfig.EvtIoWrite = RamdiskEvtIoWrite;

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_EXTENSION);

	status = WdfIoQueueCreate(device, &ioQueueConfig, &queueAttributes, &queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("queue create failed!!! \r\n"));
		return status;
	}
	pQueueExtension = QueueGetExtension(queue);
	pQueueExtension->DeviceExtension = pDeviceExtension;

	pDeviceExtension->DiskRegInfo.DriverLetter.Buffer = (PWSTR)&pDeviceExtension->DriverLetterBuffer;
	pDeviceExtension->DiskRegInfo.DriverLetter.MaximumLength = sizeof(pDeviceExtension->DriverLetterBuffer);

	// retrieve info from registry
	RamdiskQueryDiskRegParameters(WdfDriverGetRegistryPath(WdfDeviceGetDriver(device)), &pDeviceExtension->DiskRegInfo);

	// alloc memory
	pDeviceExtension->DiskImage = ExAllocatePoolWithTag(NonPagedPool, pDeviceExtension->DiskRegInfo.DiskSize, RAMDISK_TAG);
	// if success
	if (pDeviceExtension->DiskImage)
	{
		UNICODE_STRING win32Name;

		RamdiskFormatDisk(pDeviceExtension);

		// setup symbolic
		RtlInitUnicodeString(&win32Name, DOS_DEVICE_NAME);
		pDeviceExtension->SymbolicLink.Buffer = (PWSTR)&pDeviceExtension->DosDeviceNameBuffer;
		pDeviceExtension->SymbolicLink.MaximumLength = sizeof(pDeviceExtension->DosDeviceNameBuffer);
		pDeviceExtension->SymbolicLink.Length = win32Name.Length;

		RtlCopyUnicodeString(&pDeviceExtension->SymbolicLink, &win32Name);
		RtlAppendUnicodeStringToString(&pDeviceExtension->SymbolicLink, &pDeviceExtension->DiskRegInfo.DriverLetter);

		status = WdfDeviceCreateSymbolicLink(device, &pDeviceExtension->SymbolicLink);
	}

	return status;
}

VOID RamdiskQueryDiskRegParameters(IN PWSTR RegistryPath, IN PDISK_INFO DiskRegInfo)
{
	NTSTATUS status;
	DISK_INFO defDiskRegInfo;
	RTL_QUERY_REGISTRY_TABLE rtlQueryRegTbl[5 + 1];


	PAGED_CODE();
	ASSERT(RegistryPath != NULL);

	// set default values
	defDiskRegInfo.DiskSize = DEFAULT_DISK_SIZE;
	defDiskRegInfo.RootDirEntries = DEFAULT_ROOT_DIR_ENTRIES;
	defDiskRegInfo.SectorsPerCluster = DEFAULT_SECTORS_PER_CLUSTER;
	RtlInitUnicodeString(&defDiskRegInfo.DriverLetter, DEFAULT_DRIVE_LETTER);

	// setup the query table
	RtlZeroMemory(rtlQueryRegTbl, sizeof(rtlQueryRegTbl));

	rtlQueryRegTbl[0].Flags = RTL_QUERY_REGISTRY_SUBKEY;
	rtlQueryRegTbl[0].Name = L"Parameters";
	rtlQueryRegTbl[0].EntryContext = NULL;
	rtlQueryRegTbl[0].DefaultType = (ULONG_PTR)NULL;
	rtlQueryRegTbl[0].DefaultData = NULL;
	rtlQueryRegTbl[0].DefaultLength = (ULONG_PTR)NULL;
	// Disk paramters
	rtlQueryRegTbl[1].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtlQueryRegTbl[1].Name = L"DiskSize";
	rtlQueryRegTbl[1].EntryContext = &DiskRegInfo->DiskSize;
	rtlQueryRegTbl[1].DefaultType = REG_DWORD;
	rtlQueryRegTbl[1].DefaultData = &defDiskRegInfo.DiskSize;
	rtlQueryRegTbl[1].DefaultLength = sizeof(ULONG);

	rtlQueryRegTbl[2].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtlQueryRegTbl[2].Name = L"RootDirEntries";
	rtlQueryRegTbl[2].EntryContext = &DiskRegInfo->RootDirEntries;
	rtlQueryRegTbl[2].DefaultType = REG_DWORD;
	rtlQueryRegTbl[2].DefaultData = &defDiskRegInfo.RootDirEntries;
	rtlQueryRegTbl[2].DefaultLength = sizeof(ULONG);

	rtlQueryRegTbl[3].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtlQueryRegTbl[3].Name = L"SectorsPerCluster";
	rtlQueryRegTbl[3].EntryContext = &DiskRegInfo->SectorsPerCluster;
	rtlQueryRegTbl[3].DefaultType = REG_DWORD;
	rtlQueryRegTbl[3].DefaultData = &defDiskRegInfo.SectorsPerCluster;
	rtlQueryRegTbl[3].DefaultLength = sizeof(ULONG);

	rtlQueryRegTbl[4].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtlQueryRegTbl[4].Name = L"DriveLetter";
	rtlQueryRegTbl[4].EntryContext = &DiskRegInfo->DriverLetter;
	rtlQueryRegTbl[4].DefaultType = REG_SZ;
	rtlQueryRegTbl[4].DefaultData = defDiskRegInfo.DriverLetter.Buffer;
	rtlQueryRegTbl[4].DefaultLength = 0;

	status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL, RegistryPath, rtlQueryRegTbl, NULL, NULL);
	if (FALSE == NT_SUCCESS(status))
	{
		DiskRegInfo->DiskSize = defDiskRegInfo.DiskSize;
		DiskRegInfo->RootDirEntries = defDiskRegInfo.RootDirEntries;
		DiskRegInfo->SectorsPerCluster = defDiskRegInfo.SectorsPerCluster;
		RtlCopyUnicodeString(&DiskRegInfo->DriverLetter, &defDiskRegInfo.DriverLetter);
	}

	// print queried data
	KdPrint(("DiskSize          = 0x%lx\n", DiskRegInfo->DiskSize));
	KdPrint(("RootDirEntries    = 0x%lx\n", DiskRegInfo->RootDirEntries));
	KdPrint(("SectorsPerCluster = 0x%lx\n", DiskRegInfo->SectorsPerCluster));
	KdPrint(("DriveLetter       = %wZ\n", &(DiskRegInfo->DriverLetter)));

	return;
}

NTSTATUS RamdiskFormatDisk(IN PDEVICE_EXTENSION pDeviceExtension)
{

}