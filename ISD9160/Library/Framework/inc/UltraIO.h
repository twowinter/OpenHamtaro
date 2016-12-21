/**************************************************************************//**
 * @file     UltraIO.h
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 14/09/09 10:00a $
 * @brief    Header file of UltraIO control.
 *           This file is independent between the different chip. 
 *
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef _ULTRAIO_H_
#define _ULTRAIO_H_

#include "ConfigApp.h"
#include "ConfigUltraIO.h"
	
#define ULTRAIO_CMD_INDEX_START     (253)		// UltraIO user event start index.
#define ULTRAIO_CURVE_NO_CREATED    (0xFF)	
#define ULTRAIO_CURVE_MAX_NUM       (32)

#if( ULTRAIO_FW_TMR == 0 )
    #define ULTRAIO_TMR_HANDLER     TIMER0
	#define ULTRAIO_TMR_IRQ         TMR0_IRQn
	#define ULTRAIO_CLK_SOURCE      TIMER0_CLK_SOURCE
	#define ULTRAIO_CLK_MODULE      TMR0_MODULE
	#define ULTRAIO_SYS_RST         TMR0_RST
#elif( ULTRAIO_FW_TMR == 1 )
    #define ULTRAIO_TMR_HANDLER     TIMER1
	#define ULTRAIO_TMR_IRQ         TMR1_IRQn
	#define ULTRAIO_CLK_SOURCE      TIMER1_CLK_SOURCE
	#define ULTRAIO_CLK_MODULE      TMR1_MODULE
	#define ULTRAIO_SYS_RST         TMR1_RST
#else
	#error "Defined ULTRAUO_TIMER is not allowed timer."
#endif

enum E_ULTRAIO_PWM_TYPE
{
	E_ULTRAIO_TYPE_HW	= 0, 
	E_ULTRAIO_TYPE_FW	= 1
};

enum E_ULTRAIO_PORT_ID
{
	E_ULTRAIO_PORTA, 
	E_ULTRAIO_PORTB		
};

#if (ULTRAIO_USE_STRUCTURE)
/* Define structure of configuration's bin file */ 
typedef struct
{
	uint8_t				u8TotalCurveCount;
	uint8_t				u8FwCurveCount;
	uint8_t				u8HwCurveFlag;
	uint8_t				u8UsePortA;
	uint8_t				u8UsePortB;
	uint8_t 			u8LastCurveNo;
	uint8_t 			u8CtrlCurve;
	uint8_t				u8CtrlOutputPin;
	uint8_t				u8PwmFrequency;
	uint8_t				u8MaxResoluation;
	uint8_t				au8CurveMap[ULTRAIO_CURVE_MAX_NUM];
	S_ULTRAIO_SETTING 	asCurveSetting[ULTRAIO_CURVE_MAX_NUM];
	uint16_t			u16PortAPin;
	uint16_t			u16PortBPin;
	uint16_t			u16HwPwmEnable;
	uint16_t			au16CurveToPortAPins[ULTRAIO_CURVE_MAX_NUM];
	uint16_t			au16CurveToPortBPins[ULTRAIO_CURVE_MAX_NUM];	
	uint32_t			u32PortAMode;
	uint32_t			u32PortBMode;
}S_ULTRAIO_CONFIG;

/**
  * @brief  Set Configs from bin file. 
  * @param  psConfig is config data(256bytes) from bin file
**/
void UltraIO_SetConfig(S_ULTRAIO_CONFIG* psConfig);
#endif

/**
  * @brief  Initiate UltraIO setting.
**/
void UltraIO_Initiate(void);

/**
  * @brief  UltraIO start to process.     
**/
void UltraIO_Start(void);

/**
  * @brief  UltraIO stop to process.        
**/
void UltraIO_Stop(void);

/**
  * @brief  Reset UltraIO include disable GPIO function, Timer(F/W) close and PWM(H/W) close.            
**/
void UltraIO_Reset(void);

/**
  * @brief  Process audio/user event.	 
  * @param  u32CmdIndex is the curve index.(curve number = ULTRAIO_CMD_INDEX_START - u32CmdIndex) 
  * @param  u32SubIndex is the command type index.    
**/
void UltraIO_EventHandler(uint32_t u32CmdIndex, uint32_t u32SubIndex);

/**
  * @brief  1. Enable PWM channel.
  *         2. Config PWM GPIO multi function pin.
  * @param  u8ChannelMask is PWM's channel pin mask(BIT0~BIT3) 
**/
__STATIC_INLINE void UltraIO_EnablePWMChannel(uint8_t u8ChannelMask)
{
	if( u8ChannelMask&BIT0 )
		SYS->GPB_MFP  = (SYS->GPB_MFP & (~SYS_GPB_MFP_PB4MFP_Msk) ) | SYS_GPB_MFP_PB4MFP_PWM0CH0_INV;
	if( u8ChannelMask&BIT1 )
		SYS->GPB_MFP  = (SYS->GPB_MFP & (~SYS_GPB_MFP_PB5MFP_Msk) ) | SYS_GPB_MFP_PB5MFP_PWM0CH1_INV;
}

/**
  * @brief  1. Disable PWM channel.
  *         2. Config PWM GPIO multi function pin.
  * @param  u8ChannelMask is PWM's channel pin mask(BIT0~BIT3) 
**/
__STATIC_INLINE void UltraIO_DisablePWMChannel(uint8_t u8ChannelMask)
{
	if( u8ChannelMask&BIT0 )
		SYS->GPB_MFP  = (SYS->GPB_MFP & (~SYS_GPB_MFP_PB4MFP_Msk) ) | SYS_GPB_MFP_PB4MFP_GPIO;
	if( u8ChannelMask&BIT1 )
		SYS->GPB_MFP  = (SYS->GPB_MFP & (~SYS_GPB_MFP_PB5MFP_Msk) ) | SYS_GPB_MFP_PB5MFP_GPIO;
}

/**
  * @brief  1.Process firmware curve.
  *         2.This function is located in timer interrupt.   
**/
#if ( ULTRAIO_FW_CURVE_ENABLE || ULTRAIO_USE_STRUCTURE )
void UltraIO_OuputCurve(void);
#endif
																							   
#if ( ULTRAIO_CTRL_CURVE || ULTRAIO_USE_STRUCTURE )
/**
 * @brief   Control curve enable or disable.
 * @param   u32CurveNo is the number of curve.
 * @param   bEnable is enable/disable curve control.
 */
void UltraIO_ControlCurve(uint32_t u32CurveNo, BOOL bEnable);
#endif

#if ( ULTRAIO_CTRL_OUTPUTPIN || ULTRAIO_USE_STRUCTURE )
/**
 * @brief   Set(add/sub) curve output pin.(multi) 
 * @param   u32CurveNo is the number of curve.
 * @param   ePortID is the enumeration of curve port(E_ULTRAIO_PORTA/E_ULTRAIO_PORTB).
 * @param   u32Pin is the pin mask of curve pin.
 * @param   bEnable is enable/disable curve output. 
 */
void UltraIo_SetCurveOutputPin(uint32_t u32CurveNo, uint8_t ePortID, uint32_t u32Pin, BOOL bEnable);
#endif

#endif
