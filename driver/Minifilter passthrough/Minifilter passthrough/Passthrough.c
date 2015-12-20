#include <fltKernel.h>

#include "Passthrough.h"

PFLT_FILTER gFilterHandle;

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, PtUnload)
#pragma alloc_text(PAGE, PtInstanceQueryTeardown)
#pragma alloc_text(PAGE, PtInstanceSetup)
#pragma alloc_text(PAGE, PtInstanceTearDownStart)
#pragma alloc_text(PAGE, PtInstanceTearDownComplete)


CONST FLT_OPERATION_REGISTRATION Callbacks[] =
{
	{ IRP_MJ_CREATE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_CREATE_NAMED_PIPE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_CLOSE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_READ,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_WRITE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_INFORMATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SET_INFORMATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_EA,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SET_EA,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_FLUSH_BUFFERS,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_VOLUME_INFORMATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SET_VOLUME_INFORMATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_DIRECTORY_CONTROL,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_FILE_SYSTEM_CONTROL,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_DEVICE_CONTROL,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_INTERNAL_DEVICE_CONTROL,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SHUTDOWN,
	0,
	PtPreOperationNoPostOperationPassThrough,
	NULL },

	{ IRP_MJ_LOCK_CONTROL,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_CLEANUP,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_CREATE_MAILSLOT,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_SECURITY,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SET_SECURITY,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough, },

	{ IRP_MJ_QUERY_QUOTA,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_SET_QUOTA,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_PNP,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_MOD_WRITE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_CC_FLUSH,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_NETWORK_QUERY_OPEN,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_MDL_READ,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_MDL_READ_COMPLETE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_PREPARE_MDL_WRITE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_MDL_WRITE_COMPLETE,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_VOLUME_MOUNT,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_VOLUME_DISMOUNT,
	0,
	PtPreOperationPassThrough,
	PtPostOperationPassThrough },

	{ IRP_MJ_OPERATION_END }
};

CONST FLT_REGISTRATION FilterRegistration =
{
	sizeof(FLT_REGISTRATION),			// size
	FLT_REGISTRATION_VERSION,			// version
	0,									// flags
	NULL,								// context
	Callbacks,							// operation callbacks
	PtUnload,							// minifilter unload
	PtInstanceSetup,					//
	PtInstanceQueryTeardown,
	PtInstanceTearDownStart,
	PtInstanceTearDownComplete,
	NULL,								// generate file name
	NULL,								// generate dest file name
	NULL								// normalize name compenent
};

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(RegistryPath);

	KdPrint(("Passthrough!DriverEntry: entered\n"));

	status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);

	FLT_ASSERT(NT_SUCCESS(status));

	if (NT_SUCCESS(status))
	{
		status = FltStartFiltering(gFilterHandle);
		if (!NT_SUCCESS(status))
		{
			FltUnregisterFilter(gFilterHandle);
		}
		KdPrint(("Passthrough!DriverEntry: start mini filter \n"));
	}

	return status;
}

NTSTATUS PtUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
	PAGED_CODE();

	UNREFERENCED_PARAMETER(Flags);

	KdPrint(("Passthrough!PtUnload: Entered!!!"));

	FltUnregisterFilter(gFilterHandle);

	return STATUS_SUCCESS;
}


ULONG_PTR OperationStatusCtx = 1;
FLT_PREOP_CALLBACK_STATUS PtPreOperationPassThrough(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID * CompletionContext)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(FltObjects);


	KdPrint(("passthrough!ptpreoperaton passthrough: entered!!!!"));

	if (PtDoRequestOperationStatus(Data))
	{
		status = FltRequestOperationStatusCallback(Data, PtOperationStatusCallback, (PVOID)(++OperationStatusCtx));

		if (!NT_SUCCESS(status))
		{
			KdPrint(("PassThrough!PtPreOperationPassThrough: FltRequestOperationStatusCallback Failed, status=%08x\n",
				status));
		}
	}

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

VOID PtOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext)
{
	UNREFERENCED_PARAMETER(FltObjects);


	KdPrint(("PassThrough!PtOperationStatusCallback: Entered\n"));
	KdPrint(("PassThrough!PtOperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
		OperationStatus,
		RequesterContext,
		ParameterSnapshot->MajorFunction,
		ParameterSnapshot->MinorFunction,
		FltGetIrpName(ParameterSnapshot->MajorFunction)));
}

// this identify operations we want operation status for
// typically STATUS_PENDING
BOOLEAN PtDoRequestOperationStatus(_In_ PFLT_CALLBACK_DATA Data)
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;

	return (BOOLEAN)
		(
			(
			(iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL)
			&&
			(iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY)
			)

			||

			(
			(iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL)
			&&
			((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK) ||
			(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK) ||
			(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
			(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2))
			)
		);
}

FLT_POSTOP_CALLBACK_STATUS PtPostOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Data);

	KdPrint(("PassThrough!PtPostOperationPassThrough: Entered\n"));
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
PtPreOperationNoPostOperationPassThrough(
_Inout_ PFLT_CALLBACK_DATA Data,
_In_ PCFLT_RELATED_OBJECTS FltObjects,
_Flt_CompletionContext_Outptr_ PVOID * CpmpletionContext
)
{
	UNREFERENCED_PARAMETER(CpmpletionContext);
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);

	KdPrint(("PassThrough!PtPreOperationNoPostOperationPassThrough: Entered\n"));

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


NTSTATUS
PtInstanceSetup(
_In_ PCFLT_RELATED_OBJECTS FltObjects,
_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
_In_ DEVICE_TYPE VolumeDeviceType,
_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	PAGED_CODE();

	KdPrint(("passthrough!ptinstancesetup: entered!!!!!!!"));

	return STATUS_SUCCESS;
}


NTSTATUS
PtInstanceQueryTeardown(
_In_ PCFLT_RELATED_OBJECTS FltObjects,
_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	KdPrint(("passthrough! ptinstancequery teardown: entered !!!!!!!"));

	return STATUS_SUCCESS;
}


VOID
PtInstanceTearDownStart(
_In_ PCFLT_RELATED_OBJECTS FltObjects,
_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	KdPrint(("passthrough! ptinstance teardown start: entered !!!!!!!"));

	
}

VOID
PtInstanceTearDownComplete(
_In_ PCFLT_RELATED_OBJECTS FltObjects,
_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	KdPrint(("passthrough! ptinstance teardown complete: entered 1!!!!"));
}