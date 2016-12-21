/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
 
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "Driver\DrvBODTALARM.h"
#include "Driver\DrvSYS.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_DRVBODTALARM_CALLBACK  *g_pfnBODTALARMCallBack_BOD      = NULL;
static PFN_DRVBODTALARM_CALLBACK  *g_pfnBODTALARMCallBack_TALARM   = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <BOD_IRQHandler>                                                                          */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle BOD interrupt event            		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void BOD_IRQHandler(void)
{
	BOD->BOD_EN.INT = 1; //Clear INT

    if (g_pfnBODTALARMCallBack_BOD != NULL)    /* execute callback function */
    {
        g_pfnBODTALARMCallBack_BOD();
    }

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <TALARM_IRQHandler>                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle TALARM interrupt event            		                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TALARM_IRQHandler(void)
{
	BOD->TALARM_EN.INT = 1; //Clear INT
    if (g_pfnBODTALARMCallBack_TALARM != NULL)    /* execute callback function */
    {
        g_pfnBODTALARMCallBack_TALARM();
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SelectBODVolt				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Volt		- [in] 	  7:4.5V,   6:3.V,   5:2.8V,   4:2.7V  								   */
/*                     		          3:2.5V,   2:2.4V   1:2.2V,   0:2.1V              	   			       */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Select BOD voltage Level                           	           		   	   		       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SelectBODVolt(uint8_t u8Volt)
{
	BOD->BOD_SEL.BOD_LVL = u8Volt;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SelectBODHyst				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Hysteresis		- [in] 	  1: Enable Hysteresis of BOD detection 					   */
/*                     		                  0: Hysteresis Disabled              	   			           */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Select BOD Hysteresis                           	           		   	   		           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SelectBODHyst(uint8_t u8Hysteresis)
{
	BOD->BOD_SEL.BOD_HYS = u8Hysteresis & 0x01;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_EnableBOD				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32Enable		- [in] 	  1x : Enable continuous BOD detection  						   */
/*                     		              01 : Enable time multiplexed BOD detection                       */
/*                                        00 : Disable BOD detection             	   				       */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable BOD function                           	           		   	   		   			   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_EnableBOD(uint32_t u32Enable)
{
	BOD->BOD_EN.EN = u32Enable & 0x03;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SetBODIE				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32BODIE		- [in] 	  1 : Enable BOD interrupt  						               */
/*                     		              0 : Disable BOD interrupt                                        */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable BOD Interrupt                           	           		   	   		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SetBODIE(uint32_t u32BODIE)
{
	BOD->BOD_EN.IE = u32BODIE & 0x01;
	if( u32BODIE )
		NVIC_EnableIRQ(BOD_IRQn);
    else
		NVIC_DisableIRQ(BOD_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_GetBODout				                                               	   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None			  						   									   			   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               Output of BOD detection block			                                                   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Get BOD output block                           	           		   	   			   	   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvBOD_GetBODout(void)
{
	return BOD->BOD_EN.BOD_OUT;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SetTALARMselect				                                               	           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32LVL		- [in] 	  1000: 145C    0100: 135C    0010: 125C   							   */
/*                      		      0001: 115C    0000: 105C                     	   					   */
/* Returns:                                                                                                */
/*               None 										 											   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Set Tempature Alarm Sensor Level                           	           		   	   	   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SetTALARMselect(uint32_t u32LVL)
{
	BOD->TALARM_SEL.LVL = u32LVL;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_EnableTALARM				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32Enable		- [in] 	  1 : Enable TALARM detection  						               */
/*                     		              0 : Disable TALARM detection                                     */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable TALARM function                           	           		   	   		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_EnableTALARM(uint32_t u32Enable)
{
	BOD->TALARM_EN.EN = u32Enable & 0x01;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_GetTALARMstatus				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None			  						   									   			   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               Status of TALARM block			                                                           */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Get status of TALARM                           	           		   	   			   	   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvBOD_GetTALARMstatus(void)
{
	return BOD->TALARM_EN.TALARM;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SetTALARMIE				                                               	   		   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32TALARMIE	- [in] 	  1 : Enable TALARM interrupt  						               */
/*                     		              0 : Disable TALARM interrupt                                     */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Enable TALARM Interrupt                           	           		   	   		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SetTALARMIE(uint32_t u32TALARMIE)
{
	BOD->TALARM_EN.IE = u32TALARMIE & 0x01;
	if( u32TALARMIE )
		NVIC_EnableIRQ(TALARM_IRQn);
    else
		NVIC_DisableIRQ(TALARM_IRQn);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_SetDetectionTime				                                               	   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32OnDUR	- [in] 	  Time BOD detector is active (ms)  						           */
/*               u32OffDUR  - [in]    Time BOD detector if off    (ms)                                     */
/*              					                                                           			   */
/* Returns:                                                                                                */
/*               None																		   	   		   */
/*              					                                                           			   */
/* Description:                                                                                            */
/*               Set up BOD detector to take periodic samples of the                                       */
/*               supply voltage to minimize power consumption                          	           		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_SetDetectionTime(uint32_t u32OnDUR, uint32_t u32OffDUR)
{
	BOD->DET_TIMER.ON_DUR  = u32OnDUR;
	BOD->DET_TIMER.OFF_DUR = u32OffDUR;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_InstallISR                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               callback : call back function	                                                           */
/*               i32para  : 0=Setting BOD call back function;   1=Setting TALARM call back function        */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to install BOD & TALARM callback function                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvBOD_InstallISR(PFN_DRVBODTALARM_CALLBACK callback,int32_t i32para)
{
	 if(i32para ==0)
		g_pfnBODTALARMCallBack_BOD    = callback;
	 else
	 	g_pfnBODTALARMCallBack_TALARM = callback;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvBOD_GetVersion                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              None                                                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*              None                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get the version number of BOD driver.                                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvBOD_GetVersion(void)
{
	return DRVBOD_VERSION_NUM;
}

