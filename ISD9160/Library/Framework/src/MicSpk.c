/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- MIC functions:
//			* Open MIC.
//			* Close MIC.
//			* Pause MIC recording.
//			* Resume MIC recording.
//			* Change MIC recording sampling rate.
//		- SPK functions:
//			* Open speaker.
//			* Close speaker.
//			* Pause speaker playback.
//			* Resume speaker playback.
//			* Change speaker playback sampling rate.
//
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------
#include "Platform.h"
#include "ConfigApp.h"
#include "MicSpk.h"
#include "PdmaCtrl.h"

#if( !defined(__CHIP_SERIES__) )
#error "Please update all files in library from SDS version >= v5.00."
#endif


#if ((ADC_FILTER_ENABLE==1) && (ADC_DOWNSAMPLE==2))
	#define ADC_DOWNSAMPLE_ORDER	1
#elif ((ADC_FILTER_ENABLE==1) && (ADC_DOWNSAMPLE==4))
	#define ADC_DOWNSAMPLE_ORDER	2
#else
	#define ADC_DOWNSAMPLE_ORDER	0
#endif

#if ((APU_FILTER_ENABLE==1) && (APU_UPSAMPLE==2))
	#define APU_UPSAMPLE_ORDER	1
#elif ((APU_FILTER_ENABLE==1) && (APU_UPSAMPLE==4))
	#define APU_UPSAMPLE_ORDER	2
#else
	#define APU_UPSAMPLE_ORDER	0
#endif

#if ((ADC_FILTER_ENABLE == 2) || (APU_FILTER_ENABLE == 2))
// -------------------------------------------------------------------------------------------------------------------------------
//Filter Coefficients properties:
//	1. Butterworth IIR low pass
//	2. 3 section (second order section)
//	3. Cut off frequency = 0.3fs
//	4. 6 order
//	5. Coefficients format: fixed ponit 3.16
// -------------------------------------------------------------------------------------------------------------------------------
const UINT32 g_u32BiqCoeff[15]={0x08676, 0x10cec, 0x08676, 0x07ef6, 0x09ade,
						0x0642e, 0x0c85c, 0x0642e, 0x05e9a, 0x03220,
						0x05754, 0x0aea8, 0x05754, 0x05276, 0x00ada};												
#endif


static void BIQFilter_Setting(void)
{
#if (ADC_FILTER_ENABLE == 2) || (APU_FILTER_ENABLE == 2)
	UINT8	u8temp;
	
	/* Enable BIQ filter clock */
	CLK_EnableModuleClock(BFAL_MODULE);
	/* wait 32 internal system clocks */
	for(u8temp=0; u8temp<32; u8temp++);
	BIQ_Reset();
	BIQ_SetCoeff((UINT32 *)g_u32BiqCoeff);
		
	/* wait 32 internal system clocks */
	for(u8temp=0; u8temp<32; u8temp++);
	
#endif	
}

//---------------------------------------------------------------------------
// Description:
//		Change the sample rate of MIC recording
// Argument:
//		u32SampleRate: wanted sample rate
// Return:
//		none 
//---------------------------------------------------------------------------
void MIC_ChangeSR(
	UINT32 u32SampleRate
)
{
#define CHK_ADC_DIV(div)		(((div>=8)&&(div<=48))?TRUE:FALSE)
	
	UINT16 au16OSR[4] = {64, 128, 192, 384};
	UINT8 u8Div, u8OSRIdx = 0;

#if (ADC_DOWNSAMPLE_ORDER)
	u32SampleRate <<=  ADC_DOWNSAMPLE_ORDER;
#endif

#if (ADC_FILTER_ENABLE == 2)
	BIQ_SET_ADCPATH(BIQ, SystemCoreClock/u32SampleRate, 3); //down x4
	BIQ_START_RUN(BIQ) ;
#endif	
	
	do{		
		u8Div = SystemCoreClock/u32SampleRate/au16OSR[u8OSRIdx];
		if (CHK_ADC_DIV(u8Div))
			break;
	}while(++u8OSRIdx<4);

	ADC_SET_OSRATION(ADC, u8OSRIdx);
	ADC_SET_SDCLKDIV(ADC, u8Div);
		
}

//---------------------------------------------------------------------------
// Description:
// 	Configure ADC pins and mode for MIC recording function
// Argument:
// 	none
// Return:
//	None.
//---------------------------------------------------------------------------
void MIC_Open(void)
{
	/* Reset IP */
	CLK_EnableModuleClock(ADC_MODULE);
	CLK_EnableModuleClock(ANA_MODULE);
	SYS_ResetModule(EADC_RST);
	
	/* Enable Analog block power */
	ADC_ENABLE_SIGNALPOWER(ADC,
	                       ADC_SIGCTL_ADCMOD_POWER|
						   ADC_SIGCTL_IBGEN_POWER|
	                       ADC_SIGCTL_BUFADC_POWER|
	                       ADC_SIGCTL_BUFPGA_POWER);
	
	/* PGA Setting */
	ADC_MUTEON_PGA(ADC, ADC_SIGCTL_MUTE_PGA);
	ADC_MUTEOFF_PGA(ADC, ADC_SIGCTL_MUTE_IPBOOST);
	ADC_ENABLE_PGA(ADC, ADC_PGACTL_REFSEL_VMID, ADC_PGACTL_BOSST_GAIN_26DB);
	ADC_SetPGAGaindB(ADC_PGA_GAIN); 
	
	/* MIC circuit configuration */
	ADC_ENABLE_VMID(ADC, ADC_VMID_HIRES_DISCONNECT, ADC_VMID_LORES_CONNECT);
	ADC_EnableMICBias(ADC_MICBSEL_90_VCCA);
	ADC_SetAMUX(ADC_MUXCTL_MIC_PATH, ADC_MUXCTL_POSINSEL_NONE, ADC_MUXCTL_NEGINSEL_NONE);
	
	/* Open ADC block */
	ADC_Open();
	
	ADC_SET_FIFOINTLEVEL(ADC, 7);
	ADC_MUTEOFF_PGA(ADC, ADC_SIGCTL_MUTE_PGA);

	BIQFilter_Setting();

	ADC_STOP_CONV(ADC);
	
	
#if (ADC_PDMA_ENABLE)
	ADC_ENABLE_PDMA(ADC);
	PdmaCtrl_Open(ADC_PDMA_CH, PDMA_ADC_SEL);
#else
	NVIC_ClearPendingIRQ(ADC_IRQn);		// Clear pending ADC interrupt although disable interrupt during doing pop sound canceling	
	NVIC_EnableIRQ(ADC_IRQn);	
#endif

}

//---------------------------------------------------------------------------
// Description:
//		Start ADC to recording from MIC
// Argument:
// 		none
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Start(void)
{
	
	ADC_START_CONV(ADC);							// start convert	
	
#if (ADC_PDMA_ENABLE == 0)
	ADC_EnableInt(ADC_FIFO_INT);
#endif
}

//---------------------------------------------------------------------------
// Description:
//		Stop ADC to recording from MIC
// Argument:
// 		none
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Stop(void)
{
#if (ADC_PDMA_ENABLE == 0)
	ADC_DisableInt(ADC_FIFO_INT);
#endif
	
	ADC_STOP_CONV(ADC);
}


//---------------------------------------------------------------------------
// Description:
//		Close ADC to recording from MIC
// Argument:
// 		none
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Close(void)
{
	ADC_STOP_CONV(ADC);
	ADC_DisableInt(ADC_FIFO_INT);
	
	ADC_DISABLE_PGA(ADC);
	ADC_DISABLE_VMID(ADC);
	ADC_DisableMICBias();
	
	ADC_DISABLE_SIGNALPOWER(ADC,
							ADC_SIGCTL_ADCMOD_POWER|
							ADC_SIGCTL_IBGEN_POWER|
							ADC_SIGCTL_BUFADC_POWER|
							ADC_SIGCTL_BUFPGA_POWER);
	ADC_Close();
	CLK_DisableModuleClock(ADC_MODULE);
}

/*****************************************************************************/
/*****************************Speaker config APIs*****************************/
/*****************************************************************************/

//---------------------------------------------------------------------------
// Description:
//		Change the sample rate of Speaker
// Argument:
//		u32SampleRate: wanted sample rate
// Return:
//		none 
//---------------------------------------------------------------------------
void SPK_ChangeSR(
	UINT32 u32Channel,
	UINT32 u32SampleRate
)
{
#if (APU_UPSAMPLE_ORDER!=0)
	u32SampleRate <<= APU_UPSAMPLE_ORDER;
#endif

#if (APU_FILTER_ENABLE == 2)
	BIQ_SET_DPWMPATH(BIQ, SystemCoreClock/u32SampleRate, 3);//up x4
#endif	
	
	DPWM_SetSampleRate(u32SampleRate); //Set sample rate
}


//---------------------------------------------------------------------------
// Description:
// 		Configure APU mode for speaker 
// Argument:
// 		none
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Open(void)
{
	/* Reset IP */
	CLK_EnableModuleClock(DPWM_MODULE);
	CLK_SetModuleClock(DPWM_MODULE, APU_CLK_SOURCE, MODULE_NoMsk);
	SYS_ResetModule(DPWM_RST);
	
	DPWM_Open();	 
	
	DPWM_SET_MODFREQUENCY(DPWM,DPWM_CTL_MODUFRQ0);//Set FREQ_0
	
	BIQFilter_Setting();

#if (APU_PDMA_ENABLE)
	DPWM_ENABLE_PDMA(DPWM);
	PdmaCtrl_Open(APU_PDMA_CH, PDMA_APU_SEL);
#endif
	
	 /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPG multi-function pins for SPK+ and SPK- */
	SYS->GPA_MFP  = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA12MFP_Msk) ) | SYS_GPA_MFP_PA12MFP_SPKP;
	SYS->GPA_MFP  = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA13MFP_Msk) ) | SYS_GPA_MFP_PA13MFP_SPKM;

	//SPK_Pause(0);
}

void SPK_Start(void)
{
	UINT8 u8Count;
	for( u8Count = 0; u8Count < 16; u8Count ++)
		DPWM->DATA =0;	//Flush 0 (VIRTUAL_BUFF_SIZE samples) to DPWM FIFO 

	DPWM_START_PLAY(DPWM);
}

//---------------------------------------------------------------------------
// Description:
// 		Stop APU operation for speaker 
// Argument:
// 		none
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Stop(void)
{
	DPWM_STOP_PLAY(DPWM);
}


//---------------------------------------------------------------------------
// Description:
// 		Pause APU operation to output to speaker 
// Argument:
// 		none
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Pause(UINT32 u32Channel)
{

}

//---------------------------------------------------------------------------
// Description:
// 		Resume APU operation to output to speaker 
// Argument:
// 		none
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Resume(UINT32 u32Channel)
{

}

//---------------------------------------------------------------------------
// Description:
// 		Close APU operation for speaker 
// Argument:
// 		none
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Close(void)
{

#if (APU_PDMA_ENABLE)
	DPWM_DISABLE_PDMA(DPWM);
	PdmaCtrl_Stop(APU_PDMA_CH);
	PdmaCtrl_Close(APU_PDMA_CH);
#endif
	
	CLK_DisableModuleClock(DPWM_MODULE);
	
	SYS->GPA_MFP  = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA12MFP_Msk) );
	SYS->GPA_MFP  = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA13MFP_Msk) );
}


