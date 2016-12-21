/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/
#ifndef _AUDIOCOMMON_H_
#define _AUDIOCOMMON_H_

/******************************************************************************************
	.ROM File Layout
	+------------------------------+
	| S_ROM_HEADER                 |
	+------------------------------+ <------------------------------------------ S_ROM_HEADER.u32AudioStartAddr
	| S_ROM_AUDIO_CHUNK_INFO       | -+
	+------------------------------+  |
	| S_ROM_AUDIO_CHUNK_INFO       |  +-> Audio Chunck Infomation Table: size = S_ROM_HEADER.u32TotalAudioNum*sizeof(S_ROM_AUDIO_CHUNK_INFO)
	+------------------------------+  |   
	| ...                          | -+
	+------------------------------+ <------------------------------------------ Address: sizeof(S_ROM_HEADER)+ S_ROM_HEADER.u32TotalAudioNum*sizeof(S_ROM_AUDIO_CHUNK_INFO)
	| S_ROM_SENTENCE_LIST_ADDR     | -+
	+------------------------------+  |
	| S_ROM_SENTENCE_LIST_ADDR     |  |
	+------------------------------+  +-> Sentence List Information Table
	| S_ROM_SENTENCE_LIST_ADDR     |  |
	+------------------------------+  |
	| ...                          | -+
	+------------------------------+ <------------------------------------------ Address: S_ROM_HEADER.u32SentenceStartAddr
	| S_ROM_SENTENCE_INFO          | -+
	+------------------------------+  |
	| S_ROM_SENTENCE_INFO          |  |
	+------------------------------+  +-> Sentence Infomation Table
	| 0xffff(End of Sentence List) |  |
	+------------------------------+  |
	| S_ROM_SENTENCE_INFO          |  |
	+------------------------------+  |
	| 0xffff(End of Sentence List) |  |
	+------------------------------+  |
	| S_ROM_SENTENCE_INFO          |  |
	+------------------------------+  |
	| ...                          |  |
	+------------------------------+  |
	| 0xffff(End of Sentence List) | -+
	+------------------------------+ 
	| Audio Chunck                 |
	+------------------------------+
	| Audio Chunck                 |
	+------------------------------+
	| ...                          |
	+------------------------------+
	| Audio Chunck                 |
	+------------------------------+
 ******************************************************************************************/

/******************************************************************************************
	Audio Chunck Layout
	(Also Audio File Layout)
	+-----------------------------+
	| S_AUDIO_CHUNK_HEADER        |
	+-----------------------------+ -+
	|                             |  |
	|                             |  |
	|                             |  |
	|    Encoded Data             |  +-> size = S_AUDIO_CHUNK_HEADER.u32TotalSize
	|                             |  |
	|                             |  |
	|                             |  |
	|                             |  |
	+-----------------------------+ -+
 ******************************************************************************************/

/******************************************************************************************
 * Audio data format definitions
 ******************************************************************************************/
//#define AUDIO_FMT_NUONE  		(0x0030)		// old NuOne
//#define AUDIO_FMT_NUONE_E		(0x0031)		// old NuOne with event
#define AUDIO_FMT_PCM16 		(0x0005)		// P16 with event
#define AUDIO_FMT_MDPCM4 		(0x0007)		// MDM4 with event
#define AUDIO_FMT_SILENCE		(0x000B)		// Silence
#define AUDIO_FMT_LP8			(0x000E)		// LP8 with event
#define AUDIO_FMT_NUSOUND		(0x002F)		// NuSound with event
#define AUDIO_FMT_NUONE			(0x0040)		// NuOne without event
#define AUDIO_FMT_NUONE_E 		(0x0041) 		// NuOne with event
#define AUDIO_FMT_NULITE		(0x0050)		// NuLite without event
#define AUDIO_FMT_NULITE_E		(0x0051)		// NuLite with event
#define AUDIO_FMT_NUVOX			(0x0060)		// NuVox without event
#define AUDIO_FMT_NUVOX_E		(0x0061)		// NuVox with event
#define AUDIO_FMT_NUVOX53		(0x0070) 		// NuVox53 without event
#define AUDIO_FMT_IMFMIDI		(0x0800)		// IMF-MIDI with event
#define AUDIO_FMT_IMAADPCM		(0x0080)                // IMA ADPCM without event
#define AUDIO_FMT_IMAADPCM_E	(0x0081)                // IMA ADPCM with event

/******************************************************************************************
 * Description:
 * 	To describe each auido chunk header, except midi.
 ******************************************************************************************/ 
typedef struct
{  
	UINT16	u16FormatType;						// Audio data format. 
	UINT16	u16SmplRate;						// Audio sampling rate.	
	UINT32	u32TotalSize;						// Audio total size not including this structure; unit is byte
	union
	{
		UINT32	u32LoopAddr; 					// Store loop address if decode find loop event; it's unit is byte. 
		UINT32	u32BitPerFrame;						// NuXXX codec bit rate setting information.
	};
}S_AUDIOCHUNK_HEADER;
#define AUDIOCHUNK_HEADER_SIZE			sizeof(S_AUDIOCHUNK_HEADER)

typedef struct
{
	UINT32 	u32TotalAudioNum;
	UINT32 	u32TotalSentenceNum;
	UINT32 	u32AudioStartAddr;
	UINT32 	u32SentenceStartAddr;
}S_ROM_HEADER;
#define ROM_HEADER_SIZE				sizeof(S_ROM_HEADER)

typedef struct
{
	UINT32	u32AudioChunkAddr;
	UINT32	u32AudioChunkSize;
}S_ROM_AUDIO_CHUNK_INFO;
#define ROM_AUDIO_CHINK_INFO_SIZE	sizeof(S_ROM_AUDIO_CHUNK_INFO)

typedef struct
{
	UINT16	u16FormatType;						// Audio data format. 
	UINT16	u16SmplRate;						// Audio sampling rate.	
	UINT32	u32AudioChunkAddr;					// Audio trunck start address
	union
	{
		UINT32	u32AudioChunkSize;				// Audio trunck size   if u16FromatType != AUDIO_FMT_SILENCE 
		UINT32	u32SilenceSampleCount;			// Silence sample count if u16FromatType == AUDIO_FMT_SILENCE
	};
	UINT16	u16Volume;
	UINT16	u16Repeat;
	UINT8	u8Channel;
}S_ROM_SENTENCE_INFO;
#define ROM_SENTENCE_INFO_SIZE				17
#define ROM_SENTENCE_INFO_FORMAT_END		0xffff
#define ROM_SENTENCE_INFO_FORMAT_FIELD_SIZE	2

#define AUDIO_GET_STORAGE_INDEX(ID)	(((ID)&0xc00)>>12)
#define AUDIO_GET_ID(ID)			((ID)&0x3ff)

typedef struct
{
	UINT32	u32StartAddr;
}S_ROM_SENTENCE_LIST_ADDR;

typedef UINT32 (*PFN_AUDIO_DATAREQUEST)(void * pDesAddr, UINT32 u32Position, UINT32 u32ByteNum);
typedef UINT32 (*PFN_AUDIO_USREVENT)(UINT16 u16EventIndex, UINT16 u16EventSubIndex);

typedef UINT32 (*PFN_AUDIO_DECODEINITIATE)( UINT8 *pu8DecodeWorkBuf, UINT8 *pu8DecodeTempBuf,UINT32 u32StartAddr,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback );
typedef INT32 (*PFN_AUDIO_DECODEPROCESS)( UINT8 *pu8DecodeWorkBuf, UINT8 *pu8DecodeTempBuf, INT16 *pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback, PFN_AUDIO_USREVENT PFN_AUDIO_USREVENT);

typedef BOOL (*PFN_AUDIO_DECODEISEND)(UINT8 *pu8DecodeWorkBuf);


// Storage reading, User event, Wave table callback 
typedef struct sAudioCallBack
{
	PFN_AUDIO_DATAREQUEST		pfnReadDataCallback;
	PFN_AUDIO_DATAREQUEST		pfnWriteDataCallback;
	PFN_AUDIO_USREVENT			pfnUserEventCallback;
	PFN_AUDIO_DATAREQUEST		pfnReadMidiWavTableCallback;
}S_AUDIO_CALLBACK;

#endif
