#ifndef __PRAMDISK
#define __PRAMDISK

#include <ntdddisk.h>

#define NT_DEVICE_NAME	L"\\Device\\PRamdisk"


typedef struct _DISK_INFO
{
	ULONG DiskSize;
} DISK_INFO, *PDISK_INFO;

typedef struct _DEVICE_EXTENSION
{
	PUCHAR DiskImage;
	DISK_GEOMETRY DiskGeometry;
	DISK_INFO DIskRegInfo;
	// 
	UNICODE_STRING SymbolicLink;
	
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// creates an accessor method with a specified name to access the device-extension
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, DeviceGetExtension);


DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD RamdiskEvtDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP RamdiskEvtDeviceContextCleanup;

#endif