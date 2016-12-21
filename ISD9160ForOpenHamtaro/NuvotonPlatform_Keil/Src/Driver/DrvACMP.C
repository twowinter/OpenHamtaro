/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
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
#include "DrvACMP.h"
#include "DrvSYS.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_DRVACMP_CALLBACK  *g_pfnACMPCallBack      = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <ACMP_IRQHandler>                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle ACMP interrupt event           		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void ACMP_IRQHandler(void)
{
    if (g_pfnACMPCallBack != NULL)    /* execute ACMP callback function */
    {
        g_pfnACMPCallBack();
    }

    if(ACMP->CMPSR.CMPF0)
        ACMP->CMPSR.CMPF0 = 1;

    if(ACMP->CMPSR.CMPF1)
        ACMP->CMPSR.CMPF1 = 1;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_Ctrl				                                                   			   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/*               cmpCN 		- [in]		Comparator Negative     1:Enable / 0:Disable       		           */
/*               cmpIE     	- [in]		CMP Interrupt           1:Enable / 0:Disable                       */
/*               cmpEN 		- [in]		Comparator Enable       1:Enable / 0:Disable	                   */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set ACMP control register										           		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_Ctrl(uint8_t com, uint8_t cmpCN, uint8_t cmpIE, uint8_t cmpEN)
{
	if(com == CMP0 )
	{
	    ACMP->CMPCR[0].CMPCN = cmpCN;
	    ACMP->CMPCR[0].CMPIE = cmpIE;
	    ACMP->CMPCR[0].CMPEN = cmpEN;
	}
	else
	{
	    ACMP->CMPCR[1].CMPCN = cmpCN;
	    ACMP->CMPCR[1].CMPIE = cmpIE;
	    ACMP->CMPCR[1].CMPEN = cmpEN;
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_GetCMPFlag				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               0 : Flag is not set											                    	   */
/*               1 : Flag is set							                                               */
/* Description:                                                                                            */
/*               Get compare flag										               				   */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DrvACMP_GetCMPFlag(uint8_t com)
{
	if (com == CMP0)
	{
		return ACMP->CMPSR.CMPF0;
	}
	else
	{
		return ACMP->CMPSR.CMPF1;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_ClearCMPFlag				                                                   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Clear CMP flag										       					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_ClearCMPFlag(uint8_t com)
{
	if (com == CMP0)
	{
		ACMP->CMPSR.CMPF0 = 1;
	}
	else
	{
		ACMP->CMPSR.CMPF1 = 1;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_ComparatorEn				                                                   	       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable Comparator function                                                 	           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_ComparatorEn(uint8_t com)
{

    if (com == CMP0)
	{
        ACMP->CMPCR[0].CMPEN = 1;
	}
	else
	{
        ACMP->CMPCR[1].CMPEN = 1;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_ComparatorDis				                                                   	       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable Comparator function                                                 	           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_ComparatorDis(uint8_t com)
{

    if (com == CMP0)
	{
        ACMP->CMPCR[0].CMPEN = 0;
	}
	else
	{
        ACMP->CMPCR[1].CMPEN = 0;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_GetCMPOutput				                                                  		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               CMP Output Value											                    	       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Get compare Output										               				       */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DrvACMP_GetCMPOutput(uint8_t com)
{
	if (com == CMP0)
	{
		return ACMP->CMPSR.CO0;
	}
	else
	{
		return ACMP->CMPSR.CO1;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_EnableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               None  											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable ACMP interrupt and NVIC corresponding to ACMP		        					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_EnableInt(uint8_t com)
{
    if (com == CMP0)
	{
        ACMP->CMPCR[0].CMPIE = 1;
	}
	else
	{
        ACMP->CMPCR[1].CMPIE = 1;
	}
    NVIC_EnableIRQ(ACMP_IRQn);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_DisableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               CMP Number	- [in]		CMP0 / CMP1	       		                                           */
/* Returns:                                                                                                */
/*               None  											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable ACMP interrupt and NVIC corresponding to ACMP		        					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_DisableInt(uint8_t com)
{
    if (com == CMP0)
	{
        ACMP->CMPCR[0].CMPIE = 0;
	}
	else
	{
        ACMP->CMPCR[1].CMPIE = 0;
	}
    NVIC_DisableIRQ(ACMP_IRQn);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_InstallISR                                                                            */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               callback : call back function	                                                           */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to install ACMP callback function                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvACMP_InstallISR(PFN_DRVACMP_CALLBACK callback)
{
		g_pfnACMPCallBack    = callback;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvACMP_GetVersion                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              None                                                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*              None                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get the version number of ACMP driver.                                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvACMP_GetVersion(void)
{
	return DRVACMP_VERSION_NUM;
}

