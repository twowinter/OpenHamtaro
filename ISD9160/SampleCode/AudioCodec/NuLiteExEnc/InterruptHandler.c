/*---------------------------------------------------------------------------------------------------------*/
/*																										   */
/* Copyright (c) Nuvoton Technology	Corp. All rights reserved.											   */
/*																										   */
/*---------------------------------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- TimeF ISR: handle de-bounce time for direct trigger key and matrix key.
//		- Time2 ISR: handle UltraIO curve output with F/W PWM mode.
//		- GPIOAB ISR: handle wake up by GPIOA or GPIO B.
//		- Other interrupt ISRs.
//
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------

#include "Framework.h"
#include "Keypad.h"

#if (APU_ENABLE)
#define DAC_BUFF_SIZE (4*2)
INT16 g_ai16DACSamples[DAC_BUFF_SIZE];
PINT16 Playback_Process(PINT16 pi16Buff);
#endif

#if (ADC_ENABLE)
#define ADC_BUFF_SIZE (4*2)
INT16 g_ai16ADCSamples[ADC_BUFF_SIZE];
void Record_Process(PINT16 pi16Buff);
#endif


//---------------------------------------------------------------------------------------------------------
// PDMA interrupt handler                                                 
//---------------------------------------------------------------------------------------------------------
void PDMA_IRQHandler()
{
#if ((APU_ENABLE) && (APU_PDMA_ENABLE))
	if (PDMA_GET_CH_INT_STS(APU_PDMA_CH)&PDMA_CHIF_TXOKIF_Msk)
	{
		PDMA_CLR_CH_INT_FLAG(APU_PDMA_CH,PDMA_CHIF_TXOKIF_Msk );											
		Playback_Process(g_ai16DACSamples);	
	}
#endif
	
#if ((ADC_ENABLE) && (ADC_PDMA_ENABLE))
	if (PDMA_GET_CH_INT_STS(ADC_PDMA_CH)&PDMA_CHIF_TXOKIF_Msk)
	{
		PDMA_CLR_CH_INT_FLAG(ADC_PDMA_CH,PDMA_CHIF_TXOKIF_Msk );
		Record_Process(g_ai16ADCSamples);	
	}
#endif
}

//---------------------------------------------------------------------------------------------------------
// CAP interrupt handler                                                 
//---------------------------------------------------------------------------------------------------------
void CAPS_IRQHandler()
{
	TOUCH_KEY_CAP();
	CAPSENSE_DISABLE_INTERRUPT();
}

//---------------------------------------------------------------------------------------------------------
// Timer0 interrupt handler                                                 
//---------------------------------------------------------------------------------------------------------
void TMR0_IRQHandler()
{
	TIMER_ClearIntFlag(TIMER0);

	ULTRAIO_OUTCURVETMR0();
}

//---------------------------------------------------------------------------------------------------------
// Timer1 interrupt handler                                                 
//---------------------------------------------------------------------------------------------------------
void TMR1_IRQHandler()
{
	TIMER_ClearIntFlag(TIMER1);
	
	TRIGGER_KEY_DEBOUNCE();
	MATRIX_KEY_DEBOUNCE();
	TOUCH_KEY_PRESS_COUNT();
	
	ULTRAIO_OUTCURVETMR1();
}

//---------------------------------------------------------------------------------------------------------
// GPIO interrupt handler.
//---------------------------------------------------------------------------------------------------------
void GPAB_IRQHandler()
{
	#if(WAKEUP_GPIO_TRG_PA)
	GPIO_CLR_INT_FLAG( PA, GPIO_GET_INT_FLAG( PA, WAKEUP_GPIO_TRG_PA ) );
	#endif
	#if(WAKEUP_GPIO_TRG_PB)
	GPIO_CLR_INT_FLAG( PB, GPIO_GET_INT_FLAG( PB, WAKEUP_GPIO_TRG_PB ) );
	#endif

	NVIC_ClearPendingIRQ(GPAB_IRQn);
}

//---------------------------------------------------------------------------------------------------------
// RTC interrupt handler.
//---------------------------------------------------------------------------------------------------------
void RTC_IRQHandler()
{
//	RTC_CLEAR_INT_FLAG();
}

//---------------------------------------------------------------------------------------------------------
// WDT interrupt handler.
//---------------------------------------------------------------------------------------------------------
void WDT_IRQHandler()
{
//	WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();	
//	WDT_CLEAR_RESET_FLAG();
}
