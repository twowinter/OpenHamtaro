/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/

#ifndef __SYSINFRA_H__
#define __SYSINFRA_H__

// Include header file
#include "Platform.h"
#include "ModuleID.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define Constant
#define SYSINFRA_MAJOR_NUM		1
#define SYSINFRA_MINOR_NUM		00
#define SYSINFRA_BUILD_NUM		001

// Define a module version composed of major number, minor number, and build number.
#define _SYSINFRA_VERSION(MAJOR_NUM, MINOR_NUM, BUILD_NUM)          (((MAJOR_NUM) << 16) | ((MINOR_NUM) << 8) | (BUILD_NUM))

// Define an error code composed of error bit, module ID, and error ID.
#define _SYSINFRA_ERRCODE(IS_ERROR, MODULE_ID_VALUE, ERROR_ID)		((ERROR_ID) & 0xFF)

// Define module version number.
#define SYSINFRA_VERSION_NUM	_SYSINFRA_VERSION(SYSINFRA_MAJOR_NUM, SYSINFRA_MINOR_NUM, SYSINFRA_BUILD_NUM)

#define E_SUCCESS			0
#define S_OK				E_SUCCESS

// APIs declaration
UINT32 
SysInfra_GetVersion (void);

UINT32
SysInfra_CountLeadingZero (
	UINT32 u32Val
);

static __inline UINT32
SysInfra_CountLeadingOne (
	UINT32 u32Val
)
{
    return SysInfra_CountLeadingZero (~u32Val);
}

UINT32
SysInfra_CountTrailingZero (
	UINT32 u32Val
);

static __inline UINT32
SysInfra_CountTrailingOne (
	UINT32 u32Val
)
{
	return SysInfra_CountTrailingZero (~u32Val);
}


#ifdef __cplusplus
}
#endif

#endif // __SYSINFRA_H__



