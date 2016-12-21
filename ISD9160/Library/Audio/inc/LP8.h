/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __LP8_H__
#define __LP8_H__

#include "Platform.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define	LP8_MAJOR_NUM 3
#define	LP8_MINOR_NUM 30
#define	LP8_BUILD_NUM 1
#define LP8_VERSION_NUM	_SYSINFRA_VERSION(LP8_MAJOR_NUM, LP8_MINOR_NUM, LP8_BUILD_NUM)

// Constant Definition
#define LP8_DECODE_RESIN_BUFF_SIZE		32		//bytes	
#define LP8_DECODE_SAMPLE_PER_FRAME		8		// counts per frame after decoding
#define LP8_DECODE_WORK_BUF_SIZE		(40+LP8_DECODE_RESIN_BUFF_SIZE)		// bytes
#define LP8_DECODE_TEMP_BUF_SIZE		0		// bytes

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		LP8 decoder initiate.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is LP8_DECODE_WORK_BUF_SIZE bytes.
//			The buffer address must be 4 bytes alignment.
//			This buffer is used to keep LP8 decode internal information and can not be used with others.
//		pu8TempBuf [in]
//			This buffer is reserved and its value must be NULL.
//		u32StartAddr [in]
//			The adddress of the first LP8 data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		pfnReadDataCallback [in]
//			Function to read LP8 data.
//
// Return Value
//		The sample rate of LP8 file.
//		If this value is 0, it represent decode failed in initiating.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 LP8_DecodeInitiate(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,	
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		LP8 decoder decode data to PCM buffer. 
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is LP8_DECODE_WORK_BUF_SIZE bytes.
//			The buffer address must be 4 bytes alignment.
//			This buffer is used to keep LP8 decode internal information and can not be used with others.
//		pu8TempBuf [in]
//			This buffer is reserved and its value must be NULL.
//		pi16DecodedPcmBuf [out]
//			Buffer size is LP8_DECODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read LP8 data.
//		pfnUserEventCallback [in]
//			Function to handle user event.
//
//	Return Value
//		Decoded PCM count.
//		If this value is 0, it represents no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 LP8_DecodeProcess(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT PFN_AUDIO_USREVENT
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check LP8 decode data finish or not.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is LP8_DECODE_WORK_BUF_SIZE bytes.
//			The buffer address must be 4 bytes alignment.
//			This buffer is used to keep LP8 decode internal information and can not be used with others.
//
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL LP8_DecodeIsEnd(
	UINT8 *pu8DecodeWorkBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Set decoding sample counts of frame, default is LP8_DECODE_SAMPLE_PER_FRAME. This function flow is called after LP8_DecodeInitiate() API.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is LP8_DECODE_WORK_BUF_SIZE bytes.
//			The buffer address must be 4 bytes alignment.
//			This buffer is used to keep LP8 decode internal information and can not be used with others.
//		u8SampleCounts [in]
//			Decoding sample counts of one frame.
//
// Return Value
//		None.
// -----------------------------------------------------------------------------------------------------------------------
void LP8_DecodeSampleCounts(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 u8SampleCounts
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
UINT32 LP8_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif //#ifndef __LP8_H__


