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
}