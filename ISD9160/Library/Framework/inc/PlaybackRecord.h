/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __PLAYBACKRECORD_H__
#define __PLAYBACKRECORD_H__

#include "Platform.h"
#include "BufCtrl.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// Finesse of DB calibrate value
#define PLAYBACK_VOLUME_FINE_SHIFT	(10)

// Definition for "g_u32Playback_Volume_DB_Table" reference(Do not modify)
#define PLAYBACK_VOLUME_MUTE		(0)
#define PLAYBACK_VOLUME_NEG35_DB	(0.0178*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG30_DB	(0.0316*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG25_DB	(0.0562*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG20_DB	(0.1000*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG15_DB	(0.1778*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG10_DB	(0.3162*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG6_DB		(0.5012*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_NEG3_DB		(0.7079*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_0_DB		(1*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_3_DB		(1.4125*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_6_DB		(1.9953*(1<<PLAYBACK_VOLUME_FINE_SHIFT))
#define PLAYBACK_VOLUME_10_DB		(3.1623*(1<<PLAYBACK_VOLUME_FINE_SHIFT))

#define PLAYBACK_DECLARE_VOLUME_DB_TABLE()			\
uint32_t const g_u32Playback_Volume_DB_Table[] =  	\
{ 													\
	PLAYBACK_VOLUME_MUTE,							\
	PLAYBACK_VOLUME_NEG35_DB,						\
	PLAYBACK_VOLUME_NEG30_DB,						\
	PLAYBACK_VOLUME_NEG25_DB,						\
	PLAYBACK_VOLUME_NEG20_DB,						\
	PLAYBACK_VOLUME_NEG15_DB,						\
	PLAYBACK_VOLUME_NEG10_DB,						\
	PLAYBACK_VOLUME_NEG6_DB,						\
	PLAYBACK_VOLUME_NEG3_DB,						\
	PLAYBACK_VOLUME_0_DB,							\
	PLAYBACK_VOLUME_3_DB,							\
	PLAYBACK_VOLUME_6_DB,							\
	PLAYBACK_VOLUME_10_DB							\
};
	


// ---------------------------------------------------------------------------------------------------------
// Function: Playback_StartPlay
//
// Description:
//	Enable APU to play audio data
//
// Argument:
//	psDacBufCtrl:
//					the poiner o buffer control(S_BUF_CTRL*) which contain PCMs data for playback or
//					the pointer callback function(S_BUF_CTRL_CALLBACK*) which provides PCMs data for playback
//	u32SampleRate:	the playback sample rate
//
// Return:
//	TRUE:	sucessful
// ---------------------------------------------------------------------------------------------------------	
void Playback_StartPlay(void);

// ---------------------------------------------------------------------------------------------------------
// Function: Playback_StopPlay
//
// Description:
//	Stop APU to play audio data and wait for APU do stop.
//
// Argument:
//
// Return:
//
// ---------------------------------------------------------------------------------------------------------
void Playback_StopPlay(void);

void Record_StopRec(void);

void 
Record_SetInBufCallback(
	S_BUF_CTRL_CALLBACK *psAdcBufCtrl,
	PFN_DATA_REQUEST_CALLBACK pfnSetIntputData,
	void* pWorkBuf
);

void 
Record_SetInBufRecord(
	S_BUF_CTRL* psInBufCtrl,
	UINT16 u16BufSize,
	INT16* pi16Buf,
	UINT16 u16FrameSize,
	UINT16 u16SampleRate
);

// ---------------------------------------------------------------------------------------------------------
// Function: Playback_SetOutputBuf
//
// Description:
//	Set parameters to output buffer structure.
//
// Argument:
//	psOutBufCtrl: structure pointer of parameter for output buffer
//	u16BufSize: output buffer size.
//	pi16Buf: adddress pointer of output buffer.
//	u16FrameSize: frame size by processing
//	u16SampleRate: APU output sample rate
//
// Return:
//	None
// ---------------------------------------------------------------------------------------------------------
void
Playback_SetOutputBuf(
	S_BUF_CTRL* psOutBufCtrl,
	UINT16 u16BufSize,
	INT16* pi16Buf,
	UINT16 u16FrameSize,
	UINT16 u16SampleRate
);

// ---------------------------------------------------------------------------------------------------------
// Function: Playback_NeedUpdateOutputBuf
//
// Description:
//	According to free space to make decisions for output buffer updated .
//
// Argument:
//	psOutBufCtrl: structure pointer of parameter for output buffer.
//
// Return:
//	TRUE: free space  
//	FALSE: no space
// ---------------------------------------------------------------------------------------------------------
BOOL
Playback_NeedUpdateOutputBuf(
	S_BUF_CTRL* psOutBufCtrl
);

// ---------------------------------------------------------------------------------------------------------
// Function: Playback_UpdateOutputBuf
//
// Description:
//	Update write index of output buffer and avoid output buffer overflow.
//
// Argument:
//	psOutBufCtrl: structure pointer of parameter for output buffer
//
// Return:
//	None
// ---------------------------------------------------------------------------------------------------------
void
Playback_UpdateOutputBuf(
	S_BUF_CTRL* psOutBufCtrl
);

void 
Playback_Initiate(
	void
	);

void 
Playback_Add(
	UINT8 u8Channel,
	S_BUF_CTRL *psBufCtrl
);

void
Playback_Remove(
	UINT8 u8Channel
	);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Audio playback control to pause/resume one channel play.
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  bEnable : TRUE:Pause channel play, FALSE:Resume channel play. 
// ---------------------------------------------------------------------------------------------------------
void Playback_PauseCtrl(uint8_t u8Channel,BOOL bEnable);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Audio playback control to mute/unmute one channel play.
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  bEnable : TRUE:Mute channel play, FALSE:UnMute channel play. 
// ---------------------------------------------------------------------------------------------------------
void Playback_MuteCtrl(uint8_t u8Channel,BOOL bEnable);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	1. Audio playback control to set play volume DB value.
//  2. DB value is defined in file "PlaybackRecord.h".
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  u32DBValue : Config DB Value(Defined in "PlaybackRecord.h", ex. PLAYBACK_VOLUME_DB_MUTE, PLAYBACK_VOLUME_DB_NEG3)
// 
// Return:
//	TRUE : Success.
//  FALSE : DB value isn't in "g_u32AudioVolume_DB_Table" table.
// ---------------------------------------------------------------------------------------------------------
BOOL Playback_SetVolumeDB(uint8_t u8Channel,uint32_t u32DBValue);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	1. Audio playback control to adjust current DB step.
//  2. Adjust step is relative current DB index to increase(positive) or decrease(nagative). 
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  i8AdjustDBStep : Adjust DB step.(Positive:Loud,Nagative:Quiet)
//  u16StepIntervalBytes : Interval bytes between DB step.
// ---------------------------------------------------------------------------------------------------------
void Playback_AdjustVolumeDBStep(uint8_t u8Channel, int8_t i8AdjustDBStep, uint16_t u16StepIntervalBytes);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Config volume is equal to current volume.
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//
// Return:
//	TRUE : Is equal.
//  FALSE : Not equal.
// ---------------------------------------------------------------------------------------------------------
BOOL Playback_IsTargetVolume(uint8_t u8Channel);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Audio playback control to effect the "fade out volume".
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  u16StepIntervalBytes : Interval bytes between DB step.(smooth variable)
// ---------------------------------------------------------------------------------------------------------
void Playback_VolumeFadeOut(uint8_t u8ChannelIndex,uint16_t u16StepIntervalBytes);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Audio playback control to effect the "fade in volume".
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  u16StepIntervalBytes : Interval bytes between DB step.(smooth variable)
// ---------------------------------------------------------------------------------------------------------
void Playback_VolumeFadeIn(uint8_t u8ChannelIndex,uint16_t u16StepIntervalBytes);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Process one PCM byte data. 
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
//  
// ---------------------------------------------------------------------------------------------------------
void Playback_ProcessVolume(uint8_t u8Channel,int16_t* pi16Data);

// ---------------------------------------------------------------------------------------------------------
// Description:
//	Audio playback control to reset one channel volume to default DB index value.
//  (Note. This API is needed to config "PLAYBACK_VOLUME_CONTROL = 1" in "ConfigApp.h")
//
// Argument:
//	u8Channel : Control channel.
// ---------------------------------------------------------------------------------------------------------
void Playback_ResetChannelVolume(uint8_t u8Channel);

void Record_Add(
	S_BUF_CTRL *psAdcBufCtrl,
	UINT32 u32SampleRate
);

void Record_StartRec(void);

#ifdef  __cplusplus
}
#endif

#endif //#ifndef __PLAYBACKRECORD_H__
