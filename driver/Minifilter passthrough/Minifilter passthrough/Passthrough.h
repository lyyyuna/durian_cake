#ifndef __PASSTHROUGH__
#define __PASSTHROUGH__

#include <fltKernel.h>

CONST FLT_OPERATION_REGISTRATION Callbacks[] = 
{
	{IRP_MJ_OPERATION_END}
};

CONST FLT_REGISTRATION FilterRegistration = 
{
	sizeof(FLT_REGISTRATION)
};


DRIVER_INITIALIZE DriverEntry;

#endif