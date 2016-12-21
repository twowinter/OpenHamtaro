/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVALC_H__
#define __DRVALC_H__

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (PFN_DRVALC_CALLBACK)(void);


/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVALC_MAJOR_NUM 1
#define DRVALC_MINOR_NUM 00
#define DRVALC_BUILD_NUM 1


#define DRVALC_VERSION_NUM    _SYSINFRA_VERSION(DRVALC_MAJOR_NUM, DRVALC_MINOR_NUM, DRVALC_BUILD_NUM)
/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code	                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global interface variables declarations                                                                 */                                                                            
/*---------------------------------------------------------------------------------------------------------*/
#define DrvALC_ENABLE   1
#define DrvALC_DISABLE  0

#define DRVALC_NORMALMODE  0
#define DRVALC_LIMMODE     1

#define DRVALC_NGTH_N39dB  7
/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototype                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_InstallISR(PFN_DRVALC_CALLBACK callback);
void DrvALC_DisableInt(void);
void DrvALC_EnableInt(void);
void DrvALC_EnableNoiseGate(uint8_t u8NGSEL, uint8_t u8NGENval);
uint32_t DrvALC_GetFastDecrement(void);
uint32_t DrvALC_GetNoise(void);
uint32_t DrvALC_GetP2P(void);
uint32_t DrvALC_GetPeak(void);
int32_t DrvALC_GetVersion(void);
void DrvALC_InstallISR(PFN_DRVALC_CALLBACK callback);
void DrvALC_SetALCpeakLimiter(uint8_t u8PKlimiter);
void DrvALC_SetALCselect(uint8_t u8Select);
void DrvALC_SetAttackTime(uint32_t u32ATKstep);
void DrvALC_SetDecayTime(uint32_t u32DCYstep);
void DrvALC_SetHoldTime(uint32_t u32HoldTime);
void DrvALC_SetMaxGain(uint32_t u32MaxGain);
void DrvALC_SetMinGain(uint32_t u32MinGain);
void DrvALC_SetMode(uint8_t u8Mode);
void DrvALC_SetNGEN(uint8_t u8NGEN);
void DrvALC_SetNGTH(uint8_t u8NGTH);
void DrvALC_SetREG(uint32_t u32Value);
void DrvALC_SetTargetLevel(uint32_t u32Level);
void DrvALC_SetZeroCrossing(uint8_t u8ALCZC);


#endif	// __DRVALC_H__



