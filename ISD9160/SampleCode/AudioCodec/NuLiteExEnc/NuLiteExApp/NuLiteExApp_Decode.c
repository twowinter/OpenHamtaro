#include <string.h>
#include "NuLiteExApp_Decode.h"
#include "AudioRom.h"
#include "PlaybackRecord.h"

extern S_AUDIO_CALLBACK g_asAppCallBack[];

//---------------------------------------------------------------------------------------------------------
void NuLiteExApp_DecodeInitiate(S_NULITEEX_APP_DECODE *psNuLiteExAppDecode, UINT8 *pau8TempBuf, UINT32 u32CallbackIndex)
{
	memset( psNuLiteExAppDecode, '\0', sizeof(S_NULITEEX_APP_DECODE) );
	// NuLiteEx decoder will refer to this index and call which callback function of storage 
	psNuLiteExAppDecode->u8CallbackIndex = (UINT8)u32CallbackIndex;
	// Input temp buffer to provide library used.
	psNuLiteExAppDecode->pau8TempBuf = pau8TempBuf;
	BUF_CTRL_SET_INACTIVE(&psNuLiteExAppDecode->sOutBufCtrl);
}

//---------------------------------------------------------------------------------------------------------
BOOL NuLiteExApp_DecodeStartPlayByID(S_NULITEEX_APP_DECODE *psNuLiteExAppDecode, UINT32 u32AudioID, UINT32 u32RomStartAddr, UINT8 u8PlaybackChannel)
{
	S_ROM_AUDIO_CHUNK_INFO sAudioChunkInfo;
	
	// Get audio chunk information for audio chunk start address from ROM file.
	AudioRom_GetAudioChunkInfo( g_asAppCallBack[psNuLiteExAppDecode->u8CallbackIndex].pfnReadDataCallback, 
								u32RomStartAddr, 
								u32AudioID, 
								&sAudioChunkInfo);	

	return NuLiteExApp_DecodeStartPlayByAddr(psNuLiteExAppDecode, sAudioChunkInfo.u32AudioChunkAddr, u8PlaybackChannel);
}

//---------------------------------------------------------------------------------------------------------
BOOL NuLiteExApp_DecodeStartPlayByAddr(S_NULITEEX_APP_DECODE *psNuLiteExAppDecode, UINT32 u32NuLiteExStorageStartAddr, UINT8 u8PlaybackChannel)
{
	UINT16 u16SampleRate;
	
	// NuLiteEx decoder initiates work buffer and returns sample rate.
	if ( (u16SampleRate = NuLiteEx_DecodeInitiate(	(UINT8*)psNuLiteExAppDecode->au32DecodeWorkBuf, 
													psNuLiteExAppDecode->pau8TempBuf, 
													u32NuLiteExStorageStartAddr, 
													g_asAppCallBack[psNuLiteExAppDecode->u8CallbackIndex].pfnReadDataCallback )) == 0 )
		return FALSE;	

	// Initiate and set output buffer variable(include frame size, buffer size etc.) 
	Playback_SetOutputBuf( 	&psNuLiteExAppDecode->sOutBufCtrl,
							NULITEEXAPP_OUT_BUF_SIZE,
							psNuLiteExAppDecode->i16OutBuf,
							NULITEEXAPP_OUT_SAMPLES_PER_FRAME,
							u16SampleRate );
	
	// Trigger active flag of output buffer for NuLiteEx decoding
	BUF_CTRL_SET_ACTIVE(&psNuLiteExAppDecode->sOutBufCtrl);

	// Pre-decode one frame
	psNuLiteExAppDecode->sOutBufCtrl.u16BufWriteIdx = NULITEEXAPP_OUT_SAMPLES_PER_FRAME;
	if ( NuLiteExApp_DecodeProcess(psNuLiteExAppDecode) == FALSE )
	{
		BUF_CTRL_SET_INACTIVE(&psNuLiteExAppDecode->sOutBufCtrl);
		return FALSE;
	}
	psNuLiteExAppDecode->sOutBufCtrl.u16BufReadIdx = NULITEEXAPP_OUT_SAMPLES_PER_FRAME;
	
	// Record play channel index for stopping to play.
	psNuLiteExAppDecode->u8PlaybackChannel = u8PlaybackChannel;
	// Add audio codec into channel and preper to play codec.
	Playback_Add(psNuLiteExAppDecode->u8PlaybackChannel, &psNuLiteExAppDecode->sOutBufCtrl);

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
void NuLiteExApp_DecodeStopPlay(S_NULITEEX_APP_DECODE *psNuLiteExAppDecode)
{
	// Clear active flag of output buffer for stoping NuLiteEx decode.
	BUF_CTRL_SET_INACTIVE(&psNuLiteExAppDecode->sOutBufCtrl);
	// Remove audio codec output buffer from play channel.
	Playback_Remove(psNuLiteExAppDecode->u8PlaybackChannel);
}

//---------------------------------------------------------------------------------------------------------
BOOL NuLiteExApp_DecodeProcess(S_NULITEEX_APP_DECODE *psNuLiteExAppDecode)
{
	INT16 *pi16OutBuf;
	
	if (BUF_CTRL_IS_INACTIVE(&psNuLiteExAppDecode->sOutBufCtrl))
		return FALSE;
	
	if ( Playback_NeedUpdateOutputBuf(&psNuLiteExAppDecode->sOutBufCtrl) )
	{
		// Check end of file
		if(NuLiteEx_DecodeIsEnd((UINT8*)psNuLiteExAppDecode->au32DecodeWorkBuf))
		{
			// Trigger inactive flag of output buffer to stop NuLiteEx decoding
			BUF_CTRL_SET_INACTIVE(&psNuLiteExAppDecode->sOutBufCtrl);
			// Use to represnt no active(or end) of decoding
			psNuLiteExAppDecode->sOutBufCtrl.u16SampleRate = 0;
			return FALSE;
		}

		// Record output data buffer pointer(for duplicate & process)
		pi16OutBuf = (PINT16)&psNuLiteExAppDecode->sOutBufCtrl.pi16Buf[psNuLiteExAppDecode->sOutBufCtrl.u16BufWriteIdx];
		
		NuLiteEx_DecodeProcess(	(UINT8*)psNuLiteExAppDecode->au32DecodeWorkBuf, 
								psNuLiteExAppDecode->pau8TempBuf, 
								pi16OutBuf, 
								g_asAppCallBack[psNuLiteExAppDecode->u8CallbackIndex].pfnReadDataCallback, 
								g_asAppCallBack[psNuLiteExAppDecode->u8CallbackIndex].pfnUserEventCallback);

		// Update write index of output buffer and avoid buffer overflow
		Playback_UpdateOutputBuf(&psNuLiteExAppDecode->sOutBufCtrl);

		// Duplicate data into buffer for using duplication callback function.
		if ( psNuLiteExAppDecode->u8CtrlFlag&(NULITEEXAPP_CTRL_DUPLICATE_TO_BUF|NULITEEXAPP_CTRL_DUPLICATE_TO_FUNC) )
		{
			if ( psNuLiteExAppDecode->u8CtrlFlag & NULITEEXAPP_CTRL_DUPLICATE_TO_BUF )
				BufCtrl_WriteWithCount(psNuLiteExAppDecode->psDuplicateOutBufCtrl, NULITEEXAPP_OUT_SAMPLES_PER_FRAME, pi16OutBuf );
			else 
				psNuLiteExAppDecode->pfnDuplicateFunc(NULITEEXAPP_OUT_SAMPLES_PER_FRAME, pi16OutBuf);
		}		
	}	
	return TRUE;
}
