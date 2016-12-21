/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "DrvPMU.h"
#include "DrvSYS.h"
#include "ISD9xx.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <DrvPMU_DeepPowerDown>                                                                    */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               u32DPDWakeupMode: PIN&OSC10KWAKEUP=0 OSC10KWAKEUP=1  PINWAKEUP=2  NOWAKEUP=3              */
/*               u32TimerSel     : 12.8ms=1  25.6ms=2  51.2ms=4  102.4ms=8                                 */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               Entering in Deep Power Down mode            		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPMU_DeepPowerDown(uint32_t u32DPDWakeupMode, uint32_t u32TimerSel)
{
	UNLOCKREG();

	SCB->SCR = SCB_SCR_SLEEPDEEP;
	SYSCLK->PWRCON.DEEP_PD = 1;
	SYSCLK->PWRCON.STOP = 0;
	SYSCLK->PWRCON.STANDBY_PD = 0;

	SYSCLK->PWRCON.PIN_ENB= 0;
	SYSCLK->PWRCON.OSC10K_ENB= 0;
	SYSCLK->PWRCON.TIMER_SEL = u32TimerSel;

	switch(u32DPDWakeupMode)
	{
		case DPDWAKEUPMODE_PINOSC10KWAKEUP:

		     break;

		case DPDWAKEUPMODE_PINWAKEUP:
		     SYSCLK->PWRCON.OSC10K_ENB = 1; //Disable OSC10K Wakeup
		     break;

		case DPDWAKEUPMODE_OSC10KWAKEUP:
			 SYSCLK->PWRCON.PIN_ENB = 1;    //Disable PIN Wakeup

		     break;

		case DPDWAKEUPMODE_ONLYPORWAKEUP:
		     SYSCLK->PWRCON.PIN_ENB = 1;    //Disable PIN Wakeup
		     SYSCLK->PWRCON.OSC10K_ENB = 1; //Disable OSC10K Wakeup
		     break;
    }

	LOCKREG();

	__wfi();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <DrvPMU_StandbyPowerDown>                                                                 */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               Entering in Standby Power Down mode            		                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPMU_StandbyPowerDown(void)
{
	UNLOCKREG();

	SCB->SCR = SCB_SCR_SLEEPDEEP;
	SYSCLK->PWRCON.STANDBY_PD = 1;
	SYSCLK->PWRCON.STOP = 0;
	SYSCLK->PWRCON.DEEP_PD = 0;
	LOCKREG();
	__wfi();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <DrvPMU_Stop>                                                                             */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               Entering in Stop mode                           		                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPMU_Stop(void)
{
	UNLOCKREG();

	SCB->SCR = SCB_SCR_SLEEPDEEP;
	SYSCLK->PWRCON.STOP = 1;
	SYSCLK->PWRCON.STANDBY_PD = 0;
	SYSCLK->PWRCON.DEEP_PD = 0;

	LOCKREG();
	__wfi();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <DrvPMU_DeepSleep>                                                                             */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               Entering in Deep Sleep mode                           		                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPMU_DeepSleep(void)
{
	UNLOCKREG();
	SYSCLK->PWRCON.STOP = 0;
	SYSCLK->PWRCON.STANDBY_PD = 0;
	SYSCLK->PWRCON.DEEP_PD = 0;
	SCB->SCR = SCB_SCR_SLEEPDEEP;
	LOCKREG();
	__wfi();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPMU_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              None                                                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*              None                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get the version number of PMU driver.                                                      */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPMU_GetVersion(void)
{

	return DRVPMU_VERSION_NUM;

}

