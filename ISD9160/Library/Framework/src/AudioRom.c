#include "PlatForm.h"
#include "AudioRom.h"

//---------------------------------------------------------------------------------------------------------
void AudioRom_GetRomHeader( 
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	UINT32 u32RomStartAddr,
	S_ROM_HEADER *psRomHeader )
{
	pfnReadDataCallback( (PUINT8)psRomHeader, u32RomStartAddr, ROM_HEADER_SIZE );
	psRomHeader->u32AudioStartAddr += u32RomStartAddr;
	psRomHeader->u32SentenceStartAddr += u32RomStartAddr;
}

//---------------------------------------------------------------------------------------------------------
UINT32 AudioRom_GetAudioNum( 
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	UINT32 u32RomStartAddr )
{
	UINT32 u32AudioNum;

	pfnReadDataCallback( (PUINT8)&u32AudioNum, u32RomStartAddr, sizeof(UINT32) );	

	return u32AudioNum;
}

//---------------------------------------------------------------------------------------------------------
void AudioRom_GetAudioChunkInfo(  
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	UINT32 u32RomStartAddr,
	UINT32 u32AudioIndex,
	S_ROM_AUDIO_CHUNK_INFO *psAudioChunkInfo )
{
	S_ROM_HEADER sRomHeader;

	AudioRom_GetRomHeader( pfnReadDataCallback, u32RomStartAddr, &sRomHeader );

	if( u32AudioIndex < sRomHeader.u32TotalAudioNum )
	{
		pfnReadDataCallback( (PUINT8)psAudioChunkInfo, sRomHeader.u32AudioStartAddr+u32AudioIndex*ROM_AUDIO_CHINK_INFO_SIZE, ROM_AUDIO_CHINK_INFO_SIZE );	
		psAudioChunkInfo->u32AudioChunkAddr += u32RomStartAddr;
	}

}

