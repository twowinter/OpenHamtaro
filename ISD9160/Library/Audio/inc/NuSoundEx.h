/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NUSOUNDEX_H__
#define __NUSOUNDEX_H__

#include "Platform.h"
#include "SysInfra.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define	NUSOUNDEX_MAJOR_NUM 3
#define	NUSOUNDEX_MINOR_NUM 30
#define	NUSOUNDEX_BUILD_NUM 1
#define NUSOUNDDEEX_VERSION_NUM	_SYSINFRA_VERSION(NUSOUNDEX_MAJOR_NUM, NUSOUNDEX_MINOR_NUM, NUSOUNDEX_BUILD_NUM)

#define NUSOUNDEX_DECODE_SAMPLE_PER_FRAME	128		// counts per frame after decoding
#define NUSOUNDEX_DECODE_WORK_BUF_SIZE		(1140)	// bytes
#define NUSOUNDEX_DECODE_TEMP_BUF_SIZE		284		// bytes

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
UINT32 NuSoundEx_GetVersion(void);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuSoundEx decoder initiate.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUSOUNDEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuSoundEx decode internal information and can not be used with others.
//			This buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUSOUNDEX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuSoundEx decode temporary information and can be used with others.
//			This buffer address must be 4 bytes alignment.
//		u32StartAddr [in]
//			The address of the first NuSoundEx data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		 pfnReadDataCallback [in]
//			Function to read NuSoundEx data.
//
// Return Value
//		Sampling rate. If this value is 0, it represents decode failed in initiating.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 NuSoundEx_DecodeInitiate(
	UINT8 *pu8WorkBuf,
	UINT8 *pu8TempBuf,
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
	);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuSoundEx decoder decode data to PCM buffer. 
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUSOUNDEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuSoundEx decode internal information and can not be used with others.
//			This buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUSOUNDEX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuSoundEx decode temporary information and can be used with others.
//			This buffer address must be 4 bytes alignment.
//		pi16DecodedPcmBuf [out]
//			Buffer size is NUSOUNDEX_DECODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read NuSoundEx data.
//		pfnUserEventCallback [in]
//			Function to handle user event.
// Return Value
//		Decoded PCM count. If this value is 0, it represents no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 NuSoundEx_DecodeProcess(
	UINT8 *pu8WorkBuf,
	UINT8 *pu8TempBuf,
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT pfnUserEventCallback
	);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check NuSoundEx decode data finish or not.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUSOUNDEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuSoundEx decode internal information and can not be used with others.
//			This buffer address must be 4 bytes alignment.
//
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL NuSoundEx_DecodeIsEnd(
	UINT8 *pu8WorkBuf
	);

#ifdef  __cplusplus
}
#endif

#endif //#ifndef __NUSOUNDEX_H__


