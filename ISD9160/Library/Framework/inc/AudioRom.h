/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                            			       */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __AUDIOROM_H__
#define __AUDIOROM_H__

#include "AudioCommon.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/************************************************************************************************************
 * Description:
 * Argument:
 *	1 psSpiFlashHandler:
 *	2 u32RomStartAddr
 *	3 psRomHeader
 ************************************************************************************************************/
void AudioRom_GetRomHeader( PFN_AUDIO_DATAREQUEST pfnReadDataCallback, UINT32 u32RomStartAddr, S_ROM_HEADER *psRomHeader );

/************************************************************************************************************
 * Description:
 * Argument:
 *	1 psSpiFlashHandler:
 *	2 u32RomStartAddr
 ************************************************************************************************************/
UINT32 AudioRom_GetAudioNum( PFN_AUDIO_DATAREQUEST pfnReadDataCallback, UINT32 u32RomStartAddr );

/************************************************************************************************************
 * Description:
 * Argument:
 *	1 psSpiFlashHandler:
 *	2 u32RomStartAddr
 *  3 u32AudioIndex
 *  4 psAudioChunkInfo
 ************************************************************************************************************/
void AudioRom_GetAudioChunkInfo( PFN_AUDIO_DATAREQUEST pfnReadDataCallback, UINT32 u32RomStartAddr, UINT32 u32AudioIndex, S_ROM_AUDIO_CHUNK_INFO *psAudioChunkInfo );

#ifdef  __cplusplus
}
#endif

#endif
