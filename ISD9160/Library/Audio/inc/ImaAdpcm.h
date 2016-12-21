/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __IMAADPCM_H__
#define __IMAADPCM_H__

#include "Platform.h"
#include "SysInfra.h"
#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Version Definition
#define IMAADPCM_MAJOR_NUM   5
#define IMAADPCM_MINOR_NUM   00
#define IMAADPCM_BUILD_NUM   1
#define IMAADPCM_VERSION_NUM   _SYSINFRA_VERSION(IMAADPCM_MAJOR_NUM, IMAADPCM_MINOR_NUM, IMAADPCM_BUILD_NUM)

// Constant Definition
#define IMAADPCM_ENCODE_SAMPLE_PER_BLOCK 505		// sample count per block
#define IMAADPCM_ENCODE_BYTE_PER_BLOCK 256			// encoded data bytes per block

#define IMAADPCM_ENCODE_WORK_BUF_SIZE		(10*2)	// bytes
#define IMAADPCM_ENCODE_TEMP_BUF_SIZE		(15*2)	// bytes

#define IMAADPCM_ENCODE_LOW_BIT 0					// encode low half byte
#define IMAADPCM_ENCODE_HIGH_BIT 1					// encode high half byte

#define IMAADPCM_DECODE_SAMPLE_PER_BLOCK 505		// sample count per block
#define IMAADPCM_DECODE_BYTE_PER_BLOCK 256			// encoded data bytes per block

#define IMAADPCM_DECODE_WORK_BUF_SIZE		(27*2)	// bytes
#define IMAADPCM_DECODE_TEMP_BUF_SIZE		(14*2)	// bytes

#define IMAADPCM_DECODE_LOW_BIT 0					// decode low half byte
#define IMAADPCM_DECODE_HIGH_BIT 1					// decode high half byte

#define IMAADPCM_BYTE_PER_ALIGN_UNIT 4
#define IMAADPCM_SAMPLE_PER_ALIGN_UNIT 8

#define IMAADPCM_SAMPLE_PER_FRAME	32		// default sample count per frame

#define IMAADPCM_ENCODE_BYTE_PER_FRAME(Bpf)	 IMAADPCM_BYTE_PER_ALIGN_UNIT*((Bpf)/IMAADPCM_SAMPLE_PER_ALIGN_UNIT+2) // size of data buffer data for codec

#define IMAADPCM_ENCODE_BIT_PER_FRAME(bpf)	((IMAADPCM_ENCODE_BYTE_PER_FRAME(bpf))*8)

enum eIMAADPCMEncodespf
{
	E_IMAADPCM_SPF_04 = 4, 		//  4 sample per frame
	E_IMAADPCM_SPF_08 = 8, 		//  8 sample per frame
	E_IMAADPCM_SPF_12 = 12, 		// 12 sample per frame
	E_IMAADPCM_SPF_16 = 16, 		// 16 sample per frame
	E_IMAADPCM_SPF_20 = 20, 		// 20 sample per frame
	E_IMAADPCM_SPF_24 = 24, 		// 24 sample per frame
	E_IMAADPCM_SPF_28 = 28, 		// 28 sample per frame
	E_IMAADPCM_SPF_32 = 32 		// 32 sample per frame
};

enum eIMAADPCMEncodeDataSize
{
	E_IMAADPCM_SPF_04_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(4),		// bytes of data buffer while  4 sample per frame
	E_IMAADPCM_SPF_08_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(8),		// bytes of data buffer while  8 sample per frame
	E_IMAADPCM_SPF_12_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(12),	// bytes of data buffer while 12 sample per frame
	E_IMAADPCM_SPF_16_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(16),	// bytes of data buffer while 16 sample per frame
	E_IMAADPCM_SPF_20_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(20),	// bytes of data buffer while 20 sample per frame
	E_IMAADPCM_SPF_24_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(24),	// bytes of data buffer while 24 sample per frame
	E_IMAADPCM_SPF_28_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(28),	// bytes of data buffer while 28 sample per frame
	E_IMAADPCM_SPF_32_DATASIZE = IMAADPCM_ENCODE_BYTE_PER_FRAME(32),	// bytes of data buffer while 32 sample per frame
};

enum eImaAdpcmEncodeBPS
{
	E_IMAADPCM_ENCODE_BPS_04		//  4 bits data per sample
};

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		ImaAdpcm encoder initiate.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is IMAADPCM_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is IMAADPCM_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pAudioChunkHeader [in]
//			Audio Chunk header, it is defined in AudioCommon.h.
//		eBpsID [in]
//			ID of bit per frame after encoding, it is defined in eImaAdpcmEncodeBPS enumeration.
//		u16SampleRate [in]
//			Audio sampling rate.
//
// Return Value
//		None
// -----------------------------------------------------------------------------------------------------------------------
void ImaAdpcm_EncodeInitiate(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	S_AUDIOCHUNK_HEADER *psAudioChunkHeader,
	enum eImaAdpcmEncodeBPS eBpsID, 
	UINT16 u16SampleRate
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Set ImaAdpcm encode samples per frame.
//
// Parameter
//		pu8EncodeWorkBuf	[in]
//			Buffer size is IMAADPCM_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//
//		i16SampleCount [in]
//			It can be E_IMAADPCM_SPF_XX.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//
// Return Value
//		None
// -----------------------------------------------------------------------------------------------------------------------
void ImaAdpcm_EncodeSampleCount(
	UINT8 *pu8EncodeWorkBuf,
	INT16 i16SampleCount
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		ImaAdpcm encode pcm frame data to pi8EncodeDataBuf, programmer make sure encoded buffer size is consistent with sample per frame selection.
//		Function will return non-zero encoded size, unit is byte, if encode successfully.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is IMAADPCM_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is IMAADPCM_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi16PcmBuf [in]
//			Buffer size is E_IMAADPCM_SPF_XX*2 bytes.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//			This buffer is used to keep PCM data for encoding
//		pi8EncodeDataBuf [out]
//			Buffer size is IMAADPCM_ENCODE_BYTE_PER_FRAME(E_IMAADPCM_SPF_XX) bytes.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//			This buffer is used to keep encoded data read from storage.
//
// Return Value
//		The length of encoded data.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 ImaAdpcm_EncodeProcess(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	PINT16 pi16PcmBuf,
	PINT8 pi8EncodeDataBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		This function will add pcm data 0 to match ImaAdpcm block condition and then encode the added pcm data.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is IMAADPCM_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is IMAADPCM_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi8EncodeDataBuf [out]
//			Buffer size is IMAADPCM_ENCODE_BYTE_PER_FRAME(E_IMAADPCM_SPF_XX) bytes.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//			This buffer is used to keep encoded data read from storage.
//
// Return Value
//		The length of encoded data.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 ImaAdpcm_EncodeFlush(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	PINT8 pi8EncodeDataBuf
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		This function will update encoded data length, u32EncodeDataSize, into pAudioChunkHeader data structure after encoding.
//		Programmer needs to write this data structure to first 12 bytes in storage.
//
// Parameter
//		pu8EncodeWorkBuf [in]
//			Buffer size is IMAADPCM_ENCODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be shared with others.
//			The buffer address must be 4 bytes alignment.
//		pu8EncodeTempBuf [in]
//			Buffer size is IMAADPCM_ENCODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pAudioChunkHeader [out]
//			Audio Chunk header will be updated with correct encoded data size.
//		u32EncodeDataSize [in]
//			Correct data size after finish encoding.
// Return Value
//		None.
// -----------------------------------------------------------------------------------------------------------------------
void ImaAdpcm_EncodeEnd(
	UINT8 *pu8EncodeWorkBuf,
	UINT8 *pu8EncodeTempBuf,
	S_AUDIOCHUNK_HEADER *pAudioChunkHeader,
	UINT32 u32EncodeDataSize
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		ImaAdpcm decoder initiate.
//
// Parameter
//		pu8DecodeWorkBuf	[in]
//			Buffer size is IMAADPCM_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8DecodeTempBuf [in]
//			Buffer size is IMAADPCM_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		u32StartAddr [in]
//			The start adddress of the ImaAdpcm data.
//			For SPI flash, it is the SPI address.
//			For file with file system, it is the offset from a file.
//		pfnReadDataCallback [in]
//			Function to read ImaAdpcm data.
//
// Return Value
//		Sampling rate. If this value is 0, it represents decode failed in initiating.
// -----------------------------------------------------------------------------------------------------------------------
UINT32 ImaAdpcm_DecodeInitiate(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Set ImaAdpcm decode samples per frame.
//
// Parameter
//		pu8DecodeWorkBuf	[in]
//			Buffer size is IMAADPCM_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		i16SampleCount [in]
//			It can be E_IMAADPCM_SPF_XX.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//
// Return Value
//		None
// -----------------------------------------------------------------------------------------------------------------------
void ImaAdpcm_DecodeSampleCount(
	UINT8 *pu8EncodeWorkBuf,
	INT16 i16SampleCount
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		ImaAdpcm decode data to PCM buffer. Programmer needs to make sure pi16DecodedPcmBuf buffer has enough space to save decoded data,
//		or decoded buffer will be overflow.
//
// Parameter
//		pu8DecodeWorkBuf	[in]
//			Buffer size is IMAADPCM_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//		pu8DecodeTempBuf [in]
//			Buffer size is IMAADPCM_DECODE_TEMP_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode temporary information and can be used with others.
//			The buffer address must be 4 bytes alignment.
//		pi16DecodedPcmBuf [out]
//			Buffer size is E_IMAADPCM_SPF_XX*2 bytes.
//			XX : see enum eIMAADPCMEncodespf in ImaAdpcm.h
//			This buffer is used to keep decoded PCM data.
//		pfnReadDataCallback [in]
//			Function to read ImaAdpcm data.
//		pfnUserEventCallback [in]
//			The Imaadpcm library does not support user event and its value must be NULL.
//
// Return Value
//		Decoded PCM count. If this value is 0, it represent no PCM decoded.
// -----------------------------------------------------------------------------------------------------------------------
INT32 ImaAdpcm_DecodeProcess(
	UINT8 *pu8DecodeWorkBuf,
	UINT8 *pu8DecodeTempBuf,
	PINT16 pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_USREVENT pfnUserEventCallback
);

// -----------------------------------------------------------------------------------------------------------------------
// Description
//		Check ImaAdpcm decode data finish or not.
//
// Parameter
//		pu8DecodeWorkBuf [in]
//			Buffer size is IMAADPCM_DECODE_WORK_BUF_SIZE bytes.
//			This buffer is used to keep ImaAdpcm decode internal information and can not be used with others.
//			The buffer address must be 4 bytes alignment.
//
// Return Value
//		TRUE: Decode process is end. 
//		FALSE: Decode process is not end.
// -----------------------------------------------------------------------------------------------------------------------
BOOL ImaAdpcm_DecodeIsEnd(
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
UINT32 ImaAdpcm_GetVersion(void);

#ifdef  __cplusplus
}
#endif

#endif
