/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "App.h"
#include "Framework.h"
#include "ConfigSysClk.h" 

extern S_SPIFLASH_HANDLER g_sSpiFlash;

extern UINT8 SPIFlash_Initiate(void);

#define POWERDOWN_GPIO_PIN_STABLE_COUNT			(5000)

//---------------------------------------------------------------------------------------------------------
//Description: 
//	1.Make sure trigger pin stable and leaving.
//  2.Internal function for GPIOPreProc & GPIOPostProc.
//---------------------------------------------------------------------------------------------------------
void PowerDown_GPIOStableCount(UINT32 u32GPAPinMask, UINT32 u32GPBPinMask)
{	
	UINT32 u32Tmp = 0;	
	
	// GPIO stable Count.
	while( ++u32Tmp < POWERDOWN_GPIO_PIN_STABLE_COUNT )
	{
		while( ( (GPIO_GET_IN_DATA(PA)&u32GPAPinMask) != u32GPAPinMask) || 
			   ( (GPIO_GET_IN_DATA(PB)&u32GPBPinMask) != u32GPBPinMask) )
			u32Tmp = 0;
	}
	
	// Delay 100 ms to wait.
	CLK_SetSysTickClockSrc(CLK_CLKSEL0_STCLKSEL_HCLK_DIV2);
	CLK_SysTickDelay(100000);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	1.Enable/disable wake-up pin of gpio port interrupt depending on bEnable is true or false.
//	2.Internal function for GPIOPreProc & GPIOPostProc.
//---------------------------------------------------------------------------------------------------------
void PowerDown_GPIOConfigWakePin(GPIO_T *gpio, UINT32 u32PinMask, BOOL bEnable)
{	
	UINT8 u8Tmp = 0;
	
	while(u32PinMask>>u8Tmp)
	{
		if((u32PinMask>>u8Tmp)&0x01)
		{
			if(bEnable)
				GPIO_EnableInt(gpio, u8Tmp, GPIO_INT_BOTH_EDGE);
			else
				GPIO_DisableInt(gpio, u8Tmp);
		}
		u8Tmp++;
	}
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Pre-process for wake-up by GPIO process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_GPIOPreProc(void)
{			
	// If IO pin value is 0, system can't be wakeup by falling trigger, therefore wait for trigger pins value are all 1.
	PowerDown_GPIOStableCount(WAKEUP_GPIO_TRG_PA,WAKEUP_GPIO_TRG_PB);
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_HCLK, GPIO_DBCTL_DBCLKSEL_8);
	
	// set GPIO wakeup bit and GPIO QUASI mode
	#if(WAKEUP_GPIO_TRG_PA)
	GPIO_SetMode(PA, WAKEUP_GPIO_TRG_PA, GPIO_MODE_QUASI);
	GPIO_ENABLE_DEBOUNCE(PA,WAKEUP_GPIO_TRG_PA);
	PowerDown_GPIOConfigWakePin(PA,WAKEUP_GPIO_TRG_PA,TRUE);
	#endif
	#if(WAKEUP_GPIO_TRG_PB)
	GPIO_SetMode(PB, WAKEUP_GPIO_TRG_PB, GPIO_MODE_QUASI);
	GPIO_ENABLE_DEBOUNCE(PB,WAKEUP_GPIO_TRG_PB);
	PowerDown_GPIOConfigWakePin(PB,WAKEUP_GPIO_TRG_PB,TRUE);
	#endif

	//Clear GPIO pending IRQ flag, enable interrupt for wake up.
	NVIC_ClearPendingIRQ(GPAB_IRQn);
	NVIC_EnableIRQ(GPAB_IRQn);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Post-process for wake-up by GPIO process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_GPIOPostProc(void)
{		
	// Disable interrupt after wake up.
	NVIC_DisableIRQ(GPAB_IRQn);
	
	#if(WAKEUP_GPIO_TRG_PA)
	PowerDown_GPIOConfigWakePin(PA,WAKEUP_GPIO_TRG_PA,FALSE);
	#endif
	#if(WAKEUP_GPIO_TRG_PB)
	PowerDown_GPIOConfigWakePin(PB,WAKEUP_GPIO_TRG_PB,FALSE);
	#endif
	
	PowerDown_GPIOStableCount(WAKEUP_GPIO_TRG_PA,WAKEUP_GPIO_TRG_PB);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Pre-process for wake-up by RTC process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_RTCPreProc(void)
{	
	S_RTC_TIME_DATA_T sTime;
	
	sTime.u32Year = RTC_SYSTEM_TIME_YEAR;
	sTime.u32Month = RTC_SYSTEM_TIME_MONTH;
	sTime.u32Day = RTC_SYSTEM_TIME_DAY;
	sTime.u32DayOfWeek = RTC_SYSTEM_TIME_DAYOFWEEK;
	sTime.u32Hour = RTC_SYSTEM_TIME_HOUR;
	sTime.u32Minute = RTC_SYSTEM_TIME_MINUTE;
	sTime.u32Second = RTC_SYSTEM_TIME_SECOND;
	sTime.u32TimeScale = RTC_CLOCK_24;
	/* Initiate RTC & setting configuration */
	CLK_EnableModuleClock(RTC_MODULE);
	CLK_SetModuleClock(RTC_MODULE,MODULE_NoMsk,MODULE_NoMsk);	
	RTC_Open(&sTime);

	sTime.u32Year = RTC_SYSTEM_TIME_YEAR+WAKEUP_RTC_TRG_TIME_YEAR;
	sTime.u32Month = RTC_SYSTEM_TIME_MONTH+WAKEUP_RTC_TRG_TIME_MONTH;
	sTime.u32Day = RTC_SYSTEM_TIME_DAY+WAKEUP_RTC_TRG_TIME_DAY;
	sTime.u32Hour = RTC_SYSTEM_TIME_HOUR+WAKEUP_RTC_TRG_TIME_HOUR;
	sTime.u32Minute = RTC_SYSTEM_TIME_MINUTE+WAKEUP_RTC_TRG_TIME_MINUTE;
	sTime.u32Second = RTC_SYSTEM_TIME_SECOND+WAKEUP_RTC_TRG_TIME_SECOND;
    /* Enable RTC Alarm Interrupt */
	RTC_SetAlarmDateAndTime(&sTime);
    RTC_EnableInt(RTC_INTEN_ALMIEN_Msk);
    NVIC_EnableIRQ(RTC_IRQn);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Post-process for wake-up by RTC process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_RTCPostProc(void)
{
	// Disable RTC interrupt and clock.
	NVIC_DisableIRQ(RTC_IRQn);
	RTC_DisableInt(RTC_INTEN_ALMIEN_Msk);
	RTC_Close();			  					
	CLK_DisableModuleClock(RTC_MODULE);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Pre-process for wake-up by WDT process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_WDTPreProc(void)
{
	CLK_EnableModuleClock(WDT_MODULE);
	// Use RC OSC(max speed) to setting.
	CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_HIRC, MODULE_NoMsk); 	
	WDT_RESET_COUNTER();
	WDT_CLEAR_TIMEOUT_INT_FLAG();
 	WDT_Open(WAKEUP_WDT_TRG_TIMEOUT, 0);
	WDT_EnableInt();
	NVIC_EnableIRQ(WDT_IRQn);
	CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, MODULE_NoMsk);
}

//---------------------------------------------------------------------------------------------------------
//Description: 
//	Post-process for wake-up by WDT process.
//---------------------------------------------------------------------------------------------------------
void PowerDown_WDTPostProc(void)
{
	NVIC_DisableIRQ(WDT_IRQn);
	WDT_DisableInt();
	WDT_Close();
	CLK_DisableModuleClock(WDT_MODULE);
}

//---------------------------------------------------------------------------------------------------------
// Function: PowerDown_Enter
//
// Description:                                                                                            
//   Pre-process befor entering power-down .                                 
//---------------------------------------------------------------------------------------------------------
void PowerDown_Enter(void)
{
	// Add code for close IP function
	
#if( !defined(SPIFLASH_NONE) )
	SPIFlash_PowerDown(&g_sSpiFlash,1);
	SPIFlash_Close(&g_sSpiFlash);
	#if (defined(SPIFLASH_SPI1_DEVICE1))
	CLK_DisableModuleClock(SPI1_MODULE);
	#elif (defined(SPIFLASH_SPI0_DEVICE1))
	CLK_DisableModuleClock(SPI0_MODULE);
	#endif
#endif
	
	SPK_UNINITIATE();
	MIC_UNINITIATE();
	PDMA_UNINITIATE();
	TRIGGER_KEY_RESET();
	MATRIX_KEY_RESET();
	NVIC_DisableIRQ(TMR1_IRQn);
	

	// LED and key GPIO change output and driver hgih.
	GPIO_SetMode(PA, MATRIX_PORTA_INPUT_PINS_MASK|MATRIX_PORTA_OUTPUT_PINS_MASK|TRIGGER_PORTA_PINS_MASK|OUTPUT_PORTA_PINS_MASK, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PB, MATRIX_PORTB_INPUT_PINS_MASK|MATRIX_PORTB_OUTPUT_PINS_MASK|TRIGGER_PORTB_PINS_MASK|OUTPUT_PORTB_PINS_MASK, GPIO_MODE_OUTPUT);
	GPIO_SET_OUT_DATA(PA, GPIO_GET_OUT_DATA(PA)|(MATRIX_PORTA_INPUT_PINS_MASK|MATRIX_PORTA_OUTPUT_PINS_MASK|TRIGGER_PORTA_PINS_MASK|OUTPUT_PORTA_PINS_MASK));
	GPIO_SET_OUT_DATA(PB, GPIO_GET_OUT_DATA(PB)|(MATRIX_PORTB_INPUT_PINS_MASK|MATRIX_PORTB_OUTPUT_PINS_MASK|TRIGGER_PORTB_PINS_MASK|OUTPUT_PORTB_PINS_MASK));

	SYS->GPA_MFP = 0;
	SYS->GPB_MFP = 0;
	
	#if(WAKEUP_GPIO_TRIGGER)
	PowerDown_GPIOPreProc();
	#endif
	
	#if(WAKEUP_RTC_TRIGGER)
	PowerDown_RTCPreProc();
	#endif
	
	#if(WAKEUP_WDT_TRIGGER)
	PowerDown_WDTPreProc();
	#endif
	
	CLK_DisableLDO();

}

//---------------------------------------------------------------------------------------------------------
// Function: PowerDown_Exit
//
// Description:                                                                                            
//   Post-process after exiting power-down.                             
//---------------------------------------------------------------------------------------------------------
void PowerDown_Exit(void)
{	
	// Add code for open IP function
	CLK_EnableLDO(CLK_LDOSEL_3_3V);
	
#if(WAKEUP_WDT_TRIGGER)
	PowerDown_WDTPostProc();
#endif
	
#if(WAKEUP_RTC_TRIGGER)
	PowerDown_RTCPostProc();
#endif
	
#if(WAKEUP_GPIO_TRIGGER)
	PowerDown_GPIOPostProc();
#endif	
	
	OUTPUTPIN_INITIATE();
	KEYPAD_INITIATE();
	ULTRAIO_INITIATE();
	PDMA_INITIATE();
	SPK_INITIATE();
	MIC_INITIATE();

	
#if( !defined(SPIFLASH_NONE) )
	SPIFlash_Initiate();	
#endif

	// Light stand by(PB8) led for initial ready().
	OUT3(0);
	
}

void PowerDown(void)
{
	
	CLK_DeepSleep();
	__wfi();
	__isb(0);

}
