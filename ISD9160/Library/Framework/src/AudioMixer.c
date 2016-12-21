#include "AudioMixer.h"

//---------------------------------------------------------------------------------------------------------
#if defined(PLAYBACK_SAME_SAMPLERATE)
UINT8 AudioMixer_MixProcess(S_BUF_CTRL **ppInBufCtrlList, INT16 i16DataBufCount, INT16 ai16DataBuf[])
{
	S_BUF_CTRL *psInBufCtrl;
	INT32 i32Temp;
	INT16 *pi16Src, i;
	UINT8 u8Ch = 0;
	
	for( i = 0; i < i16DataBufCount; i ++)
		ai16DataBuf[i] = 0;
	
	while( (psInBufCtrl = *ppInBufCtrlList ++) != AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;

		if (BUF_CTRL_IS_PAUSE(psInBufCtrl))
		{
			if (Playback_IsTargetVolume(u8Ch))
				continue;
		}
		
		pi16Src = &psInBufCtrl->pi16Buf[psInBufCtrl->u16BufReadIdx];
				
		if ((psInBufCtrl->u16BufReadIdx+=i16DataBufCount) >= psInBufCtrl->u16BufCount)
			psInBufCtrl->u16BufReadIdx = 0;
			
		i = 0;
		do 
		{
			Playback_ProcessVolume(u8Ch, pi16Src);
			i32Temp = ai16DataBuf[i]; 
			i32Temp += *pi16Src++;
			if (i32Temp> APU_MAX_RESOULTION) // overflow, positive->negative
				i32Temp = APU_MAX_RESOULTION;
			else if (i32Temp < APU_MIN_RESOULTION) // overflow, negative->positive
				i32Temp = APU_MIN_RESOULTION;
			ai16DataBuf[i] = i32Temp;

		}while(++i < i16DataBufCount);	
		u8Ch++;
	}
	return i16DataBufCount;
}
//---------------------------------------------------------------------------------------------------------
UINT32 AudioMixer_ChangeSR(S_BUF_CTRL **ppsInBufCtrlList)
{
	UINT32 u32MainSampleRate;
	S_BUF_CTRL *psInBufCtrl;
	S_BUF_CTRL **ppsInBufCtrlListTemp;
		
	u32MainSampleRate = 0;
	ppsInBufCtrlListTemp = ppsInBufCtrlList;
	
	while( (psInBufCtrl =*ppsInBufCtrlListTemp ++)!= AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;
		if (psInBufCtrl->u16SampleRate == 0)
			*ppsInBufCtrlListTemp = NULL;
		else
			u32MainSampleRate = psInBufCtrl->u16SampleRate;
	}
	
	if (u32MainSampleRate == 0)
		return 0;
	
	ppsInBufCtrlListTemp = ppsInBufCtrlList;
	while( (psInBufCtrl =*ppsInBufCtrlListTemp ++)!= AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;
		
		if ( psInBufCtrl->u16SampleRate != u32MainSampleRate )
			*ppsInBufCtrlListTemp = NULL;
	}
	return u32MainSampleRate;
}

#else
UINT8 AudioMixer_MixProcess(S_BUF_CTRL **ppInBufCtrlList, INT16 i16DataBufCount, INT16 ai16DataBuf[])
{
	S_BUF_CTRL *psInBufCtrl;
	INT32 i32Temp;
	INT16 *pi16Src, i;
	UINT8 u8Ch = 0;
	UINT16 u16DupSampleCalculate, u16DupSampleRatio;
	
	for( i = 0; i < i16DataBufCount; i ++)
		ai16DataBuf[i] = 0;
	
	while( (psInBufCtrl = *ppInBufCtrlList ++) != AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;
		
		if (BUF_CTRL_IS_PAUSE(psInBufCtrl))
		{
			if (Playback_IsTargetVolume(u8Ch))
				continue;
		}		
			
		pi16Src = &psInBufCtrl->pi16Buf[psInBufCtrl->u16BufReadIdx];
		if (  psInBufCtrl->u16ReSamplingCalculation == AUDIOMIXER_SAME_SAMPLE_RATE )
		{
			// The sample rate is the same as DAC playback. 
			// No need to duplicate samples
			i = 0;
			do 
			{
				Playback_ProcessVolume(u8Ch, pi16Src);
				i32Temp = ai16DataBuf[i]; 
				i32Temp += *pi16Src++;
				if (i32Temp> APU_MAX_RESOULTION) // overflow, positive->negative
					i32Temp = APU_MAX_RESOULTION;
				else if (i32Temp < APU_MIN_RESOULTION) // overflow, negative->positive
					i32Temp = APU_MIN_RESOULTION;
				ai16DataBuf[i] = i32Temp;

				if ((psInBufCtrl->u16BufReadIdx+=1) >= psInBufCtrl->u16BufCount)
				{	
					psInBufCtrl->u16BufReadIdx = 0;
					pi16Src = &psInBufCtrl->pi16Buf[0];
				}
			}while(++i < i16DataBufCount);
				
			continue;
		}
			// For AUDIOMIXER_DUPLICATE defined:
			// 		Sample Ratio (8bits): represent 1 sample(low sample rate) should be duplicated to how many samples(high sample rate)
			//			bit7~bit4: integer part
			//			bit3~bit0: fraction part
			// 		Sample Calculate (8bit): represent how many samples should be duplicated from current sample(low sample rate)
			//			bit7~bit4: integer part
			//			bit3~bit0: fraction part
			// For MIXER_DUPLICATE not defined:
			// 		Sample Ratio (8bits):
			//			bit7~bit0: fraction part
			// 		Sample Calculate (8bit):
			//			bit7~bit0: fraction part
		u16DupSampleRatio = psInBufCtrl->u16ReSamplingCalculation >> 8;
		u16DupSampleCalculate = psInBufCtrl->u16ReSamplingCalculation & 0xff;
		
		for( i = 0; i < i16DataBufCount; i ++)
		{
#ifdef AUDIOMIXER_DUPLICATE	
			if ( u16DupSampleCalculate >= 0x10 )
			{
				// Duplicate same PCM.
				u16DupSampleCalculate -= 0x10;
				ai16DataBuf[i] += *pi16Src;
			}
			else
			{
				INT16 i16PrevPCM;
				// Use intrpolation to caclulate new PCM.
				{
					i16PrevPCM = *pi16Src++;
					if ((++psInBufCtrl->u16BufReadIdx) >= psInBufCtrl->u16BufCount)
					{
						psInBufCtrl->u16BufReadIdx = 0;
						pi16Src = &psInBufCtrl->pi16Buf[0];
					}
					i32Temp = ai16DataBuf[i];
					i32Temp  += (((INT32)i16PrevPCM*u16DupSampleCalculate + (INT32)(*pi16Src)*(0x10 - u16DupSampleCalculate)))>>4;
					u16DupSampleCalculate = (u16DupSampleCalculate-0x10)+u16DupSampleRatio;
				}
			}
#else
			// Use intrpolation to caclulate new PCM.
			INT32 i32PrevPCM, i32NextPCM;
			i32PrevPCM = *pi16Src;
			if ((psInBufCtrl->u16BufReadIdx+1) >= psInBufCtrl->u16BufCount)
				i32NextPCM = psInBufCtrl->pi16Buf[0];
			else
				i32NextPCM = *(pi16Src+1);
			
			Playback_ProcessVolume(u8Ch, pi16Src);
			Playback_ProcessVolume(u8Ch, (INT16 *)&i32NextPCM);
			
			i32Temp = ai16DataBuf[i];
			i32Temp += ((i32PrevPCM*(0x100-u16DupSampleCalculate) + (i32NextPCM*u16DupSampleCalculate))>>8);
			u16DupSampleCalculate = u16DupSampleCalculate+u16DupSampleRatio;
			if ( u16DupSampleCalculate >= 0x100 )
			{
				u16DupSampleCalculate -= 0x100;
				if ((++psInBufCtrl->u16BufReadIdx) >= psInBufCtrl->u16BufCount)
				{
					psInBufCtrl->u16BufReadIdx = 0;
					pi16Src = &psInBufCtrl->pi16Buf[0];
				}
				else
					pi16Src ++;
			}
#endif
			
			if (i32Temp > APU_MAX_RESOULTION) // overflow, positive->negative
				i32Temp = APU_MAX_RESOULTION;
			else if (i32Temp < APU_MIN_RESOULTION) // overflow, negative->positive
				i32Temp = APU_MIN_RESOULTION;
			ai16DataBuf[i] = i32Temp; 
		}
			
		psInBufCtrl->u16ReSamplingCalculation = (psInBufCtrl->u16ReSamplingCalculation&0xff00) | u16DupSampleCalculate;
	}
			
	return i16DataBufCount;
}

//---------------------------------------------------------------------------------------------------------
UINT32 AudioMixer_ChangeSR(S_BUF_CTRL **ppsInBufCtrlList)
{
	UINT32 u32MaxSampleRate;
	S_BUF_CTRL *psInBufCtrl;
	S_BUF_CTRL **ppsInBufCtrlListTemp;
	UINT8 u8DupSmampleRatio;
	
	u32MaxSampleRate = 0;
	ppsInBufCtrlListTemp = ppsInBufCtrlList;
	while( (psInBufCtrl =*ppsInBufCtrlListTemp ++)!= AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;
		if (psInBufCtrl->u16SampleRate == 0)
		{
			*ppsInBufCtrlListTemp = NULL;
			continue;
		}
		if ( psInBufCtrl->u16SampleRate > u32MaxSampleRate )
			u32MaxSampleRate = psInBufCtrl->u16SampleRate;
	}

	ppsInBufCtrlListTemp = ppsInBufCtrlList;
	while( (psInBufCtrl =*ppsInBufCtrlListTemp ++)!= AUDIOMIXER_BUL_CTRL_END )
	{
		if ( (psInBufCtrl == NULL) || BUF_CTRL_IS_INACTIVE(psInBufCtrl) )
			continue;
		
		if ( psInBufCtrl->u16SampleRate < u32MaxSampleRate )
		{
#ifdef AUDIOMIXER_DUPLICATE
			u8DupSmampleRatio = (UINT8)(((u32MaxSampleRate<<16)/psInBufCtrl->u16SampleRate)>>12);
			psInBufCtrl->u16UpSamplingCalculation = (u8DupSmampleRatio<<8) | u8DupSmampleRatio;
#else
			u8DupSmampleRatio = (UINT8)(((((UINT32)psInBufCtrl->u16SampleRate)<<16)/u32MaxSampleRate)>>8);
			psInBufCtrl->u16ReSamplingCalculation = (u8DupSmampleRatio<<8);
#endif			
		}
		else
			psInBufCtrl->u16ReSamplingCalculation = AUDIOMIXER_SAME_SAMPLE_RATE;
	}
	
	return u32MaxSampleRate;
}

#endif
//---------------------------------------------------------------------------------------------------------
UINT32 AudioMixer_Initiate(S_BUF_CTRL_CALLBACK *psMixerCtl, S_BUF_CTRL **ppsInBufCtrlList)
{
	psMixerCtl->u8Flag = S_BUF_CTRL_FLAG_CALLBACK;
	psMixerCtl->pfnFunc = (PFN_DATA_REQUEST_CALLBACK)AudioMixer_MixProcess;
	psMixerCtl->pu8Param = (void *)ppsInBufCtrlList;

	return AudioMixer_ChangeSR(ppsInBufCtrlList);
}

