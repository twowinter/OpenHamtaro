#include <string.h>
#include "NuLiteExApp_Encode.h"
#include "PlaybackRecord.h"

void NuLiteExApp_EncodeInitiate(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode, UINT8 *pu8EncodeTempBuf)
{
	memset( psNuLiteExAppEncode, '\0', sizeof(S_NULITEEX_APP_ENCODE) );
	
	// Initiate buffer controlling variables for encoding
	psNuLiteExAppEncode->sEncodeBufCtrl.pi16Buf = (INT16*)psNuLiteExAppEncode->au32EncodeBuf;
	psNuLiteExAppEncode->sEncodeBufCtrl.u16SampleRate = 0;
	psNuLiteExAppEncode->pau8TempBuf = pu8EncodeTempBuf;
	BUF_CTRL_SET_INACTIVE(&psNuLiteExAppEncode->sInBufCtrl);
}

BOOL NuLiteExApp_EncodeStart( S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode, S_AUDIOCHUNK_HEADER *psAudioChunkHeader, 
							  UINT16 u16SampleRate, enum eNuLiteExEncodeBPS eBitPerSample)
{
	if ( (eBitPerSample > NULITEEXAPP_ENCODE_MAX_BITRATE) || (u16SampleRate == 0) )
		return FALSE;
	else
	{
		psAudioChunkHeader->u16SmplRate = u16SampleRate;
		psAudioChunkHeader->u32BitPerFrame = eBitPerSample;
	}
	
	// NuLiteEx encoder initiates work buffer.
	// Set bit rate and sample rate information for audio chunk header.
	NuLiteEx_EncodeInitiate((UINT8 *)psNuLiteExAppEncode->au32WorkBuf, psNuLiteExAppEncode->pau8TempBuf, 
		psAudioChunkHeader, (enum eNuLiteExEncodeBPS)psAudioChunkHeader->u32BitPerFrame, psAudioChunkHeader->u16SmplRate);
			
	// Reset encode buffer read index and write index
	psNuLiteExAppEncode->sEncodeBufCtrl.u16BufWriteIdx = 0;
	psNuLiteExAppEncode->sEncodeBufCtrl.u16BufReadIdx = 0;
	
	// Set Encoded frame size, Storage Utility will refer to this size to write data.
	psNuLiteExAppEncode->sEncodeBufCtrl.u16FrameSize =  (psAudioChunkHeader->u32BitPerFrame)>>3;
	psNuLiteExAppEncode->sEncodeBufCtrl.u16BufCount = (psNuLiteExAppEncode->sEncodeBufCtrl.u16FrameSize)*NULITEEXAPP_ENCODE_BUF_COUNT;
	
	// Set input buffer size, PCM buffer pointer, frame size and sample rate.
	Record_SetInBufRecord(  &psNuLiteExAppEncode->sInBufCtrl, 
							NULITEEXAPP_IN_BUF_SIZE,
							psNuLiteExAppEncode->i16InBuf,
							NULITEEX_ENCODE_SAMPLE_PER_FRAME,
							psAudioChunkHeader->u16SmplRate);
					
	// Set application input buffer to record(ADC) output buffer.
	Record_Add(&psNuLiteExAppEncode->sInBufCtrl, psAudioChunkHeader->u16SmplRate);
	
	return TRUE;
}

BOOL NuLiteExApp_EncodeProcess(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode)
{
	UINT32 u32DataLength;
	S_BUF_CTRL *psEncodeBufCtrl, *psInBufCtrl;
	
	if (BUF_CTRL_IS_INACTIVE(&psNuLiteExAppEncode->sInBufCtrl))
		return FALSE;
	
	psEncodeBufCtrl = &psNuLiteExAppEncode->sEncodeBufCtrl;
	psInBufCtrl = &psNuLiteExAppEncode->sInBufCtrl;

	while(( psInBufCtrl->u16BufReadIdx > psInBufCtrl->u16BufWriteIdx )|| 
		(( psInBufCtrl->u16BufWriteIdx - psInBufCtrl->u16BufReadIdx)>= NULITEEX_ENCODE_SAMPLE_PER_FRAME))
	{
		// Process encoding and return encoded length
		u32DataLength = NuLiteEx_EncodeProcess((UINT8 *)psNuLiteExAppEncode->au32WorkBuf, psNuLiteExAppEncode->pau8TempBuf,
			psInBufCtrl->pi16Buf + psInBufCtrl->u16BufReadIdx,
			((INT8*)(psEncodeBufCtrl->pi16Buf)) + psEncodeBufCtrl->u16BufWriteIdx );
		
		// Update write index of encoded buffer
		if ((psEncodeBufCtrl->u16BufWriteIdx+=u32DataLength) >= psNuLiteExAppEncode->sEncodeBufCtrl.u16BufCount)
			psEncodeBufCtrl->u16BufWriteIdx = 0;
		
		// Update read index of ADC input buffer
		if ((psInBufCtrl->u16BufReadIdx+=NULITEEX_ENCODE_SAMPLE_PER_FRAME) >= NULITEEXAPP_IN_BUF_SIZE)
			psInBufCtrl->u16BufReadIdx = 0;
	}
	return TRUE;
}

void NuLiteExApp_EncodeEnd(S_NULITEEX_APP_ENCODE *psNuLiteExAppEncode)
{
	BUF_CTRL_SET_INACTIVE(&psNuLiteExAppEncode->sInBufCtrl);
}
