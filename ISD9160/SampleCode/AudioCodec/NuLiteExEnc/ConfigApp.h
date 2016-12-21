/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __CONFIGAPP_H__
#define __CONFIGAPP_H__	  	 
									
//%{CodeGen_Block_Start}
//ISD9300
#include "Platform.h"
#include "ConfigIP.h"
#include "SPIFlash.h"
// -------------------------------------------------------------------------------------------------------------------------------
// ADC Enable Options 
// -------------------------------------------------------------------------------------------------------------------------------
#define ADC_ENABLE	 				(1)			// 1:enable ADC, 0:disable ADC
#define ADC_FILTER_ENABLE			(0)			// 0: disable, 1:enable SW NuADC low pass filter, 2:enable HW biquad filter,
#define ADC_PDMA_ENABLE				(1)			// 1:enable HW PDMA to handle adc input data, 0: Software scheme

// -------------------------------------------------------------------------------------------------------------------------------
// APU Enable Options 
// -------------------------------------------------------------------------------------------------------------------------------
#define APU_ENABLE					(1)			// 1:enable APU, 0:disable APU	
#define APU_FILTER_ENABLE			(2)			// 0: disable, 1:enable SW NuDAC low pass filter, 2:enable HW biquad filter,
#define APU_PDMA_ENABLE 			(1)			// 1:enable HW PDMA to handle dpwm input data, 0: Software scheme	
#define PLAYBACK_CHANNEL_COUNT		(1)			// define the max playback channel(1, 2, 3 )
#define PLAYBACK_CHANNEL_NONE		0xff		// define the channel whill will not in playback
#define PLAYBACK_VOLUME_CONTROL		(1)			// 1:enable volume control, 0:disable volume control	
//#define PLAYBACK_SAME_SAMPLERATE	// Define playback channels are the same sample rate.	
//#define AUDIOMIXER_DUPLICATE		// Define    to enable mixer to duplicate   samples to mix channels different sample rate							
									// Undefine  to enable mixer to interpolate samples to mix channels different sample rate							
										

// -------------------------------------------------------------------------------------------------------------------------------
// PDMA Enable Options 
// -------------------------------------------------------------------------------------------------------------------------------
#define PDMA_ENABLE					(APU_PDMA_ENABLE|ADC_PDMA_ENABLE)

// -------------------------------------------------------------------------------------------------------------------------------
// SPIFLash Enable Options 
// -------------------------------------------------------------------------------------------------------------------------------
#define SPIFLASH_SPI0_DEVICE1

// -------------------------------------------------------------------------------------------------------------------------------
// Powerdown/Wakeup Enable Options 
// -------------------------------------------------------------------------------------------------------------------------------
#define WAKEUP_GPIO_TRIGGER			(1)		// Use GPIO to wake up chip
#define WAKEUP_RTC_TRIGGER			(0)		// Use RTC timer to wake up chip
#define WAKEUP_WDT_TRIGGER			(0)		// Use watch dog to wake up chip

#define WAKEUP_GPIO_TRG_PA			(0)		// GPIO trigger for GPIO A
#define WAKEUP_GPIO_TRG_PB			(BIT3)	// GPIO trigger for GPIO B	

#define WAKEUP_RTC_TRG_TIME_YEAR	(0)		// RTC wake-up time(year)
#define	WAKEUP_RTC_TRG_TIME_MONTH	(0)		// RTC wake-up time(month)
#define WAKEUP_RTC_TRG_TIME_DAY		(0)		// RTC wake-up time(day)
#define WAKEUP_RTC_TRG_TIME_HOUR	(0)		// RTC wake-up time(hour)
#define WAKEUP_RTC_TRG_TIME_MINUTE	(0)		// RTC wake-up time(minute)
#define WAKEUP_RTC_TRG_TIME_SECOND	(0)		// RTC wake-up time(second)

#define WAKEUP_WDT_TRG_TIMEOUT		(WDT_TIMEOUT_2POW4) 	// WDT wake-up time.

// Any wake-up trigger enable will enable power-down process, or disable.
#define POWERDOWN_ENABLE			(WAKEUP_GPIO_TRIGGER|WAKEUP_RTC_TRIGGER|WAKEUP_WDT_TRIGGER)				

// -------------------------------------------------------------------------------------------------
// Input and Output key configurations.
// Reference "ConfigIOPin.h" for detail configurations.
// -------------------------------------------------------------------------------------------------
#define CONFIG_MATRIX_KEY			(0)		// 1: enable matrix key, 0: disable matrix key
#define CONFIG_TRIGGER_KEY			(1)		// 1: enable trigger key, 0: disable trigger key
#define CONFIG_OUTPUT_PIN			(1)		// 1: enable output key, 0: disable output key
#define CONFIG_INPUT_STATE			(1)		// 1: enable input  state, 0: disable input  state
#define CONFIG_OUTPUT_STATE			(0)		// 1: enable output state, 0: disable output state

// -------------------------------------------------------------------------------------------------------------------------------
// UltraIO Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define ULTRAIO_FW_CURVE_ENABLE		(0)		// 1:enable UltrIO enable firmware pwm curve, 0:disable UltrIO enable firmware pwm curve
#define ULTRAIO_HW_CURVE_ENABLE		(0)		// 1:enable UltrIO enable hardware pwm curve, 0:disable UltrIO enable hardware pwm curve
#define ULTRAIO_FW_TMR				(1)		// 1:Set TIMER1 is used for FW pwm, 2:Set TIMER2 is used for FW pwm

#define ULTRAIO_USE_STRUCTURE		(0)		// 1:setting info. load from config structure, 0:setting info. use ConfigUltraIO.h


#if ((ADC_FILTER_ENABLE == 2) && (APU_FILTER_ENABLE == 2))
#error "HW biquad filter only supports alternative path for ADC or DPWM!"
#endif 	
//%{CodeGen_Block_End}

#define CONFIG_TOUCH_KEY			(0)		// 1: enable touch key, 0: disable touch key

#endif //#ifndef __CONFIGAPP_H__

