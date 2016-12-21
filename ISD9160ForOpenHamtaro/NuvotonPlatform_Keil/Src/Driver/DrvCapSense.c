/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "DrvCapSense.h"


void DrvCAPSENSE_ISRC_En(uint32_t bits)
{
	ANA->ISRC.EN = bits;
}

void DrvCAPSENSE_ISRC_Val(uint32_t val)
{
	 ANA->ISRC.VAL = val;
}

void DrvCAPSENSE_Ctrl(uint32_t time, uint32_t cnt)
{
	ANA->CAPS_CTRL.LOW_TIME = time;
    ANA->CAPS_CTRL.CYCLE_CNT = cnt;   
}

void DrvCAPSENSE_INT_Enable(void)
{
	ANA->CAPS_CTRL.INT_EN = 1;
}

void DrvCAPSENSE_INT_Disable(void)
{
	ANA->CAPS_CTRL.INT_EN = 0;
}

void DrvCAPSENSE_ResetCnt(void)
{
	ANA->CAPS_CTRL.RST_CNT = 1;
    ANA->CAPS_CTRL.RST_CNT = 0;
}

void DrvCAPSENSE_CntEnable(void)
{
	ANA->CAPS_CTRL.CNT_EN = 1;
}

