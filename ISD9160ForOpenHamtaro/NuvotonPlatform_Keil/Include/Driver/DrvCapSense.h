/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _DRVCAPSENSE_H
#define _DRVCAPSENSE_H

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*  Version define with SysInfra				                                                           */
/*---------------------------------------------------------------------------------------------------------*/
						   
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Error Code									                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Define parameter                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

#define LOW_TIME_3						0x3
#define LOW_TIME_2						0x2
#define LOW_TIME_1						0x1
#define LOW_TIME_0						0x0

#define CYCLE_CNT_7						0x7
#define CYCLE_CNT_6						0x6
#define CYCLE_CNT_5						0x5
#define CYCLE_CNT_4						0x4
#define CYCLE_CNT_3						0x3
#define CYCLE_CNT_2						0x2
#define CYCLE_CNT_1						0x1
#define CYCLE_CNT_0						0x0

typedef enum {
	eDRVCAP_0p5uA,
	eDRVCAP_1uA,
	eDRVCAP_2p5uA,
	eDRVCAP_5uA
} E_DRVCAO_ISRC;

/*---------------------------------------------------------------------------------------------------------*/
/* Define I2S functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

void DrvCAPSENSE_ISRC_En(uint32_t bits);
void DrvCAPSENSE_ISRC_Val(uint32_t val);
void DrvCAPSENSE_Ctrl(uint32_t time, uint32_t cnt);
void DrvCAPSENSE_INT_Enable(void);
void DrvCAPSENSE_INT_Disable(void);
void DrvCAPSENSE_ResetCnt(void);
void DrvCAPSENSE_CntEnable(void);

#endif

