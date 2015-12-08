#ifndef __PRAMDISK
#define __PRAMDISK

#include <ntdddisk.h>

#define NT_DEVICE_NAME	L"\\Device\\PRamdisk"
#define DOS_DEVICE_NAME	 L"\\DosDevices\\"

#define DRIVE_LETTER_BUFFER_SIZE  10
#define DOS_DEVNAME_BUFFER_SZIE  (sizeof(DRIVE_LETTER_BUFFER_SIZE) + 10)

#define RAMDISK_TAG 'DmaR'
#define RAMDISK_MEDIA_TYPE              0xF8
#define DIR_ENTRIES_PER_SECTOR          16


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


#pragma pack(1)

typedef struct _BOOT_SECTOR
{
	UCHAR       bsJump[3];          // x86 jmp instruction, checked by FS
	CCHAR       bsOemName[8];       // OEM name of formatter
	USHORT      bsBytesPerSec;      // Bytes per Sector
	UCHAR       bsSecPerClus;       // Sectors per Cluster
	USHORT      bsResSectors;       // Reserved Sectors
	UCHAR       bsFATs;             // Number of FATs - we always use 1
	USHORT      bsRootDirEnts;      // Number of Root Dir Entries
	USHORT      bsSectors;          // Number of Sectors
	UCHAR       bsMedia;            // Media type - we use RAMDISK_MEDIA_TYPE
	USHORT      bsFATsecs;          // Number of FAT sectors
	USHORT      bsSecPerTrack;      // Sectors per Track - we use 32
	USHORT      bsHeads;            // Number of Heads - we use 2
	ULONG       bsHiddenSecs;       // Hidden Sectors - we set to 0
	ULONG       bsHugeSectors;      // Number of Sectors if > 32 MB size
	UCHAR       bsDriveNumber;      // Drive Number - not used
	UCHAR       bsReserved1;        // Reserved
	UCHAR       bsBootSignature;    // New Format Boot Signature - 0x29
	ULONG       bsVolumeID;         // VolumeID - set to 0x12345678
	CCHAR       bsLabel[11];        // Label - set to RamDisk
	CCHAR       bsFileSystemType[8];// File System Type - FAT12 or FAT16
	CCHAR       bsReserved2[448];   // Reserved
	UCHAR       bsSig2[2];          // Originial Boot Signature - 0x55, 0xAA
} BOOT_SECTOR, *PBOOT_SECTOR;


typedef struct  _DIR_ENTRY
{
	UCHAR       deName[8];          // File Name
	UCHAR       deExtension[3];     // File Extension
	UCHAR       deAttributes;       // File Attributes
	UCHAR       deReserved;         // Reserved
	USHORT      deTime;             // File Time
	USHORT      deDate;             // File Date
	USHORT      deStartCluster;     // First Cluster of file
	ULONG       deFileSize;         // File Length
}   DIR_ENTRY, *PDIR_ENTRY;

#pragma pack()



//
// Directory Entry Attributes
//
#define DIR_ATTR_READONLY   0x01
#define DIR_ATTR_HIDDEN     0x02
#define DIR_ATTR_SYSTEM     0x04
#define DIR_ATTR_VOLUME     0x08
#define DIR_ATTR_DIRECTORY  0x10
#define DIR_ATTR_ARCHIVE    0x20




DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD RamdiskEvtDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP RamdiskEvtDeviceContextCleanup;

EVT_WDF_IO_QUEUE_IO_READ RamdiskEvtIoRead;

EVT_WDF_IO_QUEUE_IO_WRITE RamdiskEvtIoWrite;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL RamdiskEvtIoDeviceControl;


VOID RamdiskQueryDiskRegParameters(IN PWSTR RegistryPath, IN PDISK_INFO DiskRegInfo);

NTSTATUS RamdiskFormatDisk(IN PDEVICE_EXTENSION deviceExtension);

BOOLEAN RamdiskCheckParameters(IN PDEVICE_EXTENSION devExt, IN LARGE_INTEGER ByteOffset, IN size_t Length);

#endif