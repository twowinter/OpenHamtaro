/**************************************************************************//**
 * @file     SysClk.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date:    14/10/15 06:18p $
 * @brief    ISD9100 Series system clock configuration file for framework.
 *
 * @note
 *
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "Platform.h"

#define _48MHZ_	 			(__HSI)

/**
  * @brief  This function initiates internal RC as the clock source to generates desired system clock.
  * @param  u32SystemClk is the desired system clock(HCLK). For example, 23040000UL means 23MHz
  * @return The actually system clock
  */
UINT32 SysClk_InitiateRC(UINT32 u32SystemClk)
{
	UINT32 u32SysClkDiv;
	
	/* Enable External XTL32K, OSC49M, OSC10K */
	CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk|CLK_PWRCTL_HIRCEN_Msk|CLK_PWRCTL_LIRCEN_Msk);

	/* Switch HCLK clock source to HXT */
	if( u32SystemClk == (32*1000000UL) )
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKSEL0_HIRCFSEL_32M, CLK_CLKDIV0_HCLK(1));
		
		return 32768000UL;
	}
	
	/* Procedures to configure the HCLK from internal RC(set clock source and divide) */
	if ( (u32SysClkDiv = _48MHZ_/u32SystemClk) == 0 )	// Means the desired system clock is over 48MHz RC spec.
		u32SysClkDiv = 1;
	else if (u32SysClkDiv > 0x10)						// Means the desired system clock is small than min HCLK clock
		u32SysClkDiv = 0x10;
	
	CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKSEL0_HIRCFSEL_48M, CLK_CLKDIV0_HCLK(u32SysClkDiv));

	u32SystemClk = _48MHZ_/u32SysClkDiv;
	
	return u32SystemClk;
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
