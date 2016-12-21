/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NUDACFILTEREX_H__
#define __NUDACFILTEREX_H__
   
#include "Platform.h"
#include "SysInfra.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define NUDACFILTEREX_MAJOR_NUM   3
#define NUDACFILTEREX_MINOR_NUM   30
#define NUDACFILTEREX_BUILD_NUM   1
#define NUDACFILTEREX_VERSION_NUM   _SYSINFRA_VERSION(NUDACFILTEREX_MAJOR_NUM, NUDACFILTEREX_MINOR_NUM, NUDACFILTEREX_BUILD_NUM)

// Constant Definition
#define NUDACFILTEREX_UP2_WORK_BUF_SIZE	(17*2)
#define NUDACFILTEREX_UP4_WORK_BUF_SIZE	( 9*2)

// ------------------------------------------------------------------------------------------------------
// Description
//		Initiate up2 sampling library.
//
// Parameter
//		pu8Up2WorkBuf [in]
//			Buffer address must be 2 bytes alignment.
//			Buffer size is NUDACFILTER_UP2_WORK_BUF_SIZE bytes.
//			This buffer is used to keep Up2 filter internal information and can not be used with others.
//
// Return Value
//		None.
// ------------------------------------------------------------------------------------------------------
void NuDACFilterEx_Up2Initial(
	UINT8 *pu8Up2WorkBuf
);

// ------------------------------------------------------------------------------------------------------
// Description
//		Process each inputed PCM to output 2 PCMs.
//
// Parameter
//		pu8Up2WorkBuf [in]
//			Buffer address must be 2 bytes alignment.
//			Buffer size is NUDACFILTER_UP2_WORK_BUF_SIZE bytes.
//			This buffer is used to keep Up2 filter internal information and can not be used with others.
//		i16PCM [in]
//			Input 1PCM.
//		pi16Up2PCMs [out]
//			Output 2 PCMs.
//
// Return Value
//		None.
// ------------------------------------------------------------------------------------------------------
void NuDACFilterEx_Up2Process(			
	UINT8 *pu8Up2WorkBuf,
	INT16 i16PCM,
	INT16 *pi16Up2PCMs
);

// ------------------------------------------------------------------------------------------------------
// Description
//		Initiate up 4 sampling library.
//
// Parameter
//		pu8Up4WorkBuf [in]
//			Buffer address must be 2 bytes alignment.
//			Buffer size is NUDACFILTER_UP4_WORK_BUF_SIZE bytes.
//			This buffer is used to keep Up4 filter internal information and can not be used with others.
//
// Return Value
//		None.
// ------------------------------------------------------------------------------------------------------
void NuDACFilterEx_Up4Initial(
	UINT8 *pu8Up4WorkBuf
);

// ------------------------------------------------------------------------------------------------------
// Description
//		Process each inputed PCM to output 4 PCMs.
// Parameter
//		pu8Up2WorkBuf [in]
//			Buffer address must be 2 bytes alignment.
//			Buffer size is NUDACFILTER_UP4_WORK_BUF_SIZE bytes.
//			This buffer is used to keep Up4 filter internal information and can not be used with others.
//		i16PCM [in]
//			Input 1PCM.
//		pi16Up4PCMs [out]
//			Output 4 PCMs.
// Return Value
//		None.
// ------------------------------------------------------------------------------------------------------
void NuDACFilterEx_Up4Process(
	UINT8 *pu8Up4WorkBuf,
	INT16 i16PCM,
	INT16 *pi16Up4PCMs
);

// ------------------------------------------------------------------------------------------------------
// Description
//		Return the current version number of library.
//
// Parmeter
//		None.
//
// Return Value
//		Version number :
//			bit 23:16: major number
//			bit 15:8:  minor number
//			bit 7:0:   build number
// ------------------------------------------------------------------------------------------------------
UINT32 NuDACFilterEx_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif	//#ifndef __NUDACFILTEREX_H__

