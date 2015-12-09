#include <ntddk.h>
#include <wdf.h>
#include <ntintsafe.h>

#include "pramdisk.h"

// allocate func into different location
#pragma	alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, RamdiskEvtDeviceAdd)
#pragma alloc_text(PAGE, RamdiskEvtDeviceContextCleanup)
#pragma alloc_text(PAGE, RamdiskQueryDiskRegParameters)
#pragma alloc_text(PAGE, RamdiskFormatDisk)




NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	WDF_DRIVER_CONFIG config;

	KdPrint(("ramdisk driver\r\n"));
	KdPrint(("built %s, %s", __DATE__, __TIME__));

	// bind EvtDrvierDeviceAdd to wdfconfig
	WDF_DRIVER_CONFIG_INIT(&config, RamdiskEvtDeviceAdd);

	return WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

}

NTSTATUS RamdiskEvtDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
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

	UNREFERENCED_PARAMETER(Driver);


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
	PBOOT_SECTOR bootSector = (PBOOT_SECTOR)pDeviceExtension->DiskImage;

	ULONG rootDirEntries;
	ULONG sectorsPerCluster;
	USHORT fatEntries;
	USHORT fatType;
	USHORT fatSectorCnt;

	PUCHAR firstFatSector;
	PDIR_ENTRY rootDir;



	PAGED_CODE();
	ASSERT(sizeof(BOOT_SECTOR) == 512);
	ASSERT(pDeviceExtension->DiskImage != NULL);

	RtlZeroMemory(pDeviceExtension->DiskImage, pDeviceExtension->DiskRegInfo.DiskSize);

	pDeviceExtension->DiskGeometry.BytesPerSector = 512;
	pDeviceExtension->DiskGeometry.SectorsPerTrack = 32;
	pDeviceExtension->DiskGeometry.TracksPerCylinder = 2;
	// calculate number cylinders
	pDeviceExtension->DiskGeometry.Cylinders.QuadPart = pDeviceExtension->DiskRegInfo.DiskSize
		/ pDeviceExtension->DiskGeometry.BytesPerSector
		/ pDeviceExtension->DiskGeometry.SectorsPerTrack
		/ pDeviceExtension->DiskGeometry.TracksPerCylinder;

	pDeviceExtension->DiskGeometry.MediaType = RAMDISK_MEDIA_TYPE;

	KdPrint((
		"Cylinders: %ld\n TracksPerCylinder: %ld\n SectorsPerTrack: %ld\n BytesPerSector: %ld\n",
		pDeviceExtension->DiskGeometry.Cylinders.QuadPart, 
		pDeviceExtension->DiskGeometry.TracksPerCylinder,
		pDeviceExtension->DiskGeometry.SectorsPerTrack,
		pDeviceExtension->DiskGeometry.BytesPerSector
		));

	rootDirEntries = pDeviceExtension->DiskRegInfo.RootDirEntries;
	sectorsPerCluster = pDeviceExtension->DiskRegInfo.SectorsPerCluster;

	// 32 bytes for a directory entry
	// 512 / 32 = 16
	if (rootDirEntries & (DIR_ENTRIES_PER_SECTOR - 1))
	{
		rootDirEntries = (rootDirEntries + (DIR_ENTRIES_PER_SECTOR - 1)) & ~(DIR_ENTRIES_PER_SECTOR - 1);
	}

	KdPrint(("root dir entries: %ld\n sectors/cluster: %ld\n", rootDirEntries, sectorsPerCluster));

	// jmp
	bootSector->bsJump[0] = 0xeb;
	bootSector->bsJump[1] = 0x3c;
	bootSector->bsJump[2] = 0x90;
	// set oem name
	bootSector->bsOemName[0] = 'R';
	bootSector->bsOemName[1] = 'a';
	bootSector->bsOemName[2] = 'j';
	bootSector->bsOemName[3] = 'u';
	bootSector->bsOemName[4] = 'R';
	bootSector->bsOemName[5] = 'a';
	bootSector->bsOemName[6] = 'm';
	bootSector->bsOemName[7] = ' ';

	bootSector->bsBytesPerSec = (SHORT)pDeviceExtension->DiskGeometry.BytesPerSector;
	bootSector->bsResSectors = 1;
	// save 1, e, usually 2
	bootSector->bsFATs = 1;
	bootSector->bsRootDirEnts = (USHORT)rootDirEntries;
	// total sectors
	bootSector->bsSectors = (USHORT)(pDeviceExtension->DiskRegInfo.DiskSize / pDeviceExtension->DiskGeometry.BytesPerSector);
	bootSector->bsMedia = (UCHAR)pDeviceExtension->DiskGeometry.MediaType;
	// 
	bootSector->bsSecPerClus = (UCHAR)sectorsPerCluster;
	// num of fat entries
	fatEntries = (bootSector->bsSectors - bootSector->bsResSectors - bootSector->bsRootDirEnts / DIR_ENTRIES_PER_SECTOR) / bootSector->bsSecPerClus + 2;

	if (fatEntries > 4087)
	{
		fatType = 16;
		fatSectorCnt = (fatEntries * 2 + 511) / 512;
		fatEntries = fatEntries + fatSectorCnt;
		fatSectorCnt = (fatEntries * 2 + 511) / 512;
	}
	else
	{
		fatType = 12;
		fatSectorCnt = (((fatEntries * 3 + 1) / 2) + 511) / 512;
		fatEntries = fatEntries + fatSectorCnt;
		fatSectorCnt = (((fatEntries * 3 + 1) / 2) + 511) / 512;
	}

	bootSector->bsFATsecs = fatSectorCnt;
	bootSector->bsSecPerTrack = (USHORT)pDeviceExtension->DiskGeometry.SectorsPerTrack;
	bootSector->bsHeads = (USHORT)pDeviceExtension->DiskGeometry.TracksPerCylinder;
	bootSector->bsBootSignature = 0x29;
	bootSector->bsVolumeID = 0x12345678;

	// set label to "ramdisk "
	bootSector->bsLabel[0] = 'R';
	bootSector->bsLabel[1] = 'a';
	bootSector->bsLabel[2] = 'm';
	bootSector->bsLabel[3] = 'D';
	bootSector->bsLabel[4] = 'i';
	bootSector->bsLabel[5] = 's';
	bootSector->bsLabel[6] = 'k';
	bootSector->bsLabel[7] = ' ';
	bootSector->bsLabel[8] = ' ';
	bootSector->bsLabel[9] = ' ';
	bootSector->bsLabel[10] = ' ';

	// set filesystemtype to fat1?
	bootSector->bsFileSystemType[0] = 'F';
	bootSector->bsFileSystemType[1] = 'A';
	bootSector->bsFileSystemType[2] = 'T';
	bootSector->bsFileSystemType[3] = '1';
	bootSector->bsFileSystemType[4] = '?';
	bootSector->bsFileSystemType[5] = ' ';
	bootSector->bsFileSystemType[6] = ' ';
	bootSector->bsFileSystemType[7] = ' ';

	bootSector->bsFileSystemType[4] = (fatType == 16) ? '6' : '2';

	bootSector->bsSig2[0] = 0x55;
	bootSector->bsSig2[1] = 0xAA;


	// locate the first fat entry,
	// it just follows the DBR sector
	firstFatSector = (PUCHAR)(bootSector + 1);
	firstFatSector[0] = (UCHAR)pDeviceExtension->DiskGeometry.MediaType;
	firstFatSector[1] = 0xFF;
	firstFatSector[2] = 0xFF;

	// root directory
	rootDir = (PDIR_ENTRY)(bootSector + 1 + fatSectorCnt);
	// set device name
	rootDir->deName[0] = 'M';
	rootDir->deName[1] = 'S';
	rootDir->deName[2] = '-';
	rootDir->deName[3] = 'R';
	rootDir->deName[4] = 'A';
	rootDir->deName[5] = 'M';

	rootDir->deName[6] = 'D';
	rootDir->deName[7] = 'R';

	rootDir->deExtension[0] = 'I';
	rootDir->deExtension[1] = 'V';
	rootDir->deExtension[2] = 'E';

	rootDir->deAttributes = DIR_ATTR_VOLUME;

	return STATUS_SUCCESS;

}


BOOLEAN RamdiskCheckParameters(IN PDEVICE_EXTENSION devExt, IN LARGE_INTEGER ByteOffset, IN size_t Length)
{
	if (devExt->DiskRegInfo.DiskSize < Length ||
		ByteOffset.QuadPart < 0 || // QuadPart is signed so check for negative values
		((ULONGLONG)ByteOffset.QuadPart >(devExt->DiskRegInfo.DiskSize - Length)) ||
		(Length & (devExt->DiskGeometry.BytesPerSector - 1)))
	{
		KdPrint(("Error invalid parameter\n"
			"ByteOffset: %x\n"
			"Length: %d\n",
			ByteOffset, Length
			));

		return FALSE;
	}

	return TRUE;

}


VOID RamdiskEvtDeviceContextCleanup(IN WDFDEVICE Device)
{
	PDEVICE_EXTENSION pdvtExt = DeviceGetExtension(Device);

	PAGED_CODE();

	if (pdvtExt->DiskImage)
	{
		ExFreePool(pdvtExt->DiskImage);
	}
}


VOID RamdiskEvtIoRead(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length)
{
	PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;

	WDF_REQUEST_PARAMETERS Parameters;
	LARGE_INTEGER ByteOffset;

	NTSTATUS status = STATUS_INVALID_PARAMETER;
	WDFMEMORY hMemory;

	WDF_REQUEST_PARAMETERS_INIT(&Parameters);
	WdfRequestGetParameters(Request, &Parameters);

	ByteOffset.QuadPart = Parameters.Parameters.Read.DeviceOffset;
	// check parameters
	if (RamdiskCheckParameters(devExt, ByteOffset, Length))
	{
		status = WdfRequestRetrieveOutputMemory(Request, &hMemory);
		if (NT_SUCCESS(status))
		{
			status = WdfMemoryCopyFromBuffer(hMemory, 0, devExt->DiskImage + ByteOffset.LowPart, Length);

		}
	}

	WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR)Length);
}

VOID RamdiskEvtIoWrite(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length)
{
	PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;
	WDF_REQUEST_PARAMETERS Parameters;
	LARGE_INTEGER ByteOffset;

	NTSTATUS status = STATUS_INVALID_PARAMETER;
	WDFMEMORY hMemory;

	WDF_REQUEST_PARAMETERS_INIT(&Parameters);
	WdfRequestGetParameters(Request, &Parameters);
	ByteOffset.QuadPart = Parameters.Parameters.Write.DeviceOffset;

	if (RamdiskCheckParameters(devExt, ByteOffset, Length))
	{
		status = WdfRequestRetrieveInputMemory(Request, &hMemory);
		if (NT_SUCCESS(status))
		{
			status = WdfMemoryCopyToBuffer(hMemory, 0, devExt->DiskImage + ByteOffset.LowPart, Length);
		}
	}


	WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR)Length);
}



VOID RamdiskEvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength, IN size_t InputBufferLength, IN ULONG IoControlCode)
{

	PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;

	NTSTATUS status;
	size_t bufSize;

	ULONG_PTR information = 0;


	UNREFERENCED_PARAMETER(InputBufferLength);
	UNREFERENCED_PARAMETER(OutputBufferLength);

	switch (IoControlCode)
	{
	case IOCTL_DISK_GET_PARTITION_INFO:
	{
		PPARTITION_INFORMATION outputBuffer;
		PBOOT_SECTOR bootSector = (PBOOT_SECTOR)devExt->DiskImage;

		information = sizeof(PARTITION_INFORMATION);

		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(PARTITION_INFORMATION), &outputBuffer, &bufSize);
		if (NT_SUCCESS(status))
		{
			outputBuffer->PartitionType = (bootSector->bsFileSystemType[4] == '6') ? PARTITION_FAT_16 : PARTITION_FAT_12;
			outputBuffer->BootIndicator = FALSE;
			outputBuffer->RewritePartition = TRUE;
			outputBuffer->StartingOffset.QuadPart = 0;
			outputBuffer->PartitionLength.QuadPart = devExt->DiskRegInfo.DiskSize;
			outputBuffer->HiddenSectors = (ULONG)(1L);
			outputBuffer->PartitionNumber = (ULONG)(-1L);

			status = STATUS_SUCCESS;
		}
	}
	break;

	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	{
		PDISK_GEOMETRY outputBuffer;

		information = sizeof(PARTITION_INFORMATION);

		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(DISK_GEOMETRY), &outputBuffer, &bufSize);
		if (NT_SUCCESS(status))
		{
			RtlCopyMemory(outputBuffer, &(devExt->DiskGeometry), sizeof(DISK_GEOMETRY));
			status = STATUS_SUCCESS;
		}
		
	}
	break;


	default:
		status = STATUS_SUCCESS;
		break;
	}

	WdfRequestCompleteWithInformation(Request, status, information);
}