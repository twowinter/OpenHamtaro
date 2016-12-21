#include <stdio.h>
#include "ISD9xx.h"
#include "Driver\DrvPDMA.h"
#include "Driver\DrvADC.h"
#include "base_types.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void InitialADC(void);
void InitialADC_Remote(void);
void PDMA0_Callback(void);
void PDMA0forMIC(uint32_t u32DestAddr);

/*---------------------------------------------------------------------------------------------------------*/
/* Define macro & global variables                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define AUDIOBUFFERSIZE		160//320//

__align(4) int16_t MicBuffer[2][AUDIOBUFFERSIZE];

uint32_t g_nCallbackCounter = 0;
__align(4) INT g_bBlockReady[2] = {0,0};
__align(4) INT g_bCpuOverload = 0;
__align(4) INT g_bRecordStart = 0;

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function									                                           			   */
/*---------------------------------------------------------------------------------------------------------*/
void Sleep(uint32_t ms)
{
	INT n = g_nCallbackCounter + (ms/10);
	while (g_nCallbackCounter < n);
}

void Wave_Initial(void)
{
//  printf("+-------------------------------------------------------------------------+\n");
//  printf("|       Recroding Start												      |\n");
//  printf("+-------------------------------------------------------------------------+\n");

//	InitialADC();	  		//ADC initialization
	InitialADC_Remote();
  DrvPDMA_Init();			//PDMA initialization
	UNLOCKREG();

	g_nCallbackCounter = 0;
	g_bBlockReady[0] = g_bBlockReady[1] = 0;
	g_bCpuOverload = 0;

	PDMA0forMIC((uint32_t)&MicBuffer[0][0]);
}

void Wave_Close(void)
{
	//DrvPDMA_Close();
	DrvPDMA_DisableInt(eDRVPDMA_CHANNEL_0, eDRVPDMA_WAR);		//Disable INT
 	DrvADC_Close();
	UNLOCKREG();
}

void Wave_StartRecord(void)
{
	g_bBlockReady[0] = g_bBlockReady[1] = 0;
	g_bCpuOverload = 0;
	g_bRecordStart = 1;
}

void Wave_StopRecord(void)
{
	g_bRecordStart = 0;
}

INT Wave_GetSample(SHORT **lppsSample)
{
	while (g_bRecordStart)
	{
		if (g_bBlockReady[0])
		{
			*lppsSample = (SHORT *)MicBuffer[0];
			return AUDIOBUFFERSIZE;
		}
		else if (g_bBlockReady[1])
		{
			*lppsSample = (SHORT *)MicBuffer[1];
			return AUDIOBUFFERSIZE;
		}
		return 0;
	}
	return -1;
}

INT Wave_UnlockSample(SHORT **lppsSample)
{
		if (*lppsSample == (SHORT *)MicBuffer[0])
			g_bBlockReady[0] = 0;
		if (*lppsSample == (SHORT *)MicBuffer[1])
			g_bBlockReady[1] = 0;
		*lppsSample = NULL;
		return 0;
}

INT Wave_CheckCpuOverload(void)
{
	return g_bCpuOverload;
}

void Wave_ClearCpuOverload(void)
{
	g_bCpuOverload = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* ADC RX Callback                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void PDMA0_Callback()
{
	g_nCallbackCounter++;
	if (g_bRecordStart)
	{
		if ((g_nCallbackCounter & 1) == 1)
		{
			g_bBlockReady[0] = 1;
			if (g_bBlockReady[1])
				g_bCpuOverload = 1;
		}
		else
		{
			g_bBlockReady[1] = 1;
			if (g_bBlockReady[0])
				g_bCpuOverload = 1;
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* InitialADC                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void InitialADC(void)
{
	S_DRVADC_PARAM sParam;
	uint32_t u32AdcStatus;
	uint32_t OSR;

	// b0,b1,b2,a1,a2,b0,b1,b2,a1,a2,b0,b1,b2,a1,a2
	/*
	uint32_t u32BiqCoeff[15]={0x10000, 0x15b8a, 0x10000, 0x15068, 0x0ef98,
							  0x10000, 0x00000,	0x00000, 0x00000, 0x00000,
							  0x10000, 0x00000,	0x00000, 0x00000, 0x00000};
	*/

	/* Open Analog block */
	//DrvADC_AnaOpen();				//Will reset all IP in ANA_BASE

	/* Power control */
	DrvADC_SetPower( 
		eDRVADC_PU_MOD_ON, 
		eDRVADC_PU_IBGEN_ON, 
		eDRVADC_PU_BUFADC_ON, 
		eDRVADC_PU_BUFPGA_ON, 
		//eDRVADC_PU_ZCD_OFF);	   //ALC off
		eDRVADC_PU_ZCD_ON);	   //ALC on

	/* PGA Setting */	
	DrvADC_PGAMute(eDRVADC_MUTE_PGA);
	DrvADC_PGAUnMute(eDRVADC_MUTE_IPBOOST);
	DrvADC_SetPGA(
		eDRVADC_REF_SEL_VMID,
		eDRVADC_PU_PGA_ON,
		eDRVADC_PU_BOOST_ON,
		eDRVADC_BOOSTGAIN_0DB);			//減少周圍噪音
		//eDRVADC_BOOSTGAIN_26DB);

	DrvADC_SetPGAGaindB(1000);	//聲音比較柔和
//	DrvADC_SetPGAGaindB(-600);		//聲音銳利

	/* MIC circuit configuration */
	DrvADC_SetVMID(
		eDRVADC_PULLDOWN_VMID_RELEASE,
		eDRVADC_PDLORES_CONNECTED,
		eDRVADC_PDHIRES_DISCONNECTED);
	DrvADC_SetMIC(TRUE, eDRVADC_MIC_BIAS_65_VCCA);
//	DrvADC_SetMIC(TRUE, eDRVADC_MIC_BIAS_SEL_2);

	/* ALC Setting */	
	SYSCLK->APBCLK.BIQALC_EN = 1;
	ALC->ALC_CTRL.ALCLVL = 15;
	//ALC->ALC_CTRL.ALCSEL = 1;		//ALC enable
	ALC->ALC_CTRL.ALCSEL = 0;		//ALC disable
	ALC->ALC_CTRL.ALCDCY = 3;
	ALC->ALC_CTRL.ALCATK = 2;
	ALC->ALC_CTRL.NGEN = 1;
	ALC->ALC_CTRL.NGTH = 6;

	//BIQ->BIQ_CTRL.RSTn = 1;
	//(*((volatile unsigned long*)0x400B0048)) = 0xFF01E360; // target level

	/* Change BIQ Setting */			
	//SYS->IPRSTC2.BIQ_RST = 1;
	//SYS->IPRSTC2.BIQ_RST = 0;
	//BIQ->BIQ_CTRL.SELPWM = 1;		//Set BIQ for DPWM
	//DrvADC_SetBIQ(1023, 1, eDRVADC_BIQ_IN_ADC, u32BiqCoeff);
	//BIQ->BIQ_CTRL.EN = 1;
	//BIQ->BIQ_CTRL.RSTn = 1;

	/* Open ADC block */
	sParam.u8AdcDivisor   = 0;
	//sParam.u8SDAdcDivisor = 16;	//OSR64  :16 for 48K
	//sParam.u8SDAdcDivisor = 16;	//OSR128 :48 for 8K, 24 for 16K
	sParam.u8SDAdcDivisor = 16;		//OSR192 :32 for 8K, 16 for 16K
	//sParam.eOSR		  = eDRVADC_OSR_128;
	sParam.eOSR		  = eDRVADC_OSR_192;
	//sParam.eOSR		  = eDRVADC_OSR_64;
	sParam.eInputSrc  = eDRVADC_MIC;
	sParam.eInputMode = eDRVADC_DIFFERENTIAL;
	sParam.u8ADCFifoIntLevel = 7;
	u32AdcStatus=DrvADC_Open(&sParam);
	if(u32AdcStatus == E_SUCCESS) {
//		printf("ADC has been successfully opened.\n");
//		printf("ADC clock divisor=%d\n",SYSCLK->CLKDIV.ADC_N);
//		printf("ADC over sampling clock divisor=%d\n",SDADC->CLK_DIV);
		switch(SDADC->DEC.OSR)
		{
		  case eDRVADC_OSR_64:OSR=64;break;
		  case eDRVADC_OSR_128:OSR=128;break;
		  case eDRVADC_OSR_192:OSR=192;break;
		  case eDRVADC_OSR_384:OSR=384;break;
		}
//		printf("ADC over sampling ratio=%d\n", OSR);
//		printf("Select microphone path as differential input\n");
//		printf("ADC Fifo Interrupt Level=%d\n", SDADC->INT.FIFO_IE_LEV);
//		printf("Conversion rate: %d samples/second\n", DrvADC_GetConversionRate());
	}
	else {
//		printf("ADC Open failed!\n");
	}
	/* Change Decimation and FIFO Setting */
	//DrvADC_SetAdcOverSamplingClockDivisor(u8SDAdcDivisor);
	//DrvADC_SetOverSamplingRatio(eOSR);
	//DrvADC_SetCICGain(u8CICGain);
	//DrvADC_SetFIFOIntLevel(u8ADCFifoIntLevel);

	DrvADC_PGAUnMute(eDRVADC_MUTE_PGA);
}

void InitialADC_Remote(void)
{
	S_DRVADC_PARAM sParam;
	uint32_t u32AdcStatus;
	uint32_t OSR;

	// b0,b1,b2,a1,a2,b0,b1,b2,a1,a2,b0,b1,b2,a1,a2
	/*
	uint32_t u32BiqCoeff[15]={0x10000, 0x15b8a, 0x10000, 0x15068, 0x0ef98,
							  0x10000, 0x00000,	0x00000, 0x00000, 0x00000,
							  0x10000, 0x00000,	0x00000, 0x00000, 0x00000};
	*/

	/* Open Analog block */
	//DrvADC_AnaOpen();

	/* Power control */
	DrvADC_SetPower( 
		eDRVADC_PU_MOD_ON, 
		eDRVADC_PU_IBGEN_ON, 
		eDRVADC_PU_BUFADC_ON, 
		eDRVADC_PU_BUFPGA_ON, 
		//eDRVADC_PU_ZCD_OFF);	   //ALC off
		eDRVADC_PU_ZCD_ON);	   //ALC on

	/* PGA Setting */	
	DrvADC_PGAMute(eDRVADC_MUTE_PGA);
	DrvADC_PGAUnMute(eDRVADC_MUTE_IPBOOST);
	DrvADC_SetPGA(	
	    eDRVADC_REF_SEL_VMID,
	    eDRVADC_PU_PGA_ON,
	    eDRVADC_PU_BOOST_ON,
	    eDRVADC_BOOSTGAIN_0DB); //減少周圍噪音
//		eDRVADC_BOOSTGAIN_26DB);

//	DrvADC_SetPGAGaindB(1000);  //聲音比較柔和
//	DrvADC_SetPGAGaindB(-600);	//聲音銳利
	DrvADC_SetPGAGaindB(3525);  //

	/* MIC circuit configuration */
	DrvADC_SetVMID(
		eDRVADC_PULLDOWN_VMID_RELEASE,
		eDRVADC_PDLORES_CONNECTED,
		eDRVADC_PDHIRES_DISCONNECTED);

//	DrvADC_SetMIC(TRUE, eDRVADC_MIC_BIAS_65_VCCA);
//	DrvADC_SetMIC(TRUE, eDRVADC_MIC_BIAS_SEL_2);
	DrvADC_SetMIC(TRUE, eDRVADC_MIC_BIAS_SEL_0);

	/* ALC Setting */	
	SYSCLK->APBCLK.BIQALC_EN = 1;
	ALC->ALC_CTRL.ALCLVL = 14;
	ALC->ALC_CTRL.ALCSEL =1;		 //ALC enable
	//ALC->ALC_CTRL.ALCSEL =0;		//ALC disable
	ALC->ALC_CTRL.ALCDCY = 3;
	ALC->ALC_CTRL.ALCATK = 2;
	ALC->ALC_CTRL.NGEN = 1;
	//ALC->ALC_CTRL.NGTH =6;
	ALC->ALC_CTRL.NGTH =7;

	//BIQ->BIQ_CTRL.RSTn = 1;
	//(*((volatile unsigned long*)0x400B0048)) = 0xFF01E360; // target level

	/* Open ADC block */
	sParam.u8AdcDivisor   = 0;
	//sParam.u8SDAdcDivisor = 16;	//OSR64  :16 for 48K
	//sParam.u8SDAdcDivisor = 16;	//OSR128 :48 for 8K, 24 for 16K
	sParam.u8SDAdcDivisor = 16;//32;		//OSR192 :32 for 8K, 16 for 16K
	//sParam.eOSR		  = eDRVADC_OSR_128;
	sParam.eOSR		  = eDRVADC_OSR_192;
	//sParam.eOSR		  = eDRVADC_OSR_64;
	sParam.eInputSrc  = eDRVADC_MIC;
	sParam.eInputMode = eDRVADC_DIFFERENTIAL;
	sParam.u8ADCFifoIntLevel = 7;
	u32AdcStatus=DrvADC_Open(&sParam);
	if(u32AdcStatus == E_SUCCESS) {
//		printf("ADC has been successfully opened.\n");
//		printf("ADC clock divisor=%d\n",SYSCLK->CLKDIV.ADC_N);
//		printf("ADC over sampling clock divisor=%d\n",SDADC->CLK_DIV);
		switch(SDADC->DEC.OSR)
		{
		  case eDRVADC_OSR_64:OSR=64;break;
		  case eDRVADC_OSR_128:OSR=128;break;
		  case eDRVADC_OSR_192:OSR=192;break;
		  case eDRVADC_OSR_384:OSR=384;break;
		}
//		printf("ADC over sampling ratio=%d\n", OSR);
//		printf("Select microphone path as differential input\n");
//		printf("ADC Fifo Interrupt Level=%d\n", SDADC->INT.FIFO_IE_LEV);
//		printf("Conversion rate: %d samples/second\n", DrvADC_GetConversionRate());
	}
	else {
//		printf("ADC Open failed!\n");
	}
	/* Change Decimation and FIFO Setting */
	//DrvADC_SetAdcOverSamplingClockDivisor(u8SDAdcDivisor);
	//DrvADC_SetOverSamplingRatio(eOSR);
	//DrvADC_SetCICGain(u8CICGain);
	//DrvADC_SetFIFOIntLevel(u8ADCFifoIntLevel);

	DrvADC_PGAUnMute(eDRVADC_MUTE_PGA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Set PDMA0 to move ADC FIFO to MIC buffer with wrapped-around mode                                       */
/*---------------------------------------------------------------------------------------------------------*/
void PDMA0forMIC(uint32_t u32DestAddr)
{
	STR_PDMA_T sPDMA;  

	sPDMA.sSrcAddr.u32Addr 			= (uint32_t)&SDADC->ADCOUT; 
	sPDMA.sDestAddr.u32Addr 		= u32DestAddr;
	sPDMA.u8Mode 					= eDRVPDMA_MODE_APB2MEM;
	sPDMA.u8TransWidth 				= eDRVPDMA_WIDTH_16BITS;
	sPDMA.sSrcAddr.eAddrDirection 	= eDRVPDMA_DIRECTION_FIXED; 
	sPDMA.sDestAddr.eAddrDirection 	= eDRVPDMA_DIRECTION_WRAPAROUND;  
	sPDMA.u8WrapBcr				 	= eDRVPDMA_WRA_WRAP_HALF_INT;	//Interrupt condition set fro Half buffer & buffer end
	sPDMA.i32ByteCnt = AUDIOBUFFERSIZE * 4;	   	//Full MIC buffer length (byte)
	DrvPDMA_Open(eDRVPDMA_CHANNEL_0, &sPDMA);

	// PDMA Setting 
	//PDMA_GCR->PDSSR.ADC_RXSEL = eDRVPDMA_CHANNEL_2;
	DrvPDMA_SetCHForAPBDevice(
    	eDRVPDMA_CHANNEL_0, 
    	eDRVPDMA_ADC,
    	eDRVPDMA_READ_APB    
	);

	DrvPDMA_EnableInt(eDRVPDMA_CHANNEL_0, eDRVPDMA_WAR );		//Enable INT    
	DrvPDMA_InstallCallBack(eDRVPDMA_CHANNEL_0, eDRVPDMA_WAR, (PFN_DRVPDMA_CALLBACK) PDMA0_Callback );		//Callback registration
	DrvADC_PdmaEnable();		// Enable ADC PDMA and Trigger PDMA specified Channel 
	DrvADC_StartConvert();		 // Start A/D conversion 
	DrvPDMA_CHEnablelTransfer(eDRVPDMA_CHANNEL_0);		// start ADC PDMA transfer
}
