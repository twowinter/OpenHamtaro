/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/
#ifndef __SEMIHOST_H__
#define __SEMIHOST_H__

#include "NUC1xx.h"
#include "System/SysInfra.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define SEMIHOST_MAJOR_NUM		1
#define SEMIHOST_MINOR_NUM		00
#define SEMIHOST_BUILD_NUM		001

#define SEMIHOST_VERSION_NUM _SYSINFRA_VERSION(SEMIHOST_MAJOR_NUM, SEMIHOST_MINOR_NUM, SEMIHOST_BUILD_NUM)

/******************************************************************************************
 * Description:
 * 	Version control API.
 * Arguments:
 *  None.
 ******************************************************************************************/

uint32_t
SemiHost_GetVersion(void);

void SemiHost_Init(void);

#ifdef  __cplusplus
}
#endif

#endif //__SEMIHOST_H__
