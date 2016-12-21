/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVBODTALARM_H__
#define __DRVBODTALARM_H__

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (PFN_DRVBODTALARM_CALLBACK)(void);

#define BODLVL_45V    0x07
#define BODLVL_30V    0x06
#define BODLVL_28V    0x05
#define BODLVL_27V    0x04
#define BODLVL_25V    0x03
#define BODLVL_24V    0x02
#define BODLVL_22V    0x01
#define BODLVL_21V    0x00

#define TALARM_SEL_145C    0x08
#define TALARM_SEL_135C    0x04
#define TALARM_SEL_125C    0x02
#define TALARM_SEL_115C    0x01
#define TALARM_SEL_105C    0x00

#define TALARM_EN_ENENABLE   0x01
#define TALARM_EN_ENDISABLE  0x00
#define TALARM_EN_IEENABLE   0x01
#define TALARM_EN_IEDISABLE  0x00
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVBOD_MAJOR_NUM 1
#define DRVBOD_MINOR_NUM 00
#define DRVBOD_BUILD_NUM 1


#define DRVBOD_VERSION_NUM    _SYSINFRA_VERSION(DRVBOD_MAJOR_NUM, DRVBOD_MINOR_NUM, DRVBOD_BUILD_NUM)
/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code	                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global interface variables declarations                                                                 */                                                                            
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototype                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SelectBODVolt(uint8_t u8Volt);
void DrvBOD_SelectBODHyst(uint8_t u8Hysteresis);
void DrvBOD_EnableBOD(uint32_t u32Enable);
void DrvBOD_SetBODIE(uint32_t u32BODIE);
uint32_t DrvBOD_GetBODout(void);
void DrvBOD_SetTALARMselect(uint32_t u32LVL);
void DrvBOD_EnableTALARM(uint32_t u32Enable);
uint32_t DrvBOD_GetTALARMstatus(void);
void DrvBOD_SetTALARMIE(uint32_t u32TALARMIE);
void DrvBOD_SetDetectionTime(uint32_t u32OnDUR, uint32_t u32OffDUR);
void DrvBOD_InstallISR(PFN_DRVBODTALARM_CALLBACK callback,int32_t i32para);
int32_t DrvBOD_GetVersion(void);

#endif	// __DRVBODTALARM_H__



