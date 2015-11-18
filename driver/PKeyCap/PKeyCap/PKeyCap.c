#include <wdm.h>

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"


typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;
} C2P_DEV_EXT, *PC2P_DEV_EXT;


extern POBJECT_TYPE * IoDriverObjectType;

NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectTypePointer, KPROCESSOR_MODE AccessMode,
	PVOID ParseContext, PVOID * Object);

NTSTATUS c2pAttachDevice(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	UNICODE_STRING uni_DriverName;
	PDRIVER_OBJECT kbdDriverObject = NULL;

	PDEVICE_OBJECT pTargetDeviceObject;
	PDEVICE_OBJECT pLowerDeviceObject;
	PDEVICE_OBJECT pFilterDeviceObject;

	DbgPrint("myattach!\r\n");
	RtlInitUnicodeString(&uni_DriverName, KBD_DRIVER_NAME);

	status = ObReferenceObjectByName(&uni_DriverName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &kbdDriverObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("lyy's attach: cannot get the kbd driver object. ");
		return status;
	}
	else
	{
		// de-refer
		ObDereferenceObject(kbdDriverObject);
	}

	// get the device object
	pTargetDeviceObject = kbdDriverObject->DeviceObject;
	// all devices of the driver form a chain
	while (pTargetDeviceObject)
	{
		// create filter device
		status = IoCreateDevice(IN DriverObject, IN sizeof(C2P_DEV_EXT), IN NULL, IN pTargetDeviceObject->DeviceType, IN pTargetDeviceObject->Characteristics, IN FALSE, OUT pFilterDeviceObject);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("lyy's attach: cannot create filter device object! \r\n");
			return status;
		}
		// attach filter to lower device
		pLowerDeviceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
		if (!pLowerDeviceObject)
		{
			DbgPrint("lyy's attach: filter cannot attach to the device object. \r\n");
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}

		// device extension, store information

	}

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	ULONG i;
	NTSTATUS status;

	DbgPrint("c2p capture entering driver entry.\r\n");

	status = c2pAttachDevice(DriverObject, RegistryPath);

	return status;
}