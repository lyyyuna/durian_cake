#include <ntddk.h>
#include <wdf.h>
#include <ntintsafe.h>

#include "ramdisk.h"

/*
Microsoft��C/C++��������alloc_text��ʹ���ϼ���������������ƣ�

�ñ���ָʾ������ں������������������ǰ�档����԰����������е����к������е�һ��ͷ�ļ��У����ڰ�����ͷ�ļ���Դ�ļ��У���#include���ĺ���ʹ��alloc_text��
�ñ���ָʾ����������C������ʽ�ĺ����������������������Ա������C++Դ�ļ���δ��extern "C"�����ĺ�����
*/
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, RamdiskEvtDeviceAdd)
#pragma alloc_text(PAGE, RamdiskEvtDeviceContextCleanup)
#pragma alloc_text(PAGE, RamdiskQueryDiskRegParameters)
#pragma alloc_text(PAGE, RamdiskFormatDisk)
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

	// ��ʼ������
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

	// ������ ramdisk �йصĳ�ʼ��
	// driverletter�Ǵ����̷����Ǹ�unicode
	pDeviceExtension->DiskRegInfo.DriverLetter.Buffer = (PWSTR)&pDeviceExtension->DriverLetterBuffer;
	pDeviceExtension->DiskRegInfo.DriverLetter.MaximumLength = sizeof(pDeviceExtension->DriverLetterBuffer);

	// ��ע����л�ȡ������Ϣ
	RamdiskQueryDiskRegParameters(
		WdfDriverGetRegistryPath(WdfDeviceGetDriver(device)),
		&pDeviceExtension->DiskRegInfo
		);

	// �����̷����ڴ�
	pDeviceExtension->DiskImage = ExAllocatePoolWithTag(
		NonPagedPool,
		pDeviceExtension->DiskRegInfo.DiskSize,
		RAMDISK_TAG
		);
	// �������ɹ�
	if (pDeviceExtension->DiskImage)
	{
		UNICODE_STRING deviceName;
		UNICODE_STRING win32Name;

		RamdiskFormatDisk(pDeviceExtension);

		status = STATUS_SUCCESS;

		// �����������ӣ����ϲ�ʹ��
		RtlInitUnicodeString(&win32Name, DOS_DEVICE_NAME);
		RtlInitUnicodeString(&deviceName, NT_DEVICE_NAME);

		pDeviceExtension->SymbolicLink.Buffer = (PWSTR)&pDeviceExtension->DosDeviceNameBuffer;
		pDeviceExtension->SymbolicLink.MaximumLength = sizeof(pDeviceExtension->DosDeviceNameBuffer);
		pDeviceExtension->SymbolicLink.Length = win32Name.Length;

		RtlCopyUnicodeString(&pDeviceExtension->SymbolicLink, &win32Name);
		// \Device\Ramdisk �ǹ���ǰ׺��
		RtlAppendUnicodeStringToString(&pDeviceExtension->SymbolicLink, &pDeviceExtension->DiskRegInfo.DriverLetter);

		status = WdfDeviceCreateSymbolicLink(device, &pDeviceExtension->SymbolicLink);
	}

	return status;
}

VOID
RamdiskQueryDiskRegParameters(
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
	USHORT		fatEntries; // ����Ŀ
	USHORT		fatSectorCnt;
	PDIR_ENTRY	rootDir;

	PAGED_CODE();
	ASSERT(sizeof(BOOT_SECTOR) == 512);
	ASSERT(devExt->DiskImage != NULL);

	RtlZeroMemory(devExt->DiskImage, devExt->DiskRegInfo.DiskSize);

	devExt->DiskGeometry.BytesPerSector = 512;
	devExt->DiskGeometry.SectorsPerTrack = 32;
	devExt->DiskGeometry.TracksPerCylinder = 2;

	// ����������Ŀ
	devExt->DiskGeometry.Cylinders.QuadPart = devExt->DiskRegInfo.DiskSize / 512 / 32 / 2;

	devExt->DiskGeometry.MediaType = RAMDISK_MEDIA_TYPE;

	KdPrint((
		"Cylinders: %ld\n TracksPerCylinder: %ld\n SectorsPerTrack: %ld\n BytesPerSector: %ld\n",
		devExt->DiskGeometry.Cylinders.QuadPart, devExt->DiskGeometry.TracksPerCylinder,
		devExt->DiskGeometry.SectorsPerTrack, devExt->DiskGeometry.BytesPerSector
		));

	rootDirEntries = devExt->DiskRegInfo.RootDirEntries;
	sectorsPerCluster = devExt->DiskRegInfo.SectorsPerClusters;

	// ���ڸ�Ŀ¼��ڵ�ֻʹ��32�ֽڣ���������ռ��һ������������Ϊ�˳�����ÿռ䣬
	// ���û�ָ������Ŀ������ʱ�������������Ŀ����ʹ�����ռ�õ�������á�
	if (rootDirEntries & (DIR_ENTRIES_PER_SECTOR - 1))
	{
		rootDirEntries = (rootDirEntries + (DIR_ENTRIES_PER_SECTOR - 1)) & ~(DIR_ENTRIES_PER_SECTOR - 1);
	}

	KdPrint((
		"root dir entries: %ld\n sectors/cluster: %ld\n",
		rootDirEntries, sectorsPerCluster
		));

	// һ��ʼ����תָ�����΢���ƶ���Ӳ����
	bootSector->bsJump[0] = 0xeb;
	bootSector->bsJump[1] = 0x3c;
	bootSector->bsJump[2] = 0x90;

	// set oem name to "rajuram "
	bootSector->bsOemName[0] = 'R';
	bootSector->bsOemName[1] = 'a';
	bootSector->bsOemName[2] = 'j';
	bootSector->bsOemName[3] = 'u';
	bootSector->bsOemName[4] = 'R';
	bootSector->bsOemName[5] = 'a';
	bootSector->bsOemName[6] = 'm';
	bootSector->bsOemName[7] = ' ';

	bootSector->bsBytesPerSec = (SHORT)devExt->DiskGeometry.BytesPerSector;
	bootSector->bsResSectors = 1;
	bootSector->bsFATs = 1;
	bootSector->bsRootDirEnts = (USHORT)rootDirEntries;
	// ������������
	bootSector->bsSectors = (USHORT)(devExt->DiskRegInfo.DiskSize / devExt->DiskGeometry.BytesPerSector);
	bootSector->bsMedia = (UCHAR)devExt->DiskGeometry.MediaType;
	// ����ÿ���ص�������
	bootSector->bsSecPerClus = (UCHAR)sectorsPerCluster;

	// fat����Ŀ������������ȥ���������������ټ�ȥ��Ŀ¼��ڵ���ռ����������
	// Ȼ�����ÿ���ص���������
	// ��������� fat 0 1 �������ģ���+2
	fatEntries = (bootSector->bsSectors -
		bootSector->bsResSectors -
		bootSector->bsRootDirEnts / DIR_ENTRIES_PER_SECTOR) / bootSector->bsSecPerClus + 2;

	// ���ݴ���Ŀѡ�� fat12 or fat16
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
	bootSector->bsSecPerTrack = (USHORT)devExt->DiskGeometry.SectorsPerTrack;
	bootSector->bsHeads = (USHORT)devExt->DiskGeometry.TracksPerCylinder;
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

	// fat is located imediately following the boot sector
	// ��λfat���dbrֻ��һ������������fat�������׶�λ
	// ������� 0xFF
	firstFatSector = (PUCHAR)(bootSector + 1);
	firstFatSector[0] = (UCHAR)devExt->DiskGeometry.MediaType;
	firstFatSector[1] = 0xFF;
	firstFatSector[2] = 0xFF;

	if (fatType == 16)
	{
		firstFatSector[3] = 0xFF;
	}

	// root directory
	rootDir = (PDIR_ENTRY)(bootSector + 1 + fatSectorCnt);

	// �����豸����
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

BOOLEAN
RamdiskCheckParameters(
IN PDEVICE_EXTENSION devExt,
IN LARGE_INTEGER ByteOffset,
IN size_t	Length
)
{
	if (devExt->DiskRegInfo.DiskSize < Length ||
		ByteOffset.QuadPart < 0 || // QuadPart is signed so check for negative values
		((ULONGLONG)ByteOffset.QuadPart >(devExt->DiskRegInfo.DiskSize - Length)) ||
		(Length & (devExt->DiskGeometry.BytesPerSector - 1))) 
	{

		//
		// Do not give an I/O boost for parameter errors.
		//

		KdPrint((
			"Error invalid parameter\n"
			"ByteOffset: %x\n"
			"Length: %d\n",
			ByteOffset,
			Length
			));

		return FALSE;
	}

	return TRUE;
}

/*
When a driver creates an object, it sometimes allocates object-specific
memory buffers and stores the buffer pointers in the object's context space. 
The driver's EvtCleanupCallback or EvtDestroyCallback callback function can 
deallocate these memory buffers.
*/
VOID
RamdiskEvtDeviceContextCleanup(
IN WDFDEVICE Device
)
{
	PDEVICE_EXTENSION pDeviceExtension = DeviceGetExtension(Device);

	PAGED_CODE();

	if (pDeviceExtension->DiskImage)
	{
		ExFreePool(pDeviceExtension->DiskImage);
	}
}

VOID
RamdiskEvtIoRead(
IN WDFQUEUE Queue,
IN WDFREQUEST Request,
IN size_t Length
)
{
	PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	WDF_REQUEST_PARAMETERS Parameters;
	LARGE_INTEGER ByteOffset;
	WDFMEMORY hMemory;

	WDF_REQUEST_PARAMETERS_INIT(&Parameters);
	// �������л�ȡ������Ϣ
	WdfRequestGetParameters(Request, &Parameters);
	// ��������ʼλ��ȡ��
	ByteOffset.QuadPart = Parameters.Parameters.Read.DeviceOffset;
	// ������һ���Լ�ʵ�ֵ� ������캯����
	// ���ڶ�ȡ��Χ���ܳ������̾����С���ұ������������룬�����������Ҫ��һ��������顣
	if (RamdiskCheckParameters(devExt, ByteOffset, Length))
	{
		status = WdfRequestRetrieveOutputMemory(Request, &hMemory);
		if (NT_SUCCESS(status))
		{
			status = WdfMemoryCopyFromBuffer(hMemory,
				0,
				devExt->DiskImage + ByteOffset.LowPart,
				Length);
		}
			
	}

	WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR)Length);

}

VOID
RamdiskEvtIoWrite(
IN WDFQUEUE Queue,
IN WDFREQUEST Request,
IN size_t Length
)
{
	PDEVICE_EXTENSION devEvt = QueueGetExtension(Queue)->DeviceExtension;
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	WDF_REQUEST_PARAMETERS Parameters;
	LARGE_INTEGER ByteOffset;
	WDFMEMORY hMemory;

	WDF_REQUEST_PARAMETERS_INIT(&Parameters);

	WdfRequestGetParameters(Request, &Parameters);

	ByteOffset.QuadPart = Parameters.Parameters.Write.DeviceOffset;

	if (RamdiskCheckParameters(devEvt, ByteOffset, Length))
	{
		status = WdfRequestRetrieveInputMemory(Request, &hMemory);
		if (NT_SUCCESS(status))
		{
			status = WdfMemoryCopyToBuffer(hMemory,
				0,
				devEvt->DiskImage + ByteOffset.LowPart,
				Length);
		}
	}

	WdfRequestCompleteWithInformation(Request, status, (ULONG_PTR)Length);
}

VOID
RamdiskEvtIoDeviceControl(
IN WDFQUEUE Queue,
IN WDFREQUEST Request,
IN size_t OutputBufferLength,
IN size_t InputBufferLength,
IN ULONG IoControlCode
)
{
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	ULONG_PTR information = 0;
	size_t bufSize;
	PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;

	UNREFERENCED_PARAMETER(OutputBufferLength);
	UNREFERENCED_PARAMETER(InputBufferLength);

	switch (IoControlCode)
	{
	case IOCTL_DISK_GET_PARTITION_INFO:
	{
		PPARTITION_INFORMATION outputBuffer;
		PBOOT_SECTOR bootSector = (PBOOT_SECTOR)devExt->DiskImage;

		information = sizeof(PARTITION_INFORMATION);

		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(PARTITION_INFORMATION),
			&outputBuffer, &bufSize);
		if (NT_SUCCESS(status))
		{
			outputBuffer->PartitionType = (bootSector->bsFileSystemType[4] == '6') ? PARTITION_FAT_16 : PARTITION_FAT_12;

			outputBuffer->BootIndicator = FALSE;
			outputBuffer->RecognizedPartition = TRUE; 
			outputBuffer->RewritePartition = FALSE;
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
		information = sizeof(DISK_GEOMETRY);
		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(DISK_GEOMETRY), &outputBuffer, &bufSize);
		if (NT_SUCCESS(status))
		{
			RtlCopyMemory(outputBuffer, &(devExt->DiskGeometry), sizeof(DISK_GEOMETRY));
			status = STATUS_SUCCESS;
		}
	}
	break;

	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_DISK_IS_WRITABLE:
		status = STATUS_SUCCESS;
	
		break;

	}

	WdfRequestCompleteWithInformation(Request, status, information);
}