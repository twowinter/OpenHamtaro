/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __P16_H__
#define __P16_H__

#include "Platform.h"
//#include "System/SysInfra.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define	P16_MAJOR_NUM 3
#define	P16_MINOR_NUM 30
#define	P16_BUILD_NUM 1
//#define P16_VERSION_NUM	_SYSINFRA_VERSION(P16_MAJOR_NUM, P16_MINOR_NUM, P16_BUILD_NUM)

// Constant Definition 
#define P16_DECODE_RESIN_BUF_SIZE		16

#define P16_DECODE_SAMPLE_PER_FRAME		8								// counts per frame after decoding
#define P16_DECODE_WORK_BUF_SIZE		(16+P16_DECODE_RESIN_BUF_SIZE)	// bytes
#define P16_DECODE_TEMP_BUF_SIZE		0								// bytes

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		P16 decoder initiate.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is P16_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep P16 decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			This buffer is reserved and its value must be NULL.
//		u32StartAddr [in]
//			The address of the first P16 data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		pfnReadDataCallback [in]
//			Function to read P16 data.
//
// Return Value
//		Sampling rate. If this value is 0, it represents decode failed in initiating.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 P16_DecodeInitiate(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		P16 decoder decode data to PCM buffer. 
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is P16_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep P16 decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			This buffer is reserved and its value must be NULL.
//		pi16DecodedPcmBuf [out]
//			Buffer size is P16_DECODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read P16 data.
//		pfnUserEventCallback [in]
//			Function to handle user event.
//
// Return Value
//		Decoded PCM count. If this value is 0, it represents no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 P16_DecodeProcess(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,					
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT pfnUserEventCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check P16 decode data finish or not.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is P16_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep P16 decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL P16_DecodeIsEnd(
	UINT8 *pu8DecodeWorkBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Set decoding sample counts of frame, default is P16_DECODE_SAMPLE_PER_FRAME. This function flow is called after P16_DecodeInitiate() API.
//
// Parameter
//		pu8WorkBuf [in]
//			Buffer size is P16_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep P16 decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		u8SampleCounts [in]
//			Decoding sample counts of one frame.
//
// Return Value
//		None.
// -----------------------------------------------------------------------------------------------------------------------
void P16_DecodeSampleCounts(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 u8DecodeSampleCounts
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
UINT32 P16_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif //#ifndef __P16_H__


