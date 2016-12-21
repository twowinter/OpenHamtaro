/**************************************************************************//**
 * @file     UltraIO.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 14/09/10 02:00p $
 * @brief    Source file of UltraIO control. 
 *
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <string.h>
#include "Framework.h"
#include "UltraIO.h"
#include "ConfigUltraIO.h"

ULTRAIO_CURVE_MAP()

ULTRAIO_CURVE_SETTINGS()

ULTRAIO_CURVE_TO_PORTA_PINS()

ULTRAIO_CURVE_TO_PORTB_PINS() 

#if(ULTRAIO_USE_STRUCTURE)
	
uint8_t g_u8UltraIOCurveValue[ULTRAIO_CURVE_MAX_NUM] = { 0 };

uint8_t g_u8UltraIOFwPwmCounter = 0;

uint32_t g_uiUltraIOCtrl = 0xFFFFFFFF;

S_ULTRAIO_CONFIG g_sUltraIO_Config;

#else

ULTRAIO_CURVE_CTRL()
	
ULTRAIO_CURVE_VALUE() 
	
ULTRAIO_FW_PWM_COUNTER()

#endif

#if (ULTRAIO_USE_STRUCTURE)
/**
  * @brief  Set Configs from bin file. 
  * @param  psConfig is config data(256bytes) from bin file
**/
void UltraIO_SetConfig(S_ULTRAIO_CONFIG* psConfig)
{
	memcpy( &g_sUltraIO_Config, psConfig, sizeof(S_ULTRAIO_CONFIG) );
}
#endif

/**
  * @brief  Initiate UltraIO setting.
**/
void UltraIO_Initiate(void)
{
	#if (ULTRAIO_USE_STRUCTURE) 

		if( g_sUltraIO_Config.u16PortAPin )
			GPIO_SetMode(PA,g_sUltraIO_Config.u16PortAPin,GPIO_MODE_OUTPUT);

		if( g_sUltraIO_Config.u16PortBPin )
			GPIO_SetMode(PA,g_sUltraIO_Config.u16PortBPin,GPIO_MODE_OUTPUT);

		if ( ULTRAIO_FW_CURVE_ENABLE && ( g_sUltraIO_Config.u8FwCurveCount > 0 ) )
		{
			CLK_EnableModuleClock(ULTRAIO_CLK_MODULE);
			CLK_SetModuleClock(ULTRAIO_CLK_MODULE,ULTRAIO_CLK_SOURCE,NULL);
			SYS_ResetModule(ULTRAIO_SYS_RST);
			TIMER_Open(ULTRAIO_TMR_HANDLER,TIMER_PERIODIC_MODE,g_sUltraIO_Config.u8MaxResoluation*g_sUltraIO_Config.u8PwmFrequency);
			TIMER_EnableInt(ULTRAIO_TMR_HANDLER);
			NVIC_EnableIRQ(ULTRAIO_TMR_IRQ);
			TIMER_Start(ULTRAIO_TMR_HANDLER);
		}
		
		if( ULTRAIO_HW_CURVE_ENABLE && ( g_sUltraIO_Config.u16HwPwmEnable > 0 ))
        {
			CLK_EnableModuleClock(PWM0_MODULE);
			CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0CH01SEL_HCLK, 0);
			SYS_ResetModule(PWM0_RST);
			UltraIO_EnablePWMChannel( g_sUltraIO_Config.u16HwPwmEnable );
		
			if( g_sUltraIO_Config.u16HwPwmEnable&BIT0 )
				PWM_ConfigOutputChannel(PWM0, PWM_CH0, g_sUltraIO_Config.u8MaxResoluation*g_sUltraIO_Config.u8PwmFrequency, 0);
			if( g_sUltraIO_Config.u16HwPwmEnable&BIT1 )
				PWM_ConfigOutputChannel(PWM0, PWM_CH1, g_sUltraIO_Config.u8MaxResoluation*g_sUltraIO_Config.u8PwmFrequency, 0);
        }
		
	#else
		
		#if ( ULTRAIO_USE_PORTA )
		// Set GPIOA pin's input/output mode. 
		GPIO_SetMode(PA,ULTRAIO_INITIATE_PORTA_PIN,GPIO_MODE_OUTPUT);
		#endif
	
		#if ( ULTRAIO_USE_PORTB )
		// Set GPIOB pin's output mode. 
	    GPIO_SetMode(PB,ULTRAIO_INITIATE_PORTB_PIN,GPIO_MODE_OUTPUT);
		#endif
	
		#if ( ULTRAIO_FW_CURVE_ENABLE && ( ULTRAIO_FW_CURVE_COUNT > 0 ) )
		// Set use timer(Timer0/Timer1/Timer2)
		CLK_EnableModuleClock(ULTRAIO_CLK_MODULE);
		CLK_SetModuleClock(ULTRAIO_CLK_MODULE,ULTRAIO_CLK_SOURCE,NULL);
		SYS_ResetModule(ULTRAIO_SYS_RST);
		TIMER_Open(ULTRAIO_TMR_HANDLER,TIMER_PERIODIC_MODE,ULTRAIO_MAX_RESOLUATION*ULTRAIO_FREQUENCY);
		TIMER_EnableInt(ULTRAIO_TMR_HANDLER);
		NVIC_EnableIRQ(ULTRAIO_TMR_IRQ);
		TIMER_Start(ULTRAIO_TMR_HANDLER);
		#endif
	
		#if ( ULTRAIO_HW_CURVE_ENABLE && ( ULTRAIO_INITIATE_HW_PWM_ENABLED > 0 ) )
		CLK_EnableModuleClock(PWM0_MODULE);
		CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0CH01SEL_HCLK, 0);
		SYS_ResetModule(PWM0_RST);
		UltraIO_EnablePWMChannel(ULTRAIO_INITIATE_HW_PWM_ENABLED);
		
		if( ULTRAIO_INITIATE_HW_PWM_ENABLED&BIT0 )
			PWM_ConfigOutputChannel(PWM0, PWM_CH0, ULTRAIO_MAX_RESOLUATION*ULTRAIO_FREQUENCY, 0);
		if( ULTRAIO_INITIATE_HW_PWM_ENABLED&BIT1 )
			PWM_ConfigOutputChannel(PWM0, PWM_CH1, ULTRAIO_MAX_RESOLUATION*ULTRAIO_FREQUENCY, 0);
		#endif

	#endif
}

/**
  * @brief  UltraIO start to process.     
**/
void UltraIO_Start(void)
{
	#if (ULTRAIO_USE_STRUCTURE) 
	
	if( ULTRAIO_HW_CURVE_ENABLE && ( g_sUltraIO_Config.u16HwPwmEnable > 0 ))
	{
		if( g_sUltraIO_Config.u16HwPwmEnable&BIT0 )
			PWM_SET_CNR(PWM0, PWM_CH0, g_sUltraIO_Config.u8MaxResoluation);
		if( g_sUltraIO_Config.u16HwPwmEnable&BIT1 )
			PWM_SET_CNR(PWM0, PWM_CH1, g_sUltraIO_Config.u8MaxResoluation);
        PWM_Start(PWM0, g_sUltraIO_Config.u16HwPwmEnable);
	}
	
	#else
	
		#if ( ULTRAIO_HW_CURVE_ENABLE && ( ULTRAIO_INITIATE_HW_PWM_ENABLED > 0 ) )
        // Set CNR
		if( ULTRAIO_INITIATE_HW_PWM_ENABLED&BIT0 )
			PWM_SET_CNR(PWM0, PWM_CH0, ULTRAIO_MAX_RESOLUATION);
		if( ULTRAIO_INITIATE_HW_PWM_ENABLED&BIT1 )
			PWM_SET_CNR(PWM0, PWM_CH1, ULTRAIO_MAX_RESOLUATION);
		// Set PWM start counting.
        PWM_Start(PWM0,ULTRAIO_INITIATE_HW_PWM_ENABLED);
		#endif
	
	#endif
}

/**
  * @brief  UltraIO stop to process.        
**/
void UltraIO_Stop(void)
{
	#if (ULTRAIO_USE_STRUCTURE) 

	if( ULTRAIO_HW_CURVE_ENABLE && ( g_sUltraIO_Config.u16HwPwmEnable > 0 ))
		PWM_Stop(PWM0,g_sUltraIO_Config.u16HwPwmEnable);
	
	#else
	
		#if ( ULTRAIO_HW_CURVE_ENABLE && ( ULTRAIO_INITIATE_HW_PWM_ENABLED > 0 ) )
		// Close HW pwm. 
		PWM_Stop(PWM0,ULTRAIO_INITIATE_HW_PWM_ENABLED);	
		#endif
	
	#endif	
}

/**
  * @brief  Reset UltraIO include disable GPIO function, Timer(F/W) close and PWM(H/W) close.            
**/
void UltraIO_Reset(void)
{
	#if (ULTRAIO_USE_STRUCTURE) 

	if ( ULTRAIO_FW_CURVE_ENABLE && ( g_sUltraIO_Config.u8FwCurveCount > 0 ) )
	{
        NVIC_DisableIRQ(ULTRAIO_TMR_IRQ);
		TIMER_DisableInt(ULTRAIO_TMR_HANDLER);
		TIMER_Close(ULTRAIO_TMR_HANDLER);
	}
	
	if( ULTRAIO_HW_CURVE_ENABLE && ( g_sUltraIO_Config.u16HwPwmEnable > 0 ))
	{
		PWM_Stop(PWM0,g_sUltraIO_Config.u16HwPwmEnable);
		UltraIO_DisablePWMChannel( g_sUltraIO_Config.u16HwPwmEnable );
	}
	
	#else
	
		#if ( ULTRAIO_FW_CURVE_ENABLE && ( ULTRAIO_FW_CURVE_COUNT > 0 ) )
		// Close timer and interrupt.
        NVIC_DisableIRQ(ULTRAIO_TMR_IRQ);
		TIMER_DisableInt(ULTRAIO_TMR_HANDLER);
		TIMER_Close(ULTRAIO_TMR_HANDLER);
		#endif
	
		#if ( ULTRAIO_HW_CURVE_ENABLE && ( ULTRAIO_INITIATE_HW_PWM_ENABLED > 0 ) )
		// Close HW pwm. 
		PWM_Stop(PWM0,ULTRAIO_INITIATE_HW_PWM_ENABLED);
	    // Disable multi function pin.
	    UltraIO_DisablePWMChannel(ULTRAIO_INITIATE_HW_PWM_ENABLED);
		#endif
	
	#endif	
}

/**
  * @brief  Process audio/user event.	 
  * @param  u32CmdIndex is the curve index.(curve number = ULTRAIO_CMD_INDEX_START - u32CmdIndex) 
  * @param  u32SubIndex is the command type index.    
**/
void UltraIO_EventHandler(uint32_t u32CmdIndex, uint32_t u32SubIndex)
{
	#if (ULTRAIO_USE_STRUCTURE) 

		uint8_t 	u8i, u8CurveIndex;
		uint32_t 	u32CurveNo = ULTRAIO_CMD_INDEX_START- u32CmdIndex;
		
		if ( u32CurveNo >= g_sUltraIO_Config.u8LastCurveNo || g_sUltraIO_Config.au8CurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED )
	  		return;

		u8CurveIndex = g_sUltraIO_Config.au8CurveMap[u32CurveNo];

		if( g_sUltraIO_Config.u8CtrlCurve && ( (g_uiUltraIOCtrl&(1<<u8CurveIndex)) == 0 ) )
			return;	

 		if ( ULTRAIO_HW_CURVE_ENABLE && g_sUltraIO_Config.asCurveSetting[u8CurveIndex].u8Type == E_ULTRAIO_TYPE_HW )
		{
			for( u8i = 0; u8i < ULTRAIO_HW_PWM_MAX_COUNT; ++u8i )
			{
				if( g_sUltraIO_Config.asCurveSetting[u8CurveIndex].u8PwmIdx&(1<<u8i) )
				{
					if( u32SubIndex )
					{
						PWM_SET_CMR( PWM0, u8i, u32SubIndex*100/g_sUltraIO_Config.u8MaxResoluation );
						PWM_EnableOutput( PWM0,(1<<u8i) );
					}
					else
						PWM_DisableOutput( PWM0,(1<<u8i) );
				}	
			}
			return;
		}

	   	if ( ULTRAIO_FW_CURVE_ENABLE && ( g_sUltraIO_Config.u8FwCurveCount > 0 ) )
	  		g_u8UltraIOCurveValue[u8CurveIndex] = (uint16_t)u32SubIndex;
		
	#else
		
		#if ( ULTRAIO_LAST_CURVE_NO > 0 )
		{
			uint8_t 	u8CurveIndex;
			uint32_t 	u32CurveNo = ULTRAIO_CMD_INDEX_START- u32CmdIndex;
			
			// Check curve number is in range and created.
			if ( u32CurveNo >= ULTRAIO_LAST_CURVE_NO || g_u8UltraIOCurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED )
				return;
			
			// Cal curve index from curve map.
			u8CurveIndex = g_u8UltraIOCurveMap[u32CurveNo];		
			
			// The specified curve is disabled
			#if ( ULTRAIO_CTRL_CURVE )
			if ( (g_uiUltraIOCtrl&(1<<u8CurveIndex)) == 0 )	 	
				return;
			#endif
			
			#if ( ULTRAIO_HW_CURVE_ENABLE )
			{
				uint8_t u8i;
				
				// Check pwm type, the type should be HW.
				if ( g_sUltraIOCurveSetting[u8CurveIndex].u8Type == E_ULTRAIO_TYPE_HW )
				{
					// Scan current HW pwn index.
					for( u8i = 0; u8i < ULTRAIO_HW_PWM_MAX_COUNT; ++u8i )
					{
						// Get HW pwm index from IO curve table.
						if( g_sUltraIOCurveSetting[u8CurveIndex].u8PwmIdx&(1<<u8i) )
						{
							// Input value > 0.
							if( u32SubIndex )
							{
								// Set output value.
								PWM_SET_CMR( PWM0, u8i, u32SubIndex*100/ULTRAIO_MAX_RESOLUATION );
								// Enable HW pwm output.
								PWM_EnableOutput( PWM0, (1<<u8i) );
							}
							// Input value = 0, disable HW pwm output.
							else
								PWM_DisableOutput( PWM0, (1<<u8i) );
						}	
					}
					return;
				}
			}
			#endif
			
			// Pwm type is FW, set into IO curve value.
			#if ( ULTRAIO_FW_CURVE_ENABLE && ( ULTRAIO_FW_CURVE_COUNT > 0 ) )
			g_u8UltraIOCurveValue[u8CurveIndex] = (uint16_t)u32SubIndex;
			#endif
		}
		#endif

	#endif
}

#if ( ULTRAIO_FW_CURVE_ENABLE && ( ( ULTRAIO_FW_CURVE_COUNT > 0 ) || ULTRAIO_USE_STRUCTURE ) )
/**
  * @brief  1.Process firmware curve.
  *         2.This function is located in timer interrupt.   
**/
void UltraIO_OuputCurve(void)
{
	uint8_t u8i;

	#if (ULTRAIO_USE_STRUCTURE) 

		if( (++g_u8UltraIOFwPwmCounter) >= g_sUltraIO_Config.u8MaxResoluation )
			g_u8UltraIOFwPwmCounter = 0;

 		for( u8i = 0; u8i < g_sUltraIO_Config.u8FwCurveCount; ++u8i )
		{
			if( g_sUltraIO_Config.u8UsePortA )
			{
				if( g_u8UltraIOFwPwmCounter < g_u8UltraIOCurveValue[u8i] )
					GPIO_SET_OUT_DATA(PA,(GPIO_GET_OUT_DATA(PA)&~g_sUltraIO_Config.au16CurveToPortAPins[u8i]));
				else
				    GPIO_SET_OUT_DATA(PA,(GPIO_GET_OUT_DATA(PA)|g_sUltraIO_Config.au16CurveToPortAPins[u8i]));
			}

			if( g_sUltraIO_Config.u8UsePortB )
			{
				if( g_u8UltraIOFwPwmCounter < g_u8UltraIOCurveValue[u8i] )
					GPIO_SET_OUT_DATA(PB,(GPIO_GET_OUT_DATA(PB)&~g_sUltraIO_Config.au16CurveToPortBPins[u8i]));
				else
					GPIO_SET_OUT_DATA(PB,(GPIO_GET_OUT_DATA(PB)|g_sUltraIO_Config.au16CurveToPortBPins[u8i]));
			}	
		}

	#else
		// Add FW pwm counter.
		if( (++g_u8UltraIOFwPwmCounter) >= ULTRAIO_MAX_RESOLUATION) 
			g_u8UltraIOFwPwmCounter = 0;
	
		for( u8i = 0; u8i < ULTRAIO_FW_CURVE_COUNT; ++u8i )
		{
			// if counter >= curve set value, set output bit is high(led is dark).
			// else if counter < curve set value, set output bit is low(led is light).
			#if ( ULTRAIO_USE_PORTA )
			if( g_u8UltraIOFwPwmCounter < g_u8UltraIOCurveValue[u8i] )
				GPIO_SET_OUT_DATA(PA,(GPIO_GET_OUT_DATA(PA)&~g_uiUltraIOCurveToPortAPins[u8i]));
			else
				GPIO_SET_OUT_DATA(PA,(GPIO_GET_OUT_DATA(PA)|g_uiUltraIOCurveToPortAPins[u8i]));
			#endif
	
			#if ( ULTRAIO_USE_PORTB )
			if( g_u8UltraIOFwPwmCounter < g_u8UltraIOCurveValue[u8i] )
				GPIO_SET_OUT_DATA(PB,(GPIO_GET_OUT_DATA(PB)&~g_uiUltraIOCurveToPortBPins[u8i]));
			else
				GPIO_SET_OUT_DATA(PB,(GPIO_GET_OUT_DATA(PB)|g_uiUltraIOCurveToPortBPins[u8i]));
			#endif		
		}

	#endif
}
#endif

#if ( ULTRAIO_CTRL_CURVE || ULTRAIO_USE_STRUCTURE )	
/**
 * @brief   Control curve enable or disable.
 * @param   u32CurveNo is the number of curve.
 * @param   bEnable is enable/disable curve control.
 */
void UltraIO_ControlCurve(uint32_t u32CurveNo, BOOL bEnable)
{
	uint8_t u8CurveIndex;	

	#if( ULTRAIO_USE_STRUCTURE )
	
		if ( u32CurveNo >= g_sUltraIO_Config.u8LastCurveNo || g_sUltraIO_Config.au8CurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED )
	  		return;
	
		u8CurveIndex = g_sUltraIO_Config.au8CurveMap[u32CurveNo];
		
		if (bEnable)
			g_uiUltraIOCtrl |= 1<<u8CurveIndex;
		else
			g_uiUltraIOCtrl &= ~(1<<u8CurveIndex);				

	#else

		if ( u32CurveNo >= ULTRAIO_LAST_CURVE_NO || g_u8UltraIOCurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED )
	  		return;
		
		u8CurveIndex = g_u8UltraIOCurveMap[u32CurveNo];	
	
		if (bEnable)
			g_uiUltraIOCtrl |= 1<<u8CurveIndex;
		else
			g_uiUltraIOCtrl &= ~(1<<u8CurveIndex);
	
	#endif	
}
#endif

#if ( ULTRAIO_CTRL_OUTPUTPIN || ULTRAIO_USE_STRUCTURE  )
/**
 * @brief   Set(add/sub) curve output pin.(multi) 
 * @param   u32CurveNo is the number of curve.
 * @param   ePortID is the enumeration of curve port(E_ULTRAIO_PORTA/E_ULTRAIO_PORTB).
 * @param   u32Pin is the pin mask of curve pin.
 * @param   bEnable is enable/disable curve output. 
 */
void UltraIo_SetCurveOutputPin(uint32_t u32CurveNo, uint8_t ePortID, uint32_t u32Pin, BOOL bEnable)
{
	uint8_t u8CurveIndex;	

	#if( ULTRAIO_USE_STRUCTURE )

		if ( u32CurveNo >= g_sUltraIO_Config.u8LastCurveNo || g_sUltraIO_Config.au8CurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED )
	  		return;

		u8CurveIndex = g_sUltraIO_Config.au8CurveMap[u32CurveNo];

		if( ULTRAIO_HW_CURVE_ENABLE && g_sUltraIO_Config.asCurveSetting[u8CurveIndex].u8Type == E_ULTRAIO_TYPE_HW )
			return;
			
		switch(ePortID)
		{
			case  E_ULTRAIO_PORTA:

				if( g_sUltraIO_Config.u8UsePortA )
				{
					if ( bEnable )
						g_sUltraIO_Config.au16CurveToPortAPins[u8CurveIndex] |= 1<<u32Pin;
					else
						g_sUltraIO_Config.au16CurveToPortAPins[u8CurveIndex] &= ~(1<<u32Pin);
				}

			break;		
			
			case  E_ULTRAIO_PORTB:

				if( g_sUltraIO_Config.u8UsePortB )
				{
					if ( bEnable )
						g_sUltraIO_Config.au16CurveToPortBPins[u8CurveIndex] |= 1<<u32Pin;
					else
						g_sUltraIO_Config.au16CurveToPortBPins[u8CurveIndex] &= ~(1<<u32Pin);
				}
				
			break;				
		}			

	#else

		if ( u32CurveNo >= ULTRAIO_LAST_CURVE_NO || g_u8UltraIOCurveMap[u32CurveNo] == ULTRAIO_CURVE_NO_CREATED	)
	  		return;
		
		u8CurveIndex = g_u8UltraIOCurveMap[u32CurveNo];	
	
		#if ( ULTRAIO_HW_CURVE_ENABLE )
		if ( g_sUltraIOCurveSetting[u8CurveIndex].u8Type == E_ULTRAIO_TYPE_HW )
			return;
		#endif
	
		switch(ePortID)
		{
			#if ( ULTRAIO_USE_PORTA )
			case  E_ULTRAIO_PORTA:
				if ( bEnable )
					g_uiUltraIOCurveToPortAPins[u8CurveIndex] |= 1<<u32Pin;
				else
					g_uiUltraIOCurveToPortAPins[u8CurveIndex] &= ~(1<<u32Pin);
			break;
			#endif
	
			#if ( ULTRAIO_USE_PORTB )
			case  E_ULTRAIO_PORTB:
				if ( bEnable )
					g_uiUltraIOCurveToPortBPins[u8CurveIndex] |= 1<<u32Pin;
				else
					g_uiUltraIOCurveToPortBPins[u8CurveIndex] &= ~(1<<u32Pin);
			break;
			#endif	
		}
	
	#endif
}
#endif
