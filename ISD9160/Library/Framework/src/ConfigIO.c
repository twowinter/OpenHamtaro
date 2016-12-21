/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Include related header files                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "Platform.h"
#include "ConfigApp.h"
#include "Keypad.h"
#include "ConfigIO.h"
#include "ConfigIP.h"

DECLARE_MATRIX_KEY()

DECLARE_TRIGGER_KEY()

DECLARE_MATRIX_KEY_BUF()

DECLARE_TOUCH_KEY() 			

DECLARE_TOUCH_KEY_BUF()

DECLARE_TOUCH_THRESHOLD()

void InputKey_Change(UINT32 u32InputStateIndex)
{
	#if (TRIGGER_KEY_COUNT>0)
	Keypad_InitTgr( TRIGGER_PORTA_PINS_MASK, TRIGGER_PORTB_PINS_MASK,&g_asTriggerKeyHandler[g_au8TriggerKeyStateIndex[u32InputStateIndex]]); 
	#endif

	#if (MATRIX_KEY_COUNT>0)
	Keypad_InitKey( 							
		MATRIX_PORTA_INPUT_PINS_MASK, MATRIX_PORTB_INPUT_PINS_MASK, 						
		MATRIX_PORTA_OUTPUT_PINS_MASK, MATRIX_PORTB_OUTPUT_PINS_MASK, 						
		&g_asMatrixKeyHandler[g_au8MatrixKeyStateIndex[u32InputStateIndex]], &g_u16aKeyPinValueBuf[0] );						
	#endif 
	
	#if (TOUCH_KEY_COUNT>0)
	SYS->GPB_MFP  = (SYS->GPB_MFP & (~TOUCH_CLR_CAP_MASK) ) | TOUCH_CFG_CAP_MASK;
	
	CLK_EnableModuleClock(ANA_MODULE);	
	CLK_EnableModuleClock(ACMP_MODULE);	
	
	NVIC_EnableIRQ(CAPS_IRQn);
	
	Keypad_InitTouch( 
		TOUCH_KEY_PINS_MASK, 
		KEYPAD_TOUCH_THRESHOLD,
		&g_asTouchKeyHandler[g_au8TouchKeyStateIndex[u32InputStateIndex]], &g_u16aTouchPinValueBuf[0] );		
	#endif
}

void InputKey_Initiate(void)
{
	// GPIO pin input&output initiate.
	GPIO_SetMode(PA, (MATRIX_PORTA_INPUT_PINS_MASK|TRIGGER_PORTA_PINS_MASK), GPIO_MODE_QUASI);	
    GPIO_SetMode(PA, MATRIX_PORTA_OUTPUT_PINS_MASK, GPIO_MODE_OUTPUT);	
	GPIO_SetMode(PB, (MATRIX_PORTB_INPUT_PINS_MASK|TRIGGER_PORTB_PINS_MASK), GPIO_MODE_QUASI);		
	GPIO_SetMode(PB, MATRIX_PORTB_OUTPUT_PINS_MASK, GPIO_MODE_OUTPUT);	
	
	// In ISD9100, "UltraIO firmware curve" & "Press count of keypad" are used the same timer.
	if( ULTRAIO_FW_CURVE_ENABLE == 0 )
	{
		CLK_EnableModuleClock(TMR1_MODULE);
		CLK_SetModuleClock(TMR1_MODULE,TIMER1_CLK_SOURCE,NULL);
		TIMER_Open( TIMER1, TIMER_PERIODIC_MODE, TIMER1_FREQUENCY);
		TIMER_EnableInt(TIMER1);
		NVIC_EnableIRQ(TMR1_IRQn);	
		TIMER_Start(TIMER1);
	}

	// Change debounce time unit to second.
	Keypad_InitKeypad(KEY_DEBOUNCE_TIME*TIMER_GetWorkingFreq(TIMER1),KEY_PRESS_TIME*TIMER_GetWorkingFreq(TIMER1));
	// Default input state.
	InputKey_Change(0);	
}

void OutputPin_Initiate(void)
{
	GPIO_SetMode(PA, OUTPUT_PORTA_PINS_MASK,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PB, OUTPUT_PORTB_PINS_MASK,GPIO_MODE_OUTPUT);
}

void OutputPin_Set(GPIO_T *pPort, UINT16 u16PinMask, UINT8 u8Value)
{
	if (u8Value==1)
		GPIO_SET_OUT_DATA(pPort,(GPIO_GET_OUT_DATA(pPort)|u16PinMask));
	else
		GPIO_SET_OUT_DATA(pPort,(GPIO_GET_OUT_DATA(pPort)&~u16PinMask));
}
