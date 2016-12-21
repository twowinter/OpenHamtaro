#ifndef _CONFIGNULITEEXAPP_ENCODE_H_
#define _CONFIGNULITEEXAPP_ENCODE_H_

#include "ConfigApp.h"
#include "NuLiteEx.h"
#include "BufCtrl.h"

// -------------------------------------------------------------------------------------------------------------------------------
// PCM Ring Buffer Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define NULITEEXAPP_IN_FRAME_NUM	  		2		// it can be : 2, 3, 4, 5, ....
#define NULITEEXAPP_IN_SAMPLES_PER_FRAME	NULITEEX_ENCODE_SAMPLE_PER_FRAME
#define NULITEEXAPP_IN_BUF_SIZE 			(NULITEEXAPP_IN_FRAME_NUM*NULITEEXAPP_IN_SAMPLES_PER_FRAME)
 							
#if ( NULITEEXAPP_ENCODE_ADC_BUF_SIZE%8 )
	#error "NULITEEXAPP_ENCODE_ADC_BUF_SIZE must be multiple of '8'."	
#endif						

#define NULITEEXAPP_ENCODE_MAX_BITRATE		E_NULITEEX_ENCODE_BPS_20			// NuLite bit rate selection
#define NULITEEXAPP_ENCODE_BUF_COUNT		2									// must >= 2
#define NULITEEXAPP_ENCODE_BUF_TOTAL_SIZE	((NULITEEXAPP_ENCODE_MAX_BITRATE>>3)*NULITEEXAPP_ENCODE_BUF_COUNT)
 							
#if ( NULITEEXAPP_ENCODE_BUF_TOTAL_SIZE%8 )
	#error "NULITEEXAPP_ENCODE_BUF_TOTAL_SIZE must be multiple of '8'."	
#endif	

typedef struct
{
	// Work buffer for NuLiteEx library to keep private data during decoding.
	// (NULITEEX_ENCODE_WORK_BUF_SIZE+3)/4 : Force to do 4 byte alignment
	UINT32 au32WorkBuf[(NULITEEX_ENCODE_WORK_BUF_SIZE+3)/4];
	
	// Frame buffer for keeping encoded data after NuLiteEx encoding 
	// (NULITEEX_ENCODE_WORK_BUF_SIZE+3)/4 : Force to do 4 byte alignment
	UINT32 au32EncodeBuf[(NULITEEXAPP_ENCODE_BUF_TOTAL_SIZE+3)/4];
	
	// Input buffer to save PCM samples.
	INT16 i16InBuf[NULITEEXAPP_IN_BUF_SIZE];
	
	// Pointer of temporary buffer array.
	// Temporary buffer is provided for NuLiteEx encode library.
	UINT8 *pau8TempBuf;
	
	// The encoded buffer control structure provides these operations:
	// 1. stores encoded data.
	// 2. the read index which represents the first encoded data in the ring buffer
	// 3. the write index which represents the first free space in the ring buffer
	// 4. the frame size which represents the count of encoded data at each time
	S_BUF_CTRL	sEncodeBufCtrl;
	
	// The PCM input buffer control structure provides these operations:
	// 1. stores PCM data.
	// 2. the read index which represents the first PCM data for encoding in the ring buffer
	// 3. the write index which represents the first free space in the ring buffer
	// 4. the frame size which represents the count of input PCM at each time
	S_BUF_CTRL	sInBufCtrl;
		
} S_NULITEEX_APP_ENCODE;

//---------------------------------------------------------------------------------------------------------
//	Description
//		Initiate buffer controlling for NuLiteEx encode application
//
//	Parameter
//  	psNuLiteExAppEncode[in] :
//			Pointer of NuLiteEx encode application handler.
//  	pu8EncodeTempBuf[in]
//É‹		Temporary buffer for NuXXX series codec application.
//É‹		Temporary buffer could be re-used by user after encoding one frame.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void NuLiteExApp_EncodeInitiate(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode, UINT8 *pu8EncodeTempBuf);

//---------------------------------------------------------------------------------------------------------
//	Description:                                                                                           
//		Initiate NuLiteEx encode required variables and header information.
//		
//		Due to this function does not enable ADC to record input PCMs.
//		Must call Record_StartRec() to start ADC recording if necessary!	
//
// 	Argument:
//		psNuLiteExAppEncode[in] :
//			Pointer of NuLiteEx encode application handler.
//		psAudioChunkHeader[in] :
//			Structure pointer of audio chunk header for recording information. 
//			Encoded information:
//			enocded format, sample rate, encoded total size and bit rate.
// 		u16SampleRate[in] :
//			Sample rate of input data to provide NuLiteEx encoder.
// 		eBitPerSample[in] :
//			Bit per sample of input data to provide NuLiteEx encoder.
//
//	Return:
//		FALSE : 
//			Bit rate is larger than NULITEEXAPP_ENCODE_MAX_BITRATE or sample rate is zero.
//		TRUE :
//			Initiate NuLiteEx encoder successfully.
//---------------------------------------------------------------------------------------------------------
BOOL NuLiteExApp_EncodeStart(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode, S_AUDIOCHUNK_HEADER *psAudioChunkHeader, UINT16 u16SampleRate, enum eNuLiteExEncodeBPS eBitPerSample);

//---------------------------------------------------------------------------------------------------------
// 	Description:     
//		Encode data from input buffer and put encode data into encoded buffer.
//		Can check the function return value to know it is running out of audio data or encoding stopped. 
//
//		Due to this function does not write any encoded data to storage.
//		Must write storage function to write encoded data to storage.
//
// 	Argument:
//		psNuLiteExAppDecode[in] :
//			Pointer of NuLiteEx encode application handler.
//
// 	Return:
// 		FALSE : 
//			Active flag of input buffer for ADC is not triggered.
//		TRUE :  
//			Encoding is going on.
//---------------------------------------------------------------------------------------------------------
BOOL NuLiteExApp_EncodeProcess(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode);

//---------------------------------------------------------------------------------------------------------
// 	Description:     
//		Stop to encode PCM data.	
//
// 	Argument:
//		psNuLiteExAppDecode[in] :
//			Pointer of NuLiteEx encode application handler.
//
// 	Return:
// 		None.
//---------------------------------------------------------------------------------------------------------
void NuLiteExApp_EncodeEnd(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode);

#endif
