/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NUVOX53EX_H__
#define __NUVOX53EX_H__
   
#include "Platform.h"
//#include "System/SysInfra.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define NUVOX53EX_MAJOR_NUM   3
#define NUVOX53EX_MINOR_NUM   30
#define NUVOX53EX_BUILD_NUM   1
//#define NUVOX53EX_VERSION_NUM   _SYSINFRA_VERSION(NUVOX53EX_MAJOR_NUM, NUVOX53EX_MINOR_NUM, NUVOX53EX_BUILD_NUM)

// Constant Definition 
#define NUVOX53EX_DECODE_SAMPLE_PER_FRAME	240   	// counts per frame after decoding
#define NUVOX53EX_DECODE_WORK_BUF_SIZE		(170*2)	// bytes
#define NUVOX53EX_DECODE_TEMP_BUF_SIZE		(584*2)	// bytes


// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuVox53Ex decoder initiate.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUVOX53EX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuVox53Ex decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUVOX53EX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuVox53Ex decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		u32StartAddr [in]
//			The address of the first NuVox53Ex data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		pfnReadDataCallback [in]
//			Function to read NuVox53Ex data.
//
// Return Value
//		Sampling rate. If this value is 0, it represents decode failed in initiating.

// -----------------------------------------------------------------------------------------------------------------------
UINT32 NuVox53Ex_DecodeInitiate(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuVox53Ex decoder decode data to PCM buffer. 
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUVOX53EX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuVox53Ex decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUVOX53EX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuVox53Ex decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi16DecodedPcmBuf [out]
//			Buffer size is NUVOX53EX_DECODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read NuVox53Ex data.
//		pfnUserEventCallback [in]
//			Due to NuVox53Ex library does not support event and its value must be NULL.
//
// Return Value
//		Decoded PCM count. If this value is 0, it represents no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 NuVox53Ex_DecodeProcess(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT pfnUserEventCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check NuVox53Ex decode data finish or not.
//
// Parameter
//		pu8WorkBuf [in]
//			Buffer size is NUVOX53EX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuVox53Ex decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL NuVox53Ex_DecodeIsEnd(
	UINT8 *pu8DecodeWorkBuf
);

// -----------------------------------------------------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------------------------------------------------
UINT32 NuVox53Ex_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif	//#ifndef __NUVOX53EX_H__
