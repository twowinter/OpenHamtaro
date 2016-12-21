/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NUADCFILTEREX_H__
#define __NUADCFILTEREX_H__
   
#include "Platform.h"
#include "SysInfra.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define NUADCFILTEREX_MAJOR_NUM   3
#define NUADCFILTEREX_MINOR_NUM   30
#define NUADCFILTEREX_BUILD_NUM   1
#define NUADCFILTEREX_VERSION_NUM   _SYSINFRA_VERSION(NUADCFILTEREX_MAJOR_NUM, NUADCFILTEREX_MINOR_NUM, NUADCFILTEREX_BUILD_NUM)

// Constant Definition
#define NUADCFILTEREX_DOWN2_WORK_BUF_SIZE	 (33*2) 
#define NUADCFILTEREX_DOWN4_WORK_BUF_SIZE	 (33*2)

void NuADCFilterEx_Down2Initial(
	UINT8 *pu8Down2WorkBuf				// Buffer address must be 2 bytes alignment.
										// Buffer size is NUADCFILTER_DOWN2_WORK_BUF_SIZE bytes,
										// this buffer is used to keep Down2 filter internal information.
										// Can not be used with others.
										
);

INT16 NuADCFilterEx_Down2Process(		// Return 1 PCM
	UINT8 *pu8Down2WorkBuf,				// Buffer address must be 2 bytes alignment.
										// Buffer size is NUADCFILTER_DOWN2_WORK_BUF_SIZE bytes,
										// this buffer is used to keep Down2 filter internal information.
										// Can not be used with others.
	INT16 *pi16PCMs						// Input  2 ADC PCMs
);

void NuADCFilterEx_Down4Initial(
	UINT8 *pu8Down4WorkBuf				// Buffer address must be 2 bytes alignment.
										// Buffer size is NUADCFILTER_DOWN4_WORK_BUF_SIZE bytes,
										// this buffer is used to keep Down2 filter internal information.
										// Can not be used with others.
);

INT16 NuADCFilterEx_Down4Process(		// Return 1 PCM
	UINT8 *pu8Down4WorkBuf,				// Buffer address must be 2 bytes alignment.
										// Buffer size is NUADCFILTER_DOWN4_WORK_BUF_SIZE bytes,
										// this buffer is used to keep Down2 filter internal information.
										// Can not be used with others.
	INT16 *pi16PCMs						// Input  4 ADC PCMs
);

UINT32 NuADCFilterEx_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif	//#ifndef __NUADCFILTEREX_H__

