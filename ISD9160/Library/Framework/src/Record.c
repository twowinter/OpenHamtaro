/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "App.h"
#include "string.h"
#include "MicSpk.h"
#if (ADC_FILTER_ENABLE == 1) 	
#include "NoiseFilter.h"
#define NoiseFilter_IIR2 NoiseFilter_IIR2x4
#endif

extern INT16 g_ai16ADCSamples[];
static INT16 *s_pi16AdcBuf;

extern volatile UINT8		g_u8AppCtrl;
S_BUF_CTRL *g_psAdcBufCtrl;

//-----------------------------------------------------------------------------------------------------------
// ADC IRQ handler.
//-----------------------------------------------------------------------------------------------------------
void Record_Process(PINT16 pi16Buff)
{
	
	#if ((ADC_FILTER_ENABLE == 1)&&(ADC_DOWNSAMPLE==2))
	#define ADC_SAMPLES_COUNT	4
	INT16 *pi16inBuf;
	pi16inBuf = &g_psAdcBufCtrl->pi16Buf[g_psAdcBufCtrl->u16BufWriteIdx];
	pi16inBuf[0] = NoiseFilter_IIR2(s_pi16AdcBuf[0]);
	NoiseFilter_IIR2(s_pi16AdcBuf[1]);
	
	pi16inBuf[1] = NoiseFilter_IIR2(s_pi16AdcBuf[2]);
	NoiseFilter_IIR2(s_pi16AdcBuf[3]);
	
	pi16inBuf[2] = NoiseFilter_IIR2(s_pi16AdcBuf[4]);
	NoiseFilter_IIR2(s_pi16AdcBuf[5]);
	
	pi16inBuf[3] = NoiseFilter_IIR2(s_pi16AdcBuf[6]);
	NoiseFilter_IIR2(s_pi16AdcBuf[7]);
	
	#elif ((ADC_FILTER_ENABLE == 1)&&(ADC_DOWNSAMPLE==4))
	#define ADC_SAMPLES_COUNT	2
	INT16 *pi16inBuf;
	pi16inBuf = &g_psAdcBufCtrl->pi16Buf[g_psAdcBufCtrl->u16BufWriteIdx];
	
	pi16inBuf[0] = NoiseFilter_IIR2(s_pi16AdcBuf[0]);
	NoiseFilter_IIR2(s_pi16AdcBuf[1]);
	NoiseFilter_IIR2(s_pi16AdcBuf[2]);
	NoiseFilter_IIR2(s_pi16AdcBuf[3]);
	
	pi16inBuf[1] = NoiseFilter_IIR2(s_pi16AdcBuf[4]);
	NoiseFilter_IIR2(s_pi16AdcBuf[5]);
	NoiseFilter_IIR2(s_pi16AdcBuf[6]);
	NoiseFilter_IIR2(s_pi16AdcBuf[7]);
	#else
	#define ADC_SAMPLES_COUNT	8
		
	#endif
	
	if ( BUF_CTRL_ISNOT_CALLBACK(g_psAdcBufCtrl))
	{
		g_psAdcBufCtrl->u16BufWriteIdx += ADC_SAMPLES_COUNT;
		if ( g_psAdcBufCtrl->u16BufWriteIdx >= g_psAdcBufCtrl->u16BufCount )
			g_psAdcBufCtrl->u16BufWriteIdx = 0;
		
		#if (ADC_FILTER_ENABLE != 1)
		s_pi16AdcBuf = &g_psAdcBufCtrl->pi16Buf[g_psAdcBufCtrl->u16BufWriteIdx];
		#endif
	}
	else
	{
		((S_BUF_CTRL_CALLBACK*)g_psAdcBufCtrl)->pfnFunc(((S_BUF_CTRL_CALLBACK*)g_psAdcBufCtrl)->pu8Param, ADC_SAMPLES_COUNT, s_pi16AdcBuf);
	}	
	
	#if (ADC_PDMA_ENABLE == 1)
	PdmaCtrl_Start(ADC_PDMA_CH, (uint32_t *)&ADC->DAT, (uint32_t *)s_pi16AdcBuf, 8);
	#endif

}

void Record_Add(
	S_BUF_CTRL *psAdcBufCtrl,
	UINT32 u32SampleRate
)
{
	g_psAdcBufCtrl = psAdcBufCtrl;
	if ( u32SampleRate )
		MIC_ChangeSR(u32SampleRate);
}
/************************************************************************************************************
 * Description:
 *  Start to record. 
 *		
 * Argument:
 * 	None.
 ************************************************************************************************************/
void Record_StartRec(void)
{
	
	g_u8AppCtrl |= APPCTRL_RECORD;

	#if (ADC_FILTER_ENABLE == 1)
	NoiseFilter_ResetIIR2();
	s_pi16AdcBuf = g_ai16ADCSamples;
	#else
	if ( BUF_CTRL_ISNOT_CALLBACK(g_psAdcBufCtrl))	
		s_pi16AdcBuf = &g_psAdcBufCtrl->pi16Buf[g_psAdcBufCtrl->u16BufWriteIdx];
	else
		s_pi16AdcBuf = g_ai16ADCSamples;
	#endif
	
	#if (ADC_PDMA_ENABLE == 1)	
	PdmaCtrl_Start(ADC_PDMA_CH, (uint32_t *)&ADC->DAT, (uint32_t *)s_pi16AdcBuf, 8);
	#endif

	MIC_Start();
}


void  Record_StopRec(void)
{
	g_u8AppCtrl &= ~APPCTRL_RECORD;
	
	#if (ADC_PDMA_ENABLE == 1)	
	PdmaCtrl_Stop(ADC_PDMA_CH);
	#endif
	
	MIC_Stop();

}

/************************************************************************************************************
 * Description:
 *  Set parameters to input buffer structur for callback feature
 *		
 * Argument:
 * 	None.
 ************************************************************************************************************/
void 
Record_SetInBufCallback(
	S_BUF_CTRL_CALLBACK *psInBufCtrl,
	PFN_DATA_REQUEST_CALLBACK pfnSetIntputData,
	void* pWorkBuf
)
{
	BUF_CTRL_SET_CALLBACK(psInBufCtrl);
	psInBufCtrl->pfnFunc = pfnSetIntputData;
	psInBufCtrl->pu8Param = pWorkBuf;
	BUF_CTRL_SET_ACTIVE(psInBufCtrl);
}

/************************************************************************************************************
 * Description:
 *  Set parameters to input buffer structur for record feature
 *		
 * Argument:
 * 	None.
 ************************************************************************************************************/
void 
Record_SetInBufRecord(
	S_BUF_CTRL* psInBufCtrl,
	UINT16 u16BufSize,
	INT16* pi16Buf,
	UINT16 u16FrameSize,
	UINT16 u16SampleRate
)
{
	memset(psInBufCtrl,0,sizeof(S_BUF_CTRL));
	psInBufCtrl->u8Flag = S_BUF_CTRL_FLAG_BUF;
	psInBufCtrl->u16BufCount = u16BufSize;
	psInBufCtrl->pi16Buf = pi16Buf;
	psInBufCtrl->u16BufReadIdx = 0;
	psInBufCtrl->u16BufWriteIdx = u16FrameSize;
	psInBufCtrl->u16SampleRate = u16SampleRate;
	psInBufCtrl->u16FrameSize = u16FrameSize;
	BUF_CTRL_SET_ACTIVE(psInBufCtrl);
	memset( psInBufCtrl->pi16Buf, 0, u16FrameSize*2);
}

