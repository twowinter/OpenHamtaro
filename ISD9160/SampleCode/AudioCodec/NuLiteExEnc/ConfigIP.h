/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __CONFIGIP_H__
#define __CONFIGIP_H__	  	 

#include "Platform.h"

#if( !defined(__CHIP_SERIES__) )
#error "Please update all files in HW\Include\ from SDS version >= v3.50."
#endif

// -------------------------------------------------------------------------------------------------------------------------------
// PDMA Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*  [31:24]						|		[20:19]					|		[15:12]					|		[11:8]						|		[7:6]									|		[5:4]						|		[3:2]						*/
/*  Service selection |		Transfer Width	|		Wrap Int Select	|		Interrupt type		|		Destination behavior 	|		Source behavior	|		PDMA Mode 			*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
#define PDMA_WRAP_HALF					(0x4)
#define PDMA_WRAP_END					(0x1)

#define PDMA_SHOT_MODE					((0<<PDMA_DSCT_CTL_WAINTSEL_Pos)|(PDMA_INTENCH_TXOKIEN_Msk<<8))
#define PDMA_WRAP_MODE					(((PDMA_WRAP_HALF|PDMA_WRAP_END)<<PDMA_DSCT_CTL_WAINTSEL_Pos)|(PDMA_INTENCH_WAINTEN_Msk<<8))

#define PDMA_ADC_SEL					((PDMA_ADC<<24)|PDMA_WIDTH_16|PDMA_SHOT_MODE|PDMA_SAR_FIX|PDMA_DAR_INC|PDMA_APB_SRAM)
#define PDMA_APU_SEL					((PDMA_DPWM<<24)|PDMA_WIDTH_16|PDMA_SHOT_MODE|PDMA_SAR_INC|PDMA_DAR_FIX|PDMA_SRAM_APB)

// -------------------------------------------------------------------------------------------------------------------------------
// ADC Related Definitions  
// -------------------------------------------------------------------------------------------------------------------------------
#define ADC_MAX_RESOULTION				(32767)
#define ADC_MIN_RESOULTION				(-32768)
#define ADC_SAMPLE_RATE		  			(12000)
#define ADC_PGA_GAIN					(600)
#define ADC_PDMA_CH						(0)		// PDMA supports 4 channels ,0~3.
#define ADC_DOWNSAMPLE					(4)		// only 1, 2, 4
												//---------------------------------------------------------------------------
												//Select ADC downsampling scheme for MIC:
												//  1-->No down sampling
												//  2-->down x2 rate
												//  4-->down x4 rate
												//---------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------------------------
// APU (DPWM) Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define APU_CLK_SOURCE					CLK_CLKSEL1_DPWMSEL_HIRC2X
#define APU_CLK							(__HIRC<<(APU_CLK_SOURCE>>CLK_CLKSEL1_DPWMCKSEL_Pos))
#define APU_MAX_RESOULTION				(32767)
#define APU_MIN_RESOULTION				(-32768)
#define APU_RAMP_STEP					(2)		// Increase or decrease value to do ramp down or do ramp up
#define APU_PDMA_CH						(1)		// PDMA supports 4 channels ,0~3.
#define APU_UPSAMPLE					(2)		// only 1 , 2,  4
												//---------------------------------------------------------------------------
												// Select APU upsampling scheme for speaker:
												//  1-->No up sampling
												//  2-->up x2 rate
												//  4-->up x4 rate
												//---------------------------------------------------------------------------
												
// -------------------------------------------------------------------------------------------------------------------------------
// Timer0 Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define TIMER0_CLK_SOURCE 				CLK_CLKSEL1_TMR0SEL_HIRC

// -------------------------------------------------------------------------------------------------------------------------------
// Timer1 Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define TIMER1_CLK_SOURCE 				CLK_CLKSEL1_TMR1SEL_LIRC
#define TIMER1_FREQUENCY      			(100)

// -------------------------------------------------------------------------------------------------------------------------------
// PWM0/1 Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define PWM01_CLK_SOURCE 				CLK_CLKSEL1_PWM0CH01SEL_HIRC

// -------------------------------------------------------------------------------------------------------------------------------
// WDT Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define WDT_CLK_SOURCE 					CLK_CLKSEL1_WDTSEL_HIRC

// -------------------------------------------------------------------------------------------------------------------------------
// SPIFLash Clock Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define SPI0_CLOCK						12000000

// -------------------------------------------------------------------------------------------------------------------------------
// RTC Related Definitions 
// -------------------------------------------------------------------------------------------------------------------------------
#define RTC_SYSTEM_TIME_YEAR			(2014)
#define RTC_SYSTEM_TIME_MONTH			(12)
#define RTC_SYSTEM_TIME_DAY				(1)
#define RTC_SYSTEM_TIME_HOUR			(0)
#define	RTC_SYSTEM_TIME_MINUTE			(0)
#define RTC_SYSTEM_TIME_SECOND			(0)
#define RTC_SYSTEM_TIME_DAYOFWEEK		(RTC_MONDAY)


#endif
