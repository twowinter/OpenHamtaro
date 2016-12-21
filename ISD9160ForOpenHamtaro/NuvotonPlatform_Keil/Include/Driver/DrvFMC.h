/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _DRVFMC_H
#define _DRVFMC_H

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVFMC_MAJOR_NUM	1
#define DRVFMC_MINOR_NUM	00
#define DRVFMC_BUILD_NUM	1
/*---------------------------------------------------------------------------------------------------------*/
/*  Version define with SysInfra				                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVFMC_VERSION_NUM     _SYSINFRA_VERSION(DRVFMC_MAJOR_NUM, DRVFMC_MINOR_NUM, DRVFMC_BUILD_NUM)							   
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Error Code									                                                   */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVFMC_ERR_ISP_FAIL  		ISP Failed when illegal condition occurs
#define E_DRVFMC_ERR_ISP_FAIL   _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVFMC, 1)

/*---------------------------------------------------------------------------------------------------------*/
/* Define parameter                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_BASE     0x00300000
#define CONFIG0         0x00300000
#define CONFIG1         0x00300004 
#define LDROM_BASE      0x00100000
#define PAGE_SIZE       1024
/*---------------------------------------------------------------------------------------------------------*/
/* ISPGO Trigger ISP and ensure M0 pipeline cleared with Instruction Sync Barrier                          */
/*---------------------------------------------------------------------------------------------------------*/

static __INLINE void ISPGO(void)
{
    // Trigger ISP command
  	FMC->ISPTRG = 1;
	// Flush M0 pipeline.
  	__ISB();
}

static __INLINE void ISPGO_ISR(void)
{
	__disable_irq();
    // Trigger ISP command
  	FMC->ISPTRG = 1;
	// Flush M0 pipeline.
  	__ISB();
    __enable_irq();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Flash Boot Selector 								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {APROM = 0, LDROM = 1} E_FMC_BOOTSELECT;
/*---------------------------------------------------------------------------------------------------------*/
/*  FMC Funciton Command Set     						                                                   */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum{
	E_DRVFMC_FUNC_READ =0x00, 
	E_DRVFMC_FUNC_CID  =0x0B,
	E_DRVFMC_FUNC_DID  =0x0C,
	E_DRVFMC_FUNC_PROG =0x21, 
	E_DRVFMC_FUNC_ERASE=0x22, 
	E_DRVFMC_FUNC_IDLE =0x30 
} E_DRVFMC_ISPCMD;

/*---------------------------------------------------------------------------------------------------------*/
/* Define FMC functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void 	 DrvFMC_EnableISP(int32_t i32Enable);
void 	 DrvFMC_BootSelect(E_FMC_BOOTSELECT boot);
void 	 DrvFMC_EnableLDUpdate(int32_t i32Enable);
void 	 DrvFMC_EnableConfigUpdate(int32_t i32Enable);
int32_t  DrvFMC_ReadCID(uint32_t * u32data);
int32_t  DrvFMC_ReadDID(uint32_t * u32data);
int32_t  DrvFMC_ReadPID(uint32_t * u32data);
int32_t  DrvFMC_Write(uint32_t u32addr, uint32_t u32data);
int32_t  DrvFMC_Read(uint32_t u32addr, uint32_t * u32data);
int32_t  DrvFMC_Erase(uint32_t u32addr);
int32_t  DrvFMC_WriteConfig(uint32_t u32data0, uint32_t u32data1);
uint32_t DrvFMC_ReadDataFlashBaseAddr(void);
E_FMC_BOOTSELECT DrvFMC_GetBootSelect(void);
int32_t DrvFMC_ReadBuffer(uint32_t u32addr, uint32_t * u32data,uint32_t u32buffersize);
int32_t DrvFMC_WriteBuffer(uint32_t u32addr, uint32_t *u32data,uint32_t u32buffersize);
#endif
