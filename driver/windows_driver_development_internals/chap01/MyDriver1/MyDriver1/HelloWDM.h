#if __cplusplus
extern "C"
{
#endif
#include <wdm.h>
#ifdef __cplusplus
}
#endif

typedef struct _DEVICE_EXTERSION
{
	PDEVICE_OBJECT fdo;
	PDEVICE_OBJECT NextStatckDevice;
	UNICODE_STRING ustrDeviceName;		//设备名
	UNICODE_STRING ustrSymLinkName;		//符号链接名
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_set("INIT")

#define PAGEDDATA data_set("PAGE")
#define LOCKEDDATA data_set()
#define INITDATA data_set("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT DriverObject, IN PDEVICE_OBJECT PhysicalDeviceObject);
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP Irp);
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp);
void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS DefaultPnpHandler(PDEVICE_EXTENSION pdx, PIRP Irp);
NTSTATUS HandleRemoveDevice(PDEVICE_EXTENSION pdx, PIRP Irp);

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);


