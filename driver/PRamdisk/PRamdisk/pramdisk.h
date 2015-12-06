#ifndef __PRAMDISK
#define __PRAMDISK

#include <ntdddisk.h>

#define NT_DEVICE_NAME	L"\\Device\\PRamdisk"
#define DOS_DEVICE_NAME	 L"\\DosDevices\\"

#define DRIVE_LETTER_BUFFER_SIZE  10
#define DOS_DEVNAME_BUFFER_SZIE  (sizeof(DRIVE_LETTER_BUFFER_SIZE) + 10)

#define RAMDISK_TAG 'DmaR'

// default disk property
#define DEFAULT_DISK_SIZE               (1024*1024)     // 1 MB
#define DEFAULT_ROOT_DIR_ENTRIES        512
#define DEFAULT_SECTORS_PER_CLUSTER     2
#define DEFAULT_DRIVE_LETTER            L"Z:"

typedef struct _DISK_INFO
{
	ULONG DiskSize;
	ULONG RootDirEntries;
	ULONG SectorsPerCluster;
	UNICODE_STRING DriverLetter;
} DISK_INFO, *PDISK_INFO;

typedef struct _DEVICE_EXTENSION
{
	PUCHAR DiskImage;
	DISK_GEOMETRY DiskGeometry;
	DISK_INFO DiskRegInfo;
	// 
	UNICODE_STRING SymbolicLink;

	WCHAR DriverLetterBuffer[DRIVE_LETTER_BUFFER_SIZE];
	WCHAR DosDeviceNameBuffer[DOS_DEVNAME_BUFFER_SZIE];
	
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// creates an accessor method with a specified name to access the device-extension
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, DeviceGetExtension);

typedef struct _QUEUE_EXTENSION
{
	PDEVICE_EXTENSION DeviceExtension;
} QUEUE_EXTENSION, *PQUEUE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_EXTENSION, QueueGetExtension);


DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD RamdiskEvtDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP RamdiskEvtDeviceContextCleanup;

EVT_WDF_IO_QUEUE_IO_READ RamdiskEvtIoRead;

EVT_WDF_IO_QUEUE_IO_WRITE RamdiskEvtIoWrite;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL RamdiskEvtIoDeviceControl;


VOID RamdiskQueryDiskRegParameters(IN PWSTR RegistryPath, IN PDISK_INFO DiskRegInfo);

NTSTATUS RamdiskFormatDisk(IN PDEVICE_EXTENSION deviceExtension);


#endif