/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/	 

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- APU ISR to handle:
//			* Get PCM data from:
//				a. Ring buffer.
//				b. Callback for special usage(example: call audio mixer function).
//			* Up sampling(1x, 2x, 4x) to reduce "metal sound".
//			* Clipping at PCM value > 4095 or < -4096.
//			* Ramp down(if force to end playback).
//		- Playback start/stop.
//
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------

#include "App.h"
#include "SPIFlash.h"
#if ((APU_FILTER_ENABLE == 1))
#include "NuDACFilterEx.h"
#endif
#include "MicSpk.h"
#include <string.h>

#define PLAYBACK_NOACTION   (0)
#define PLAYBACK_START		(1)
#define PLAYBACK_RAMP		(2)

extern volatile UINT8 g_u8AppCtrl;

extern INT16 g_ai16DACSamples[];

static UINT8	s_u8PlayCtrl = PLAYBACK_NOACTION;	
S_BUF_CTRL *g_psDacBufCtrl;
INT16 *g_pi16PcmBuf = 0;

#if ( PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1 )

#define PLAYBACK_SET_DB_SMOOTH	(300)

typedef struct
{
	uint8_t  u8DBIndex;			// Volume dB current index for g_u32Playback_Volume_DB_Table.
	uint8_t  u8DBTargetIndex;	// Volume dB target index for g_u32Playback_Volume_DB_Table.
	uint8_t  u8DBRecordIndex;	// Volume dB target index for g_u32Playback_Volume_DB_Table(Fade in used).
	uint16_t u16StepCount;		// Volume step count.
	uint16_t u16Counter;		// Volume step speed counter.
}S_PLAYBACK_VOLUMECONTROL;

S_PLAYBACK_VOLUMECONTROL g_sPlayback_VolumeControl[PLAYBACK_CHANNEL_COUNT];

uint8_t g_u8DBDefaultIndex;

PLAYBACK_DECLARE_VOLUME_DB_TABLE()
#endif

#if ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 2))
#define DAC_BUF_CONSUME_SAMPLES_COUNT	2
		
__align(2) UINT8  g_au8Up2WorkBuf[NUDACFILTEREX_UP2_WORK_BUF_SIZE];
#elif ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 4))
#define DAC_BUF_CONSUME_SAMPLES_COUNT	1
		
__align(2) UINT8  g_au8Up4WorkBuf[NUDACFILTEREX_UP4_WORK_BUF_SIZE];	
#else
#define DAC_BUF_CONSUME_SAMPLES_COUNT	4
#endif

#if ( PLAYBACK_CHANNEL_COUNT > 1)
#include "AudioMixer.h"
S_BUF_CTRL * g_sMixerInBufCtrlList[PLAYBACK_CHANNEL_COUNT+1];
#else
S_BUF_CTRL *g_psDacBufCtrl;
#endif

#define DPWM_FLAUSH_CNT			(2)
static UINT8 s_u8FlaushCnt = DPWM_FLAUSH_CNT;

// ---------------------------------------------------------------------------------------------------------
#if (PLAYBACK_CHANNEL_COUNT > 1)
PINT16 Playback_Process(PINT16 pi16Buff)
{
	UINT8 u8Count = 0;
	
	if (s_u8PlayCtrl)
	{
		if (( s_u8PlayCtrl&PLAYBACK_RAMP) == 0)
		{	
			g_pi16PcmBuf = pi16Buff;

			AudioMixer_MixProcess(g_sMixerInBufCtrlList, DAC_BUF_CONSUME_SAMPLES_COUNT, &g_pi16PcmBuf[4]);
			#if ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 2))
			// Up sampling 2x to reduce "metal sound".
			NuDACFilterEx_Up2Process(g_au8Up2WorkBuf,g_pi16PcmBuf[4], g_pi16PcmBuf);
			NuDACFilterEx_Up2Process(g_au8Up2WorkBuf,g_pi16PcmBuf[5], g_pi16PcmBuf+2);
			#elif ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 4))
			// Up sampling 4x to reduce "metal sound".
			NuDACFilterEx_Up4Process(g_au8Up4WorkBuf,g_pi16PcmBuf[4], g_pi16PcmBuf);
			#else
			g_pi16PcmBuf = &g_pi16PcmBuf[4];
			#endif
		}
		else
		{	
			g_pi16PcmBuf[0] = g_pi16PcmBuf[1] = g_pi16PcmBuf[2] = g_pi16PcmBuf[3];
			
			if (g_pi16PcmBuf[0] == 0)  
			{
				if (s_u8FlaushCnt == 0)
				{
					#if (APU_PDMA_ENABLE)
					PdmaCtrl_Stop(APU_PDMA_CH);
					#endif
					s_u8PlayCtrl &= ~PLAYBACK_RAMP;
					s_u8FlaushCnt = DPWM_FLAUSH_CNT;
					return g_pi16PcmBuf;
				}
				s_u8FlaushCnt--;
			}	
			//#if defined(PLAYBACK_VOLUME_CONTROL)
			do
			{
				Playback_ProcessVolume(0, g_pi16PcmBuf+u8Count);
			}while(++u8Count<4);
			//#endif
		}
		
		#if (APU_PDMA_ENABLE)
		PdmaCtrl_Start(APU_PDMA_CH, (UINT32 *)g_pi16PcmBuf,(uint32_t *)&DPWM->DATA, 4);
		#endif
		
		return g_pi16PcmBuf;
	}else
		return 0;	
}
#else
PINT16 Playback_Process(PINT16 pi16Buff)
{
	
	UINT8 u8Count = 0;	
	
	if (s_u8PlayCtrl)
	{
		if ( ( g_psDacBufCtrl ) && ((s_u8PlayCtrl&PLAYBACK_RAMP) == 0) )
		{	
			if (! BUF_CTRL_IS_PAUSE(g_psDacBufCtrl))
			{
				if ((g_psDacBufCtrl->u16BufReadIdx+=DAC_BUF_CONSUME_SAMPLES_COUNT)>=g_psDacBufCtrl->u16BufCount)
					g_psDacBufCtrl->u16BufReadIdx = 0;
			}
			
			g_pi16PcmBuf = &g_psDacBufCtrl->pi16Buf[g_psDacBufCtrl->u16BufReadIdx];
			
		
			#if ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 2))
			// Up sampling 2x to reduce "metal sound".
			NuDACFilterEx_Up2Process(g_au8Up2WorkBuf,*g_pi16PcmBuf, &pi16Buff[0]);
			NuDACFilterEx_Up2Process(g_au8Up2WorkBuf,*(g_pi16PcmBuf+1), &pi16Buff[2]);
			g_pi16PcmBuf = pi16Buff;
			#elif ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 4))
			// Up sampling 4x to reduce "metal sound".
			NuDACFilterEx_Up4Process(g_au8Up4WorkBuf,*g_pi16PcmBuf, &pi16Buff[0]);
			g_pi16PcmBuf = pi16Buff;
			#endif
		}else
		{
			g_pi16PcmBuf[0] = g_pi16PcmBuf[1] = g_pi16PcmBuf[2] = g_pi16PcmBuf[3];
			
			if (g_pi16PcmBuf[0] == 0)  
			{
				if (s_u8FlaushCnt == 0)
				{
					#if (APU_PDMA_ENABLE)
					PdmaCtrl_Stop(APU_PDMA_CH);
					#endif
					s_u8PlayCtrl &= ~PLAYBACK_RAMP;
					s_u8FlaushCnt = DPWM_FLAUSH_CNT;
					return g_pi16PcmBuf;
				}
				s_u8FlaushCnt--;
			}	
		}
				//#if defined(PLAYBACK_VOLUME_CONTROL)
		do
		{
			Playback_ProcessVolume(0, g_pi16PcmBuf+u8Count);
		}while(++u8Count<4);
		//#endif
		
		#if (APU_PDMA_ENABLE)
		PdmaCtrl_Start(APU_PDMA_CH, (UINT32 *)g_pi16PcmBuf,(uint32_t *)&DPWM->DATA, 4);
		#endif
		
		return g_pi16PcmBuf;
	}else
		return 0;	
}
#endif

//---------------------------------------------------------------------------------------------------------
void Playback_ResetChannelVolume(uint8_t u8Channel)
{
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8Channel];
	
	psVolumeControl->u8DBIndex = g_u8DBDefaultIndex;
	psVolumeControl->u8DBTargetIndex = g_u8DBDefaultIndex;
	psVolumeControl->u8DBRecordIndex = g_u8DBDefaultIndex;
	#endif
}

//---------------------------------------------------------------------------------------------------------
BOOL Playback_IsTargetVolume(uint8_t u8Channel)
{	
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8Channel];
	
	if( psVolumeControl->u8DBIndex != psVolumeControl->u8DBTargetIndex )
		return FALSE;

	#endif
	return TRUE;	
}

//---------------------------------------------------------------------------------------------------------
void Playback_AdjustVolumeDBStep(uint8_t u8Channel, int8_t i8AdjustDBStep, uint16_t u16StepIntervalBytes)
{
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8Channel];
	
	psVolumeControl->u16Counter = 0;
	psVolumeControl->u16StepCount = u16StepIntervalBytes;
	
	if( i8AdjustDBStep > 0 && (psVolumeControl->u8DBIndex + i8AdjustDBStep >= (sizeof(g_u32Playback_Volume_DB_Table)/sizeof(uint32_t))))
		psVolumeControl->u8DBTargetIndex = sizeof(g_u32Playback_Volume_DB_Table)/sizeof(uint32_t) -1;
	else if( i8AdjustDBStep < 0 && ((int8_t)psVolumeControl->u8DBIndex+i8AdjustDBStep < 0) )
		psVolumeControl->u8DBTargetIndex = 0;
	else
		psVolumeControl->u8DBTargetIndex = psVolumeControl->u8DBIndex + i8AdjustDBStep;	
	#endif
}

//---------------------------------------------------------------------------------------------------------
BOOL Playback_SetVolumeDB(uint8_t u8Channel,uint32_t u32DBValue)
{
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)	
	int8_t i8DBIndex;
	
	if(u8Channel < PLAYBACK_CHANNEL_COUNT )
	{
		for( i8DBIndex=0; i8DBIndex<sizeof(g_u32Playback_Volume_DB_Table)/sizeof(uint32_t); i8DBIndex++ )
		{
			if(g_u32Playback_Volume_DB_Table[i8DBIndex]==u32DBValue)
			{
				Playback_AdjustVolumeDBStep(u8Channel,i8DBIndex-(int8_t)g_sPlayback_VolumeControl[u8Channel].u8DBIndex,PLAYBACK_SET_DB_SMOOTH);
				return TRUE;
			}
		}
	}
	#endif	
	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
void Playback_VolumeFadeOut(uint8_t u8ChannelIndex,uint16_t u16StepIntervalBytes)
{
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8ChannelIndex];
	
	if( psVolumeControl->u8DBIndex != 0 )
	{
		psVolumeControl->u8DBRecordIndex = psVolumeControl->u8DBIndex;
		Playback_AdjustVolumeDBStep(u8ChannelIndex,0-((int8_t)psVolumeControl->u8DBIndex),u16StepIntervalBytes);
	}
	#endif	
}

//---------------------------------------------------------------------------------------------------------
void Playback_VolumeFadeIn(uint8_t u8ChannelIndex,uint16_t u16StepIntervalBytes)
{
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8ChannelIndex];
	
	psVolumeControl->u8DBIndex = 0;
	Playback_AdjustVolumeDBStep(u8ChannelIndex,psVolumeControl->u8DBRecordIndex,u16StepIntervalBytes);
	#endif	
}

//---------------------------------------------------------------------------------------------------------
void Playback_ProcessVolume(uint8_t u8Channel,int16_t* pi16Data)
{	
	#if(PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	S_PLAYBACK_VOLUMECONTROL *psVolumeControl = &g_sPlayback_VolumeControl[u8Channel];
	
	if( Playback_IsTargetVolume(u8Channel) == FALSE )
	{
		if( psVolumeControl->u16StepCount == 0 )
			psVolumeControl->u8DBIndex = psVolumeControl->u8DBTargetIndex;
		else if( psVolumeControl->u16Counter >= psVolumeControl->u16StepCount )
		{
			psVolumeControl->u16Counter = 0;
			
			if( psVolumeControl->u8DBIndex > psVolumeControl->u8DBTargetIndex )
				psVolumeControl->u8DBIndex--;
			else
				psVolumeControl->u8DBIndex++;
		}
		else
			psVolumeControl->u16Counter++;		
	}
	
	if( psVolumeControl->u8DBIndex != g_u8DBDefaultIndex )
		*pi16Data = ((int32_t)*pi16Data*g_u32Playback_Volume_DB_Table[psVolumeControl->u8DBIndex]) >> PLAYBACK_VOLUME_FINE_SHIFT;

	#endif
}

// ---------------------------------------------------------------------------------------------------------
void Playback_StartPlay(void)
{
	INT16 *pi16PcmBuf;
	
	if( s_u8PlayCtrl == PLAYBACK_NOACTION )
	{
		#if ( PLAYBACK_CHANNEL_COUNT > 1)
		pi16PcmBuf = g_ai16DACSamples;
		#else
		pi16PcmBuf = &g_psDacBufCtrl->pi16Buf[g_psDacBufCtrl->u16BufReadIdx];
		#endif
		
		#if ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 2))
		NuDACFilterEx_Up2Initial(g_au8Up2WorkBuf);
		#elif ((APU_FILTER_ENABLE == 1)&&(APU_UPSAMPLE == 4))
		NuDACFilterEx_Up4Initial(g_au8Up4WorkBuf);
		#endif
		g_u8AppCtrl|=APPCTRL_PLAY;
		s_u8PlayCtrl |= PLAYBACK_START;
		#if (APU_ENABLE)
		{
			UINT8 u8Count;
			
			for( u8Count = 0; u8Count < 8; u8Count ++)
				g_ai16DACSamples[u8Count] = 0;		//Clear virtual buffer
		}
		#endif
		
		Playback_ResetChannelVolume(0);
		
		SPK_Start();
		
		#if (APU_PDMA_ENABLE)
		PdmaCtrl_Start(APU_PDMA_CH, (uint32_t *)pi16PcmBuf, (uint32_t *)&DPWM->DATA, 8);
		#endif
		
	}
}

// ---------------------------------------------------------------------------------------------------------
void Playback_StopPlay(void)
{
	if( s_u8PlayCtrl == PLAYBACK_START)
	{
		if (g_pi16PcmBuf)
		{
			#if (APU_PDMA_ENABLE)
			PDMA_DisableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
			#endif
			
			s_u8PlayCtrl |= PLAYBACK_RAMP;
			Playback_ResetChannelVolume(0);
			Playback_VolumeFadeOut(0, 10);
		
			#if (APU_PDMA_ENABLE)
			PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
			#endif
	
			while(s_u8PlayCtrl&PLAYBACK_RAMP);
		}
		
		s_u8PlayCtrl &= ~PLAYBACK_START;	
		g_u8AppCtrl&=~APPCTRL_PLAY;
		
		SPK_Stop();
	}
}

// ---------------------------------------------------------------------------------------------------------
void
Playback_SetOutputBuf(
	S_BUF_CTRL* psOutBufCtrl,
	UINT16 u16BufSize,
	INT16* pi16Buf,
	UINT16 u16FrameSize,
	UINT16 u16SampleRate
)
{
	memset(psOutBufCtrl, 0,sizeof(S_BUF_CTRL));
	psOutBufCtrl->u16BufCount = u16BufSize;
	psOutBufCtrl->u16FrameSize = u16FrameSize;
	psOutBufCtrl->pi16Buf = pi16Buf;
	psOutBufCtrl->u16SampleRate = u16SampleRate;
	BUF_CTRL_SET_ACTIVE( psOutBufCtrl);
	memset( psOutBufCtrl->pi16Buf, 0, psOutBufCtrl->u16BufCount*sizeof(UINT16));
}

// ---------------------------------------------------------------------------------------------------------
void
Playback_UpdateOutputBuf(
	S_BUF_CTRL* psOutBufCtrl
)
{
	if ((psOutBufCtrl->u16BufWriteIdx+=psOutBufCtrl->u16FrameSize)>=psOutBufCtrl->u16BufCount)
		psOutBufCtrl->u16BufWriteIdx = 0;
}

// ---------------------------------------------------------------------------------------------------------
BOOL
Playback_NeedUpdateOutputBuf(
	S_BUF_CTRL* psOutBufCtrl
)
{
	return ( ( psOutBufCtrl->u16BufWriteIdx > psOutBufCtrl->u16BufReadIdx ) ||
		( psOutBufCtrl->u16BufReadIdx - psOutBufCtrl->u16BufWriteIdx >= psOutBufCtrl->u16FrameSize ) );
}

// ---------------------------------------------------------------------------------------------------------
void Playback_Initiate(void)
{
	#if ( PLAYBACK_CHANNEL_COUNT > 1)
	{
		int i;
		
		for(i=0; i < PLAYBACK_CHANNEL_COUNT; i++ )
			g_sMixerInBufCtrlList[i] = NULL;
		g_sMixerInBufCtrlList[PLAYBACK_CHANNEL_COUNT] = AUDIOMIXER_BUL_CTRL_END;
	}
	#endif

	#if (PLAYBACK_CHANNEL_COUNT > 0 && PLAYBACK_VOLUME_CONTROL == 1)
	{
		uint8_t u8i;
		// Get default DB index.
		for( u8i=0; u8i<sizeof(g_u32Playback_Volume_DB_Table)/sizeof(uint32_t); u8i++ )
		{
			if(g_u32Playback_Volume_DB_Table[u8i] == PLAYBACK_VOLUME_0_DB)
			{
				g_u8DBDefaultIndex = u8i;
				break;
			}
		}
		// Reset all channel volume.
		for( u8i=0; u8i<PLAYBACK_CHANNEL_COUNT; u8i++)
			Playback_ResetChannelVolume(u8i);
	}
	#endif
}

// ---------------------------------------------------------------------------------------------------------
void Playback_Add(
	UINT8 u8Channel,
	S_BUF_CTRL *psBufCtrl
)
{
	UINT32 u32SampleRate;

	if ( u8Channel >= PLAYBACK_CHANNEL_COUNT )
		return;
	
	#if (APU_PDMA_ENABLE)
	PDMA_DisableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
	#endif
	
#if ( PLAYBACK_CHANNEL_COUNT > 1)
	g_sMixerInBufCtrlList[u8Channel] = psBufCtrl;
	u32SampleRate = AudioMixer_ChangeSR(g_sMixerInBufCtrlList);
#else
	g_psDacBufCtrl = psBufCtrl;
	u32SampleRate = g_psDacBufCtrl->u16SampleRate;
#endif
		
	if( u32SampleRate == 0 )
		Playback_StopPlay();
	else
	{
		SPK_ChangeSR(0, u32SampleRate);
		#if (APU_PDMA_ENABLE)
		PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
		#endif
	}
}

// ---------------------------------------------------------------------------------------------------------
void Playback_Remove(
	UINT8 u8Channel
)
{	
	UINT32 u32SampleRate;
	
	if ( u8Channel >= PLAYBACK_CHANNEL_COUNT )
		return;
	
	#if (APU_PDMA_ENABLE)
	PDMA_DisableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
	#endif
	
#if ( PLAYBACK_CHANNEL_COUNT > 1)
	g_sMixerInBufCtrlList[u8Channel] = NULL;
	u32SampleRate = AudioMixer_ChangeSR(g_sMixerInBufCtrlList);
#else
	g_psDacBufCtrl = NULL;
	u32SampleRate = 0;
#endif
	
	if( u32SampleRate == 0 )
		Playback_StopPlay();
	else
	{	
		SPK_ChangeSR(0, u32SampleRate);
		#if (APU_PDMA_ENABLE)
		PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
		#endif
	}
	
}

// ---------------------------------------------------------------------------------------------------------
void Playback_PauseCtrl(uint8_t u8Channel,BOOL bEnable)
{
	#if( PLAYBACK_VOLUME_CONTROL == 1)
	{
		S_BUF_CTRL *psBufCtrl;

		if ( u8Channel >= PLAYBACK_CHANNEL_COUNT )
			return;
	
		#if ( PLAYBACK_CHANNEL_COUNT > 1)
		{
			psBufCtrl = g_sMixerInBufCtrlList[u8Channel];
	
			if (psBufCtrl)
			{
				#if (APU_PDMA_ENABLE)
				PDMA_DisableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
				#endif
				if ( bEnable)
				{
					Playback_VolumeFadeOut(u8Channel, 1);
					BUF_CTRL_SET_PAUSE(psBufCtrl);
				}
				else
				{
					Playback_VolumeFadeIn(u8Channel, 1);
					BUF_CTRL_SET_RESUME(psBufCtrl);
				}
				#if (APU_PDMA_ENABLE)
				PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
				#endif
			}
		}
		#else
		{
			psBufCtrl = g_psDacBufCtrl;
	
			if (psBufCtrl)
			{
				#if (APU_PDMA_ENABLE)
				PDMA_DisableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
				#endif
				if ( bEnable)
				{
					Playback_VolumeFadeOut(0, 1);
					#if (APU_PDMA_ENABLE)
					PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
					#endif
					while(Playback_IsTargetVolume(0) == FALSE);
					BUF_CTRL_SET_PAUSE(psBufCtrl);
				}
				else
				{
					Playback_VolumeFadeIn(0, 1);
					#if (APU_PDMA_ENABLE)
					PDMA_EnableInt(APU_PDMA_CH, PDMA_INTENCH_TXOKIEN_Msk);
					#endif
					BUF_CTRL_SET_RESUME(psBufCtrl);
				}
			}
		}
		#endif
	}
	#endif
}

// ---------------------------------------------------------------------------------------------------------
void Playback_MuteCtrl(uint8_t u8Channel,BOOL bEnable)
{
	#if( PLAYBACK_VOLUME_CONTROL == 1)
	{
		S_BUF_CTRL *psBufCtrl;

		if ( u8Channel >= PLAYBACK_CHANNEL_COUNT )
			return;
	
		#if ( PLAYBACK_CHANNEL_COUNT > 1)
		{
			psBufCtrl = g_sMixerInBufCtrlList[u8Channel];
			
			if (psBufCtrl)
			{
				SPK_Pause(0);
				if ( bEnable)
				{
					Playback_VolumeFadeOut(u8Channel, 1);
					BUF_CTRL_SET_MUTE(psBufCtrl);
				}
				else
				{
					Playback_VolumeFadeIn(u8Channel, 1);
					BUF_CTRL_SET_UNMUTE(psBufCtrl);
				}
				SPK_Resume(0);
			}
		}
		#else
		{
			psBufCtrl = g_psDacBufCtrl;
	
			if (psBufCtrl)
			{
				SPK_Pause(0);
				if ( bEnable)
				{
					Playback_VolumeFadeOut(0, 1);
					BUF_CTRL_SET_MUTE(psBufCtrl);
				}
				else
				{
					Playback_VolumeFadeIn(0, 1);
					BUF_CTRL_SET_UNMUTE(psBufCtrl);
				}
				SPK_Resume(0);
			}
		}
		#endif
	}
	#endif
}

// ---------------------------------------------------------------------------------------------------------
UINT32 Playback_GetPlayingChannels(void)
{
#if ( PLAYBACK_CHANNEL_COUNT > 1)
	int i;
	UINT32 u32ChannelStatus = 0;
		
	for(i = 0; i < PLAYBACK_CHANNEL_COUNT; i++ )
	{
		if ( g_sMixerInBufCtrlList[i] && BUF_CTRL_IS_ACTIVE(g_sMixerInBufCtrlList[i]) )
			u32ChannelStatus |= 1<<i;
	}
	return u32ChannelStatus;
#else
	if ( g_psDacBufCtrl && BUF_CTRL_IS_ACTIVE(g_psDacBufCtrl) )
		return 1;

	return 0;
#endif
}
