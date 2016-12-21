/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVPMU_H__
#define __DRVPMU_H__

#include "ISD9xx.h"

/* SysTick constants - copied from \Keil\ARM\RV31\INC\core_cm0.h */
#define SYSTICK_ENABLE              0                                          /* Config-Bit to start or stop the SysTick Timer                         */
#define SYSTICK_TICKINT             1                                          /* Config-Bit to enable or disable the SysTick interrupt                 */
#define SYSTICK_CLKSOURCE           2                                          /* Clocksource has the offset 2 in SysTick Control and Status Register   */
#define SYSTICK_MAXCOUNT       ((1<<24) -1)                                    /* SysTick MaxCount    */        
/*---------------------------------------------------------------------------------------------------------
 Macro, type and constant definitions                                                                    
---------------------------------------------------------------------------------------------------------*/
#define DPDWAKEUPMODE_PINOSC10KWAKEUP   0
#define DPDWAKEUPMODE_PINWAKEUP         1
#define DPDWAKEUPMODE_OSC10KWAKEUP      2
#define DPDWAKEUPMODE_ONLYPORWAKEUP     3

#define DPDWAKETIME_12ms    1
#define DPDWAKETIME_25ms    2
#define DPDWAKETIME_50ms    4
#define DPDWAKETIME_100ms   8

#define SPDWAKEUPMODE_GPIORTCWAKEUP     0
#define SPDWAKEUPMODE_GPIOWAKEUP        1
#define SPDWAKEUPMODE_OSC10KWAKEUP      2
#define SPDWAKEUPMODE_ONLYPORWAKEUP     3

#define SCB_SCR_SLEEPDEEP 4
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVPMU_MAJOR_NUM 1
#define DRVPMU_MINOR_NUM 00
#define DRVPMU_BUILD_NUM 1


#define DRVPMU_VERSION_NUM    _SYSINFRA_VERSION(DRVPMU_MAJOR_NUM, DRVPMU_MINOR_NUM, DRVPMU_BUILD_NUM)
/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code	                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global interface variables declarations                                                                 */                                                                            
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototype                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
extern void DrvPMU_DeepPowerDown(uint32_t u32DPDWakeupMode, uint32_t u32TimerSel);
extern void DrvPMU_StandbyPowerDown(void);
extern void DrvPMU_Stop(void);
extern void DrvPMU_DeepSleep(void);

#endif	// __DRVPMU_H__

