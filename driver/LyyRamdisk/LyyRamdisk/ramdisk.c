#include <ntddk.h>
#include <wdf.h>
#include <ntintsafe.h>

#include "ramdisk.h"

/*
Microsoft的C/C++编译器在alloc_text的使用上加了两个讨厌的限制：

该编译指示必须跟在函数声明后面而不能在前面。你可以把驱动程序中的所有函数集中到一个头文件中，并在包含该头文件的源文件中，在#include语句的后面使用alloc_text。
该编译指示仅能用于有C连接形式的函数。即，它不能用于类成员函数或C++源文件中未用extern "C"声明的函数。
*/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, RamdiskEvtDeviceAdd)
#endif

NTSTATUS
DriverEntry(
IN PDRIVER_OBJECT DriverObject,
IN PUNICODE_STRING RegistryPath
)
{
	WDF_DRIVER_CONFIG config;

	KdPrint(("windows ramdisk driver\n"));
	KdPrint(("built %s, %s", __DATE__, __TIME__));

	WDF_DRIVER_CONFIG_INIT(&config, RamdiskEvtDeviceAdd);

	return WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
}

NTSTATUS
RamdiskEvtDeviceAdd(
IN WDFDRIVER Driver,
IN PWDFDEVICE_INIT DeviceInit
)
{
	WDF_OBJECT_ATTRIBUTES deviceAttributes;
	NTSTATUS status;
	WDFDEVICE device;
	WDF_OBJECT_ATTRIBUTES queueAttributes;
	WDF_IO_QUEUE_CONFIG ioQueueConfig;
	PDEVICE_EXTENSION pDeviceExtension;
	PQUEUE_EXTENSION pQueueContext;
	WDFQUEUE queue;
	DECLARE_CONST_UNICODE_STRING(ntDeviceName, NT_DEVICE_NAME);

	PAGED_CODE();

	UNREFERENCED_PARAMETER(Driver);

	status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("assign name failed \n"));
		return status;
	}

	WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_DISK);
	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);
	WdfDeviceInitSetExclusive(DeviceInit, FALSE);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_EXTENSION);
	deviceAttributes.EvtCleanupCallback = RamdiskEvtDeviceContextCleanup;

	status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("device create failed \n"));
		return status;
	}

	pDeviceExtension = DeviceGetExtension(device);

	// 初始化队列
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchSequential);

	ioQueueConfig.EvtIoDeviceControl	= RamdiskEvtIoDeviceControl;
	ioQueueConfig.EvtIoRead = RamdiskEvtIoRead;
	ioQueueConfig.EvtIoWrite = RamdiskEvtIoWrite;

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_EXTENSION);

	status = WdfIoQueueCreate(device,
		&ioQueueConfig,
		&queueAttributes,
		&queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("queue create failed \n"));
		return status;
	}

	pQueueContext = QueueGetExtension(queue);

	pQueueContext->DeviceExtension = pDeviceExtension;

	// 以下是 ramdisk 有关的初始化
	// driverletter是磁盘盘符，是个unicode
	pDeviceExtension->DiskRegInfo.DriverLetter.Buffer = (PWSTR)&pDeviceExtension->DriverLetterBuffer;
	pDeviceExtension->DiskRegInfo.DriverLetter.MaximumLength = sizeof(pDeviceExtension->DriverLetterBuffer);

	// 从注册表中获取磁盘信息
	RamdiskQueryDiskRegParameters(
		WdfDriverGetRegistryPath(WdfDeviceGetDriver(device)),
		&pDeviceExtension->DiskRegInfo
		);

	// 给磁盘分配内存
	pDeviceExtension->DiskImage = ExAllocatePoolWithTag(
		NonPagedPool,
		pDeviceExtension->DiskRegInfo.DiskSize,
		RAMDISK_TAG
		);
	// 如果分配成功
	if (pDeviceExtension->DiskImage)
	{
		UNICODE_STRING deviceName;
		UNICODE_STRING win32Name;

		RamdiskFormatDisk(pDeviceExtension);

		status = STATUS_SUCCESS;

		// 建立符号链接，供上层使用
		RtlInitUnicodeString(&win32Name, DOS_DEVICE_NAME);
		RtlInitUnicodeString(&deviceName, NT_DEVICE_NAME);

		pDeviceExtension->SymbolicLink.Buffer = (PWSTR)&pDeviceExtension->DosDeviceNameBuffer;
		pDeviceExtension->SymbolicLink.MaximumLength = sizeof(pDeviceExtension->DosDeviceNameBuffer);
		pDeviceExtension->SymbolicLink.Length = win32Name.Length;

		RtlCopyUnicodeString(&pDeviceExtension->SymbolicLink, &win32Name);
		// \Device\Ramdisk 是共用前缀？
		RtlAppendUnicodeStringToString(&pDeviceExtension->SymbolicLink, &pDeviceExtension->DiskRegInfo.DriverLetter);

		status = WdfDeviceCreateSymbolicLink(device, &pDeviceExtension->SymbolicLink);
	}

	return status;
}

VOID
RamdiskQuerydiskRegParameters(
IN PWSTR RegistryPath,
IN PDISK_INFO DiskRegInfo
)
{
	// PRTL_QUERY_REGISTRY_TABLE
	RTL_QUERY_REGISTRY_TABLE rtlQueryRegTbl[5 + 1];
	NTSTATUS	status;
	DISK_INFO	defDiskRegInfo;

	PAGED_CODE();

	ASSERT(RegistryPath != NULL);

	// set the default values;
	defDiskRegInfo.DiskSize = DEFAULT_DISK_SIZE;
	defDiskRegInfo.RootDirEntries = DEFAULT_ROOT_DIR_ENTRIES;
	defDiskRegInfo.SectorsPerClusters = DEFAULT_SECTORS_PER_CLUSTER;

	RtlInitUnicodeString(&defDiskRegInfo.DriverLetter, DEFAULT_DRIVE_LETTER);

	RtlZeroMemory(rtlQueryRegTbl, sizeof(rtlQueryRegTbl));

	//
	// Setup the query table
	//

	rtlQueryRegTbl[0].Flags = RTL_QUERY_REGISTRY_SUBKEY;
	rtlQueryRegTbl[0].Name = L"Parameters";
	rtlQueryRegTbl[0].EntryContext = NULL;
	rtlQueryRegTbl[0].DefaultType = (ULONG_PTR)NULL;
	rtlQueryRegTbl[0].DefaultData = NULL;
	rtlQueryRegTbl[0].DefaultLength = (ULONG_PTR)NULL;

	//
	// Disk paramters
	//

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
	rtlQueryRegTbl[3].EntryContext = &DiskRegInfo->SectorsPerClusters;
	rtlQueryRegTbl[3].DefaultType = REG_DWORD;
	rtlQueryRegTbl[3].DefaultData = &defDiskRegInfo.SectorsPerClusters;
	rtlQueryRegTbl[3].DefaultLength = sizeof(ULONG);

	rtlQueryRegTbl[4].Flags = RTL_QUERY_REGISTRY_DIRECT;
	rtlQueryRegTbl[4].Name = L"DriveLetter";
	rtlQueryRegTbl[4].EntryContext = &DiskRegInfo->DriverLetter;
	rtlQueryRegTbl[4].DefaultType = REG_SZ;
	rtlQueryRegTbl[4].DefaultData = defDiskRegInfo.DriverLetter.Buffer;
	rtlQueryRegTbl[4].DefaultLength = 0;

	status = RtlQueryRegistryValues(
		RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL,
		RegistryPath,
		rtlQueryRegTbl,
		NULL,
		NULL
		);

	if (FALSE == NT_SUCCESS(status))
	{
		DiskRegInfo->DiskSize = defDiskRegInfo.DiskSize;
		DiskRegInfo->RootDirEntries = defDiskRegInfo.RootDirEntries;
		DiskRegInfo->SectorsPerClusters = defDiskRegInfo.SectorsPerClusters;
		RtlCopyUnicodeString(&DiskRegInfo->DriverLetter, &defDiskRegInfo.DriverLetter);
	}

	KdPrint(("DiskSize          = 0x%lx\n", DiskRegInfo->DiskSize));
	KdPrint(("RootDirEntries    = 0x%lx\n", DiskRegInfo->RootDirEntries));
	KdPrint(("SectorsPerCluster = 0x%lx\n", DiskRegInfo->SectorsPerClusters));
	KdPrint(("DriveLetter       = %wZ\n", &(DiskRegInfo->DriverLetter)));

	return;
}

NTSTATUS
RamdiskFormatDisk(
IN PDEVICE_EXTENSION devExt
)
{
	PBOOT_SECTOR bootSector = (PBOOT_SECTOR)devExt->DiskImage;
	PUCHAR		firstFatSector;
	ULONG		rootDirEntries;
	ULONG		sectorsPerCluster;
	USHORT		fatType;	// fat12 or fat16
	USHORT		fatEntries; // 簇数目
	USHORT		fatSectorCnt;
	PDIR_ENTRY	rootDir;

	PAGED_CODE();
	ASSERT(sizeof(BOOT_SECTOR) == 512);
	ASSERT(devExt->DiskImage != NULL);

	RtlZeroMemory(devExt->DiskImage, devExt->DiskRegInfo.DiskSize);

	devExt->DiskGeometry.BytesPerSector = 512;
	devExt->DiskGeometry.SectorsPerTrack = 32;
	devExt->DiskGeometry.TracksPerCylinder = 2;

	// 计算柱面数目
	devExt->DiskGeometry.Cylinders.QuadPart = devExt->DiskRegInfo.DiskSize / 512 / 32 / 2;

	devExt->DiskGeometry.MediaType = RAMDISK_MEDIA_TYPE;

	KdPrint((
		"Cylinders: %ld\n TracksPerCylinder: %ld\n SectorsPerTrack: %ld\n BytesPerSector: %ld\n",
		devExt->DiskGeometry.Cylinders.QuadPart, devExt->DiskGeometry.TracksPerCylinder,
		devExt->DiskGeometry.SectorsPerTrack, devExt->DiskGeometry.BytesPerSector
		));

	rootDirEntries = devExt->DiskRegInfo.RootDirEntries;
	sectorsPerCluster = devExt->DiskRegInfo.SectorsPerClusters;

	if (rootDirEntries & (DIR_ENTRIES_PER_SECTOR - 1))
	{
		rootDirEntries = (rootDirEntries + (DIR_ENTRIES_PER_SECTOR - 1)) & ~(DIR_ENTRIES_PER_SECTOR - 1);
	}

}