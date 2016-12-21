/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NUONEEX_H__
#define __NUONEEX_H__
   
#include "Platform.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define NUONEEX_MAJOR_NUM   3
#define NUONEEX_MINOR_NUM   30
#define NUONEEX_BUILD_NUM   1
//#define NUONEEX_VERSION_NUM   _SYSINFRA_VERSION(NUONEEX_MAJOR_NUM, NUONEEX_MINOR_NUM, NUONEEX_BUILD_NUM)

// Constant Definition
#define NUONEEX_DECODE_SAMPLE_PER_FRAME	     320 	// counts per frame after decoding
#define NUONEEX_DECODE_WORK_BUF_SIZE		(198*2+4+2) // bytes
#define NUONEEX_DECODE_TEMP_BUF_SIZE		(506*2)	// bytes

#define NUONEEX_ENCODE_SAMPLE_PER_FRAME      320	// counts of PCMs per frame before encodeing
#define NUONEEX_ENCODE_WORK_BUF_SIZE		(326*2+4)	// bytes
#define NUONEEX_ENCODE_TEMP_BUF_SIZE		(551*2)	// bytes
//#define NUONEEX_ENCODE_DATA_BUF_SIZE		120		// bytes


#define NUONEEX_ENCODE_BIT_PER_FRAME(bps)	((NUONEEX_ENCODE_SAMPLE_PER_FRAME*bps)/10)	// total bits of one encoded frame
#define NUONEEX_ENCODE_BYTE_PER_FRAME(bps)	 (NUONEEX_ENCODE_BIT_PER_FRAME(bps)>>3)	// total bytes of one encoded frame

enum eNuOneExEncodeBPS		// Bit Per Sample after encoding
{
	E_NUONEEX_ENCODE_BPS_05 = NUONEEX_ENCODE_BIT_PER_FRAME( 5),				// 0.5 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_06 = NUONEEX_ENCODE_BIT_PER_FRAME( 6),				// 0.6 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_07 = NUONEEX_ENCODE_BIT_PER_FRAME( 7),				// 0.7 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_08 = NUONEEX_ENCODE_BIT_PER_FRAME( 8),				// 0.8 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_09 = NUONEEX_ENCODE_BIT_PER_FRAME( 9),				// 0.9 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_10 = NUONEEX_ENCODE_BIT_PER_FRAME(10),				// 1.0 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_11 = NUONEEX_ENCODE_BIT_PER_FRAME(11),				// 1.1 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_12 = NUONEEX_ENCODE_BIT_PER_FRAME(12),				// 1.2 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_13 = NUONEEX_ENCODE_BIT_PER_FRAME(13),				// 1.3 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_14 = NUONEEX_ENCODE_BIT_PER_FRAME(14),				// 1.4 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_15 = NUONEEX_ENCODE_BIT_PER_FRAME(15),				// 1.5 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_16 = NUONEEX_ENCODE_BIT_PER_FRAME(16),				// 1.6 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_17 = NUONEEX_ENCODE_BIT_PER_FRAME(17),				// 1.7 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_18 = NUONEEX_ENCODE_BIT_PER_FRAME(18),				// 1.8 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_19 = NUONEEX_ENCODE_BIT_PER_FRAME(19),				// 1.9 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_20 = NUONEEX_ENCODE_BIT_PER_FRAME(20),				// 2.0 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_21 = NUONEEX_ENCODE_BIT_PER_FRAME(21),				// 2.1 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_22 = NUONEEX_ENCODE_BIT_PER_FRAME(22),				// 2.2 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_23 = NUONEEX_ENCODE_BIT_PER_FRAME(23),				// 2.3 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_24 = NUONEEX_ENCODE_BIT_PER_FRAME(24),				// 2.4 bit per sample after encoding
	E_NUONEEX_ENCODE_BPS_25 = NUONEEX_ENCODE_BIT_PER_FRAME(25),				// 2.5 bit per sample after encoding
};

enum eNuOneExEncodeDataSize	// The bytes of one encoded frame
{
	E_NUONEEX_ENCODE_BPS_05_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME( 5),	// The bytes of one encoded frame at 0.5 bit per sample
	E_NUONEEX_ENCODE_BPS_06_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME( 6),	// The bytes of one encoded frame at 0.6 bit per sample
	E_NUONEEX_ENCODE_BPS_07_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME( 7),	// The bytes of one encoded frame at 0.7 bit per sample
	E_NUONEEX_ENCODE_BPS_08_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME( 8),	// The bytes of one encoded frame at 0.8 bit per sample
	E_NUONEEX_ENCODE_BPS_09_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME( 9),	// The bytes of one encoded frame at 0.9 bit per sample
	E_NUONEEX_ENCODE_BPS_10_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(10),	// The bytes of one encoded frame at 1.0 bit per sample
	E_NUONEEX_ENCODE_BPS_11_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(11),	// The bytes of one encoded frame at 1.1 bit per sample
	E_NUONEEX_ENCODE_BPS_12_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(12),	// The bytes of one encoded frame at 1.2 bit per sample
	E_NUONEEX_ENCODE_BPS_13_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(13),	// The bytes of one encoded frame at 1.3 bit per sample
	E_NUONEEX_ENCODE_BPS_14_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(14),	// The bytes of one encoded frame at 1.4 bit per sample
	E_NUONEEX_ENCODE_BPS_15_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(15),	// The bytes of one encoded frame at 1.5 bit per sample
	E_NUONEEX_ENCODE_BPS_16_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(16),	// The bytes of one encoded frame at 1.6 bit per sample
	E_NUONEEX_ENCODE_BPS_17_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(17),	// The bytes of one encoded frame at 1.7 bit per sample
	E_NUONEEX_ENCODE_BPS_18_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(18),	// The bytes of one encoded frame at 1.8 bit per sample
	E_NUONEEX_ENCODE_BPS_19_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(19),	// The bytes of one encoded frame at 1.9 bit per sample
	E_NUONEEX_ENCODE_BPS_20_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(20),	// The bytes of one encoded frame at 2.0 bit per sample
	E_NUONEEX_ENCODE_BPS_21_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(21),	// The bytes of one encoded frame at 2.1 bit per sample
	E_NUONEEX_ENCODE_BPS_22_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(22),	// The bytes of one encoded frame at 2.2 bit per sample
	E_NUONEEX_ENCODE_BPS_23_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(23),	// The bytes of one encoded frame at 2.3 bit per sample
	E_NUONEEX_ENCODE_BPS_24_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(24),	// The bytes of one encoded frame at 2.4 bit per sample
	E_NUONEEX_ENCODE_BPS_25_DATASIZE = NUONEEX_ENCODE_BYTE_PER_FRAME(25),	// The bytes of one encoded frame at 2.5 bit per sample
};


// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuOneEx decoder initiate.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUONEEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUONEEX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		u32StartAddr [in]
//			The adddress of the first NuOneEx data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		pfnReadDataCallback [in]
//			Function to read NuOneEx data.
//
// Return Value
//		Sampling rate. If this value is 0, it represents decode failed in initiating.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 NuOneEx_DecodeInitiate(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuOneEx decode data to PCM buffer. Programmer needs to make sure pi16DecodedPcmBuf buffer has enough space to save decoded data,
//		or decoded buffer will be overflow.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUONEEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8TempBuf [in]
//			Buffer size is NUONEEX_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi16DecodedPcmBuf [out]
//			Buffer size is NUONEEX_DECODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read NuOneEx data.
//		pfnUserEventCallback [in]
//			Function to handle user event.
//
// Return Value
//		Decoded PCM count. If this value is 0, it represent no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 NuOneEx_DecodeProcess(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT pfnUserEventCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check NuOneEx decode data finish or not.
//
// Parameter
//		pu8WorkBuf	[in]
//			Buffer size is NUONEEX_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL NuOneEx_DecodeIsEnd(
	UINT8 *pu8DecodeWorkBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuOneEx encoder initiate.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is NUONEEX_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be shared with others.
//		pu8EncodeTempBuf [in]
//			Buffer size is NUONEEX_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pPAudioChunkHeader [in]
//			Audio Chunk header, it is defined in AudioCommon.h.
//		eBpsID [in]
//			ID of bit per sample after encoding, it is defined in eNuOneExEncodeBPS enumeration.
//		u16SampleRate [in]
//			Audio sampling rate.
//
// Return Value
//		None.
// -----------------------------------------------------------------------------------------------------------------------
void NuOneEx_EncodeInitiate(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	S_AUDIOCHUNK_HEADER *pAudioChunkHeader,
	enum eNuOneExEncodeBPS eBpsID,
	UINT16 u16SampleRate
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		NuOneEx encode frame data to pi8EncodeDataBuf. Programmer make sure encoded buffer size is consistent with bit rate selection.
//		Function will return non-zero encoded size, unit is byte, if encode successfully.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is NUONEEX_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is NUONEEX_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi16PcmBuf [out]
//			Buffer size is NUONEEX_ENCODE_SAMPLE_PER_FRAME*2 bytes.
//			This buffer is used to keep PCM data for encoding
//		pi8EncodeDataBuf [in]
//			Buffer size is E_NUONEEX_ENCODE_BPS_xx_DATASIZE bytes, where xx is the bit rate for encoding.
//			This buffer is used to keep encoded data read from storage.
//
// Return Value
//		The length of encoded data.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 NuOneEx_EncodeProcess(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	PINT16 pi16PcmBuf,
	PINT8  pi8EncodeDataBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		This function will update encoded data length, u32EncodeDataSize, into pAudioChunkHeader data structure after encoding.
//		Programmer needs to write this data structure to first 12 bytes in storage.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is NUONEEX_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is NUONEEX_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep NuOneEx decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pAudioChunkHeader [out]
//			Audio Chunk header will be updated with correct encoded data size.
//		u32EncodeDataSize [in]
//			Correct data size after finish encoding.
// Return Value
//		None.
// -----------------------------------------------------------------------------------------------------------------------
void NuOneEx_EncodeEnd(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	S_AUDIOCHUNK_HEADER *pAudioChunkHeader,	
	UINT32 u32EncodeDataSize
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
UINT32 NuOneEx_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif	//#ifndef __NUONEEX_H__
