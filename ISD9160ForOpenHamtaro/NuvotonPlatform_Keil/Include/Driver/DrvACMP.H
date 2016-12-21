/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVACMP_H__
#define __DRVACMP_H__

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (PFN_DRVACMP_CALLBACK)(void);

#define CMP0   0
#define CMP1   1

/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVACMP_MAJOR_NUM 1
#define DRVACMP_MINOR_NUM 00
#define DRVACMP_BUILD_NUM 1


#define DRVACMP_VERSION_NUM    _SYSINFRA_VERSION(DRVACMP_MAJOR_NUM, DRVACMP_MINOR_NUM, DRVACMP_BUILD_NUM)
/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code	                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global interface variables declarations                                                                 */                                                                            
/*---------------------------------------------------------------------------------------------------------*/
#define CMPCR_CN0_VMID       1
#define CMPCR_CN0_VBG        0
#define CMPCR_CN1_VBG        1
#define CMPCR_CN1_GPIB7      0
#define CMPCR_CMPIE_EN       1
#define CMPCR_CMPIE_DIS      0
#define CMPCR_CMPEN_EN       1
#define CMPCR_CMPEN_DIS      0


/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototype                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_Ctrl(uint8_t com, uint8_t CN, uint8_t IE, uint8_t EN);
void DrvACMP_InstallISR(PFN_DRVACMP_CALLBACK callback);



#endif	// __DRVACMP_H__



