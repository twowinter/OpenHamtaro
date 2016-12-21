/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


#include "DrvPWM.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define PWMA_CNR0        PWMA_BA + 0x0C
#define PWMA_PDR0        PWMA_BA + 0x14
#define PWMA_CRLR0       PWMA_BA + 0x58
#define PWMA_CFLR0       PWMA_BA + 0x5C

#define PWMB_CNR0        PWMA_BA + 0x18
#define PWMB_PDR0        PWMA_BA + 0x20
#define PWMB_CRLR0       PWMA_BA + 0x60
#define PWMB_CFLR0       PWMA_BA + 0x64

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static S_DRVPWM_CALLBACK_T g_sDrvPWMAHandler = {0};


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               None	                                                         						   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               The DrvPWM version number                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to get the DrvPWM version number                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_GetVersion(void)
{
	return DRVPWM_VERSION_NUM;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <PWMA_IRQHandler>                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle PWMA interrupt event           		                     		           */
/*---------------------------------------------------------------------------------------------------------*/
void PWMA_IRQHandler(void)
{    



	/* Handle PWMA Timer function */
	if (PWMA->PIFR.PWMIF0)
	{
		PWMA->PIFR.PWMIF0 = 1;		 
		if (g_sDrvPWMAHandler.pfnPWM0CallBack != 0)
		{                           
        	g_sDrvPWMAHandler.pfnPWM0CallBack();
		}	
	}

	if (PWMA->PIFR.PWMIF1)
	{ 
		PWMA->PIFR.PWMIF1 = 1;		 
		if (g_sDrvPWMAHandler.pfnPWM1CallBack != 0)                           
        {
			g_sDrvPWMAHandler.pfnPWM1CallBack();
		}	
	}
 	


    /* Handle PWMA Capture function */
	if (PWMA->CCR0.CAPIF0) 
    {
        PWMA->CCR0.CAPIF0 = 1;
		if (g_sDrvPWMAHandler.pfnCAP0CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP0CallBack();
        }
	}
    
	if (PWMA->CCR0.CAPIF1) 
    {
        PWMA->CCR0.CAPIF1 = 1;
		if (g_sDrvPWMAHandler.pfnCAP1CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP1CallBack();
        }
	}
    

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_IsTimerEnabled                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer   - [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               0		disable                                                                            */
/*               1		enable	                                                                           */
/* Description:                                                                                            */
/*               This function is used to get PWMA/PWMB specified timer enable/disable state  	           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_IsTimerEnabled(uint8_t u8Timer)
{
	int32_t status;

	status = 0;
	
	switch (u8Timer)
	{
		case DRVPWM_TIMER0:
			status = PWMA->PCR.CH0EN;
			break;
		case DRVPWM_TIMER1:
			status = PWMA->PCR.CH1EN;				
			break;
	}
	
	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerCounter                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*               u16Counter     - [in]      Timer counter : 0~65535                                        */
/* Returns:                                                                                                */
/*               None	                                                                           		   */
/* Description:                                                                                            */
/*               This function is used to set the PWM0~1 specified timer counter 				   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SetTimerCounter(uint8_t u8Timer, uint16_t u16Counter)
{
	u8Timer &= 0x0F;

	if(u8Timer <= DRVPWM_TIMER1)
	{
		*((__IO uint32_t *) (PWMA_CNR0 + (u8Timer & 0x1) * 12)) = u16Counter;	
	}
}		

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetTimerCounter                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The specified timer counter value                                                         */
/*              											                                        	   */
/* Description:                                                                                            */
/*               This function is used to get the PWM0~1 specified timer counter value 			           */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_GetTimerCounter(uint8_t u8Timer)
{
	uint32_t u32Reg = 0;
	
	if (u8Timer <= DRVPWM_TIMER1)
	{
		u32Reg = *((__IO uint32_t *) (PWMA_PDR0 + (u8Timer & 0x1) * 12));	
	}

	return u32Reg;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_EnableInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1 							       */
/*               u8Int	 		- [in]		DRVPWM_CAP_RISING_INT/DRVPWM_CAP_FALLING_INT/DRVPWM_CAP_ALL_INT*/
/*              					        (The parameter is valid only when capture function)	       	   */
/*               pfncallback	- [in]		The call back function for specified timer / capture		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to Enable the PWM0~1 timer/capture interrupt 		               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_EnableInt(uint8_t u8Timer, uint8_t u8Int, PFN_DRVPWM_CALLBACK pfncallback)
{
	int32_t Rflag, Fflag;	

	Rflag = u8Int & 0x01;
	Fflag = u8Int & 0x02;
		
	switch (u8Timer)
	{	
		case DRVPWM_TIMER0:
			PWMA->PIER.PWMIE0 = 1;
			g_sDrvPWMAHandler.pfnPWM0CallBack = pfncallback;
			break;
		case DRVPWM_CAP0:			
			if (Rflag)
				PWMA->CCR0.CRL_IE0 = 1;
			if (Fflag)
				PWMA->CCR0.CFL_IE0 = 1;
		    g_sDrvPWMAHandler.pfnCAP0CallBack = pfncallback;
			break;	
		case DRVPWM_TIMER1:
			PWMA->PIER.PWMIE1 = 1;
			g_sDrvPWMAHandler.pfnPWM1CallBack = pfncallback;
			break;
		case DRVPWM_CAP1:			
			if (Rflag)
				PWMA->CCR0.CRL_IE1 = 1;
			if (Fflag)
				PWMA->CCR0.CFL_IE1 = 1;
		    g_sDrvPWMAHandler.pfnCAP1CallBack = pfncallback;
			break;
			
	}							
		
	NVIC_EnableIRQ(PWMA_IRQn);			 
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_DisableInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1 							       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to clear the PWM0~1 timer/capture interrupt 			               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_DisableInt(uint8_t u8Timer)
{   	
	switch(u8Timer)
	{	
		case DRVPWM_TIMER0:
			PWMA->PIER.PWMIE0 = 0;
			PWMA->PIFR.PWMIF0 = 1;
			g_sDrvPWMAHandler.pfnPWM0CallBack = NULL;
			break;
		case DRVPWM_TIMER1:
			PWMA->PIER.PWMIE1 = 0;
			PWMA->PIFR.PWMIF1 = 1;
			g_sDrvPWMAHandler.pfnPWM1CallBack = NULL;
			break;			

		case DRVPWM_CAP0:			 		
			PWMA->CCR0.CRL_IE0 = 0;
			PWMA->CCR0.CFL_IE0 = 0;
			g_sDrvPWMAHandler.pfnCAP0CallBack = NULL;	
			break;				
		case DRVPWM_CAP1:
			PWMA->CCR0.CRL_IE1 = 0;
			PWMA->CCR0.CFL_IE1 = 0;
			g_sDrvPWMAHandler.pfnCAP1CallBack = NULL;	
			break;		
						
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_ClearInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   							       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to clear the PWM0~1 timer/capture interrupt 			               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_ClearInt(uint8_t u8Timer)
{
	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				PWMA->CCR0.CAPIF0 = 1;
				break;
			case DRVPWM_CAP1:
				PWMA->CCR0.CAPIF1 = 1;				
				break;

		}			
	}	
	else
	{
		switch (u8Timer)
		{
			case DRVPWM_TIMER0:
				PWMA->PIFR.PWMIF0 = 1;
				break;
			case DRVPWM_TIMER1:
				PWMA->PIFR.PWMIF1 = 1;				
				break;

		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetIntFlag				                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   								   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               0		FALSE                                                                              */
/*               1		TRUE		                                                                       */
/* Description:                                                                                            */
/*               This function is used to get the PWM0~1 timer/capture interrupt flag 			           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_GetIntFlag(uint8_t u8Timer)
{
	int32_t status = 0;
	
	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				status = PWMA->CCR0.CAPIF0;
				break;
			case DRVPWM_CAP1:
				status = PWMA->CCR0.CAPIF1;				
				break;

		}		
	}
	else
	{
		switch (u8Timer)
		{
			case DRVPWM_TIMER0:
				status = PWMA->PIFR.PWMIF0;
				break;
			case DRVPWM_TIMER1:
				status = PWMA->PIFR.PWMIF1;				
				break;

		}	
	}

	return status;		
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetRisingCounter				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1 								       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The value which latches the counter when there・s a rising transition                      */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get value which latches the counter when there・s a rising 	   */		
/*				 transition		                   														   */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t DrvPWM_GetRisingCounter(uint8_t u8Capture)
{
	uint32_t u32Reg = 0;
	
	if (u8Capture <= DRVPWM_CAP1)
	{
		u32Reg = *((__IO uint32_t *) (PWMA_CRLR0 + ((u8Capture & 0x1) << 3)));	
	}

	return u32Reg;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetFallingCounter				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1								       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The value which latches the counter when there・s a falling transition                     */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get value which latches the counter when there・s a falling 	   */		
/*				 transition		                   														   */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t DrvPWM_GetFallingCounter(uint8_t u8Capture)
{
	uint32_t u32Reg = 0;
	
	if (u8Capture <= DRVPWM_CAP1)
	{
		u32Reg = *((__IO uint32_t *) (PWMA_CFLR0 + ((u8Capture & 0x1) << 3)));	
	}

	return u32Reg;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetCaptureIntStatus				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1								       */
/*               u8IntType 		- [in]		DRVPWM_CAP_RISING_FLAG/DRVPWM_CAP_FALLING_FLAG	       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               Check if there・s a rising / falling transition                    					       */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get the rising / falling transition interrupt flag			   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_GetCaptureIntStatus(uint8_t u8Capture, uint8_t u8IntType)
{	
	int32_t status;

	status = 0;	

	switch (u8Capture)
	{
		case DRVPWM_CAP0:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR0.CRLRI0 : PWMA->CCR0.CFLRI0;
			break;
		case DRVPWM_CAP1:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR0.CRLRI1 : PWMA->CCR0.CFLRI1;
			break;

	}
	
	return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_ClearCaptureIntStatus				                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1								       */
/*               u8IntType 		- [in]		DRVPWM_CAP_RISING_FLAG/DRVPWM_CAP_FALLING_FLAG	       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               Clear the rising / falling transition interrupt flag	                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to clear the rising / falling transition interrupt flag		 	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_ClearCaptureIntStatus(uint8_t u8Capture, uint8_t u8IntType)
{
	switch (u8Capture)
	{
		case DRVPWM_CAP0:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR0.CRLRI0 = 0) : (PWMA->CCR0.CFLRI0 = 0);
			break;
		case DRVPWM_CAP1:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR0.CRLRI1 = 0) : (PWMA->CCR0.CFLRI1 = 0);
			break;

	} 
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Open				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None             											                               */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None 											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable PWM engine clock and reset PWM											 	   	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Open(void)
{ 
	SYSCLK->APBCLK.PWM01_EN = 1;
	SYS->IPRSTC2.PWM10_RST = 1;
	SYS->IPRSTC2.PWM10_RST = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Close				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None             											                               */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None 											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable PWM engine clock and the I/O enable											   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Close(void)
{
	PWMA->POE.PWM0 = 0;
	PWMA->POE.PWM1 = 0;
	PWMA->CAPENR = 0;


	NVIC_DisableIRQ(PWMA_IRQn);
	
	SYS->IPRSTC2.PWM10_RST = 1;
	SYS->IPRSTC2.PWM10_RST = 0;

	SYSCLK->APBCLK.PWM01_EN = 0;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_EnableDeadZone				                                             		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Length 		   - [in]	Dead Zone Length : 0~255				       		   		   */
/*               i32EnableDeadZone - [in]	Enable DeadZone (1) / Diasble DeadZone (0)     		           */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to set the dead zone length and enable/disable Dead Zone function   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_EnableDeadZone(uint8_t u8Length, int32_t i32EnableDeadZone)
{	
			PWMA->PPR.DZI01  = u8Length;
			PWMA->PCR.DZEN01 = i32EnableDeadZone;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Enable				                            		                 		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1  								   */
/*               i32Enable		- [in]		Enable  (1) / Disable  (0)     					       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to enable PWM0~1 timer/capture function						 	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Enable(uint8_t u8Timer, int32_t i32Enable)
{
	switch (u8Timer & 0x01)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0EN = i32Enable;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1EN = i32Enable;
			break;

	} 	

	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				PWMA->CCR0.CAPCH0EN = i32Enable;
				break;
			case DRVPWM_CAP1:
				PWMA->CCR0.CAPCH1EN = i32Enable;
				break;

		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerClk					                                             		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 						DRVPWM_TIMER0/DRVPWM_TIMER1							       */
/*         										DRVPWM_CAP0/DRVPWM_CAP1						        	   */
/*				 S_DRVPWM_TIME_DATA_T *sPt																   */
/*               u8Frequency					Frequency   					       					   */
/*               u8HighPulseRatio				High Pulse Ratio    					       			   */
/*               u8Mode							DRVPWM_ONE_SHOT_MODE / DRVPWM_TOGGLE_MODE 				   */
/*               bInverter						Inverter Enable  (TRUE) / Inverter Disable  (FALSE)        */
/*               u8ClockSelector				Clock Selector											   */
/*              								DRVPWM_CLOCK_DIV_1/DRVPWM_CLOCK_DIV_2/DRVPWM_CLOCK_DIV_4   */
/*              								DRVPWM_CLOCK_DIV_8/DRVPWM_CLOCK_DIV_16					   */
/*												(The parameter takes effect when u8Frequency = 0)		   */
/*               u8PreScale						Prescale (2 ~ 256)										   */ 
/*												(The parameter takes effect when u8Frequency = 0)		   */
/*               u32Duty						Pulse duty										           */                             
/*												(The parameter takes effect when u8Frequency = 0 or		   */
/*												u8Timer = DRVPWM_CAP0/DRVPWM_CAP1) 						   */	
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The actual frequency											                           */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to configure the frequency/pulse/mode/inverter function		       */		
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_SetTimerClk(uint8_t u8Timer, S_DRVPWM_TIME_DATA_T *sPt)
{
	uint32_t 	u32Frequency;
	uint32_t	u32Freqout;
	uint16_t	u16Duty, u16cnrData, u16cmrData;
	uint8_t	    u8EngineClk = 0;
		
	u8EngineClk = SYSCLK->CLKSEL1.PWM01_S;				
		
	if (u8EngineClk == 0)		 /* external 12MHz crystal clock	*/
	{
		u32Freqout = 10000;	
	}
	else if(u8EngineClk == 1)    /* external 32KHz crystal clock */
	{
		u32Freqout = 32000;
	}
	else if(u8EngineClk == 2)	 /* HCLK clock */
	{
		u32Freqout = DrvSYS_GetHCLK()*1000;
	}
	else						 /* internal 48MHz oscillator clock */
	{
	 	u32Freqout = 48000000;	
	}

	switch (u8Timer)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1INV = sPt->i32Inverter;
			break;

		case DRVPWM_CAP0:
			PWMA->CCR0.INV0 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP1:
			PWMA->CCR0.INV1 = sPt->i32Inverter;
			break;

	} 

	switch (u8Timer & 0x01)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1MOD = sPt->u8Mode;
			break;

	} 
			
	
	if (sPt->u32Frequency == 0)
	{	
		uint8_t	u8Divider = 1;
		uint32_t u32duty;
		
		PWMA->PPR.CP01 = sPt->u8PreScale - 1;

		u32duty = sPt->u32Duty * sPt->u8HighPulseRatio / 100 - 1;
		
		switch (u8Timer & 0x01)
		{
			case DRVPWM_TIMER0:
				PWMA->CSR.CSR0 = sPt->u8ClockSelector;
				PWMA->CNR0 = sPt->u32Duty - 1;
				PWMA->CMR0 = u32duty;
				break;
			case DRVPWM_TIMER1:
				PWMA->CSR.CSR1 = sPt->u8ClockSelector;
				PWMA->CNR1 = sPt->u32Duty - 1;
				PWMA->CMR1 = u32duty;
				break;

		}
				
		switch (sPt->u8ClockSelector)
		{
			case DRVPWM_CLOCK_DIV_1:
				u8Divider = 1;
				break;
			case DRVPWM_CLOCK_DIV_2:
				u8Divider = 2;			
				break;			
			case DRVPWM_CLOCK_DIV_4:
				u8Divider = 4;			
				break;			
			case DRVPWM_CLOCK_DIV_8:
				u8Divider = 8;			
				break;			
			case DRVPWM_CLOCK_DIV_16: 
				u8Divider = 16;			
				break;		
		}		
		
		u32Frequency = u32Freqout / sPt->u8PreScale / u8Divider / sPt->u32Duty;
	}
	else
	{
		uint8_t	 u8Divider;
		uint16_t u16PreScale;
		
		u32Frequency =  u32Freqout / sPt->u32Frequency;		
		
		if (u32Frequency > 0x10000000)
			return 0;
			
		u8Divider = 1;			
			
		if (u32Frequency < 0x20000)
			u16PreScale = 2;	
		else
		{
			u16PreScale = u32Frequency / 65536;	
							
			if (u32Frequency / u16PreScale > 65536)
				u16PreScale++;
			
			if (u16PreScale > 256)
			{
				uint8_t u8i = 0;
				
				u16PreScale = 256;
				u32Frequency = u32Frequency / u16PreScale;
				
				u8Divider = u32Frequency / 65536;				
				
				if(u32Frequency / u8Divider > 65536)
					u8Divider++;				
				
				while(1)	
				{
					if((1 << u8i++) > u8Divider)
						break;
				}
				
				u8Divider = 1 << (u8i - 1);
				
				if (u8Divider > 16)
					return 0;	
					
				u32Frequency = u32Frequency * u16PreScale;						
			}		
					
		}
		u16Duty = (uint16_t )(u32Frequency/u16PreScale/u8Divider);
		
		u32Frequency = (u32Freqout / u16PreScale / u8Divider) / u16Duty;	
				
		PWMA->PPR.CP01 = u16PreScale - 1;
			
		switch(u8Divider)
		{
			case 1:
				u8Divider = DRVPWM_CLOCK_DIV_1;
				break;
			case 2:
				u8Divider = DRVPWM_CLOCK_DIV_2;			
				break;			
			case 4:
				u8Divider = DRVPWM_CLOCK_DIV_4;			
				break;			
			case 8:
				u8Divider = DRVPWM_CLOCK_DIV_8;			
				break;			
			case 16:
				u8Divider = DRVPWM_CLOCK_DIV_16;			
				break;		
		}				
					
		if (u8Timer & 0x10)
		{
			u16cnrData = sPt->u32Duty - 1; 
			u16cmrData = sPt->u32Duty * sPt->u8HighPulseRatio / 100 - 1;		
		}
		else
		{
		 	u16cnrData = u16Duty - 1;
			u16cmrData = u16Duty * sPt->u8HighPulseRatio / 100 - 1;
		}
		
		switch (u8Timer & 0x01)
		{
			case DRVPWM_TIMER0:
				PWMA->CSR.CSR0 = u8Divider;
				PWMA->CNR0 = u16cnrData;
				PWMA->CMR0 = u16cmrData;
				break;
			case DRVPWM_TIMER1:
				PWMA->CSR.CSR1 = u8Divider;
				PWMA->CNR1 = u16cnrData;
				PWMA->CMR1 = u16cmrData;
				break;

		}
	}
	
	return u32Frequency;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerIO				                                             		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 								   */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   								   */
/*               i32Enable		- [in]		Enable  (1) / Diasble  (0)     					       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to enable/disable PWM0~1 timer/capture I/O function				   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SetTimerIO(uint8_t u8Timer, int32_t i32Enable)
{
	if (i32Enable)
	{
		if (u8Timer & 0x10)
		{
				PWMA->CAPENR = PWMA->CAPENR | (1 << (u8Timer & 0x01));			
		}
		else
		{
			switch (u8Timer)
			{
				case DRVPWM_TIMER0:
					PWMA->POE.PWM0 = 1;
					break;
				case DRVPWM_TIMER1:
					PWMA->POE.PWM1 = 1;				
					break;

			}		
		}
	}
	else
	{
		if (u8Timer & 0x10)			
		{		
				PWMA->CAPENR = PWMA->CAPENR & ~(1 << (u8Timer & 0x01));			
		}
		else
		{
			switch (u8Timer)
			{
				case DRVPWM_TIMER0:
					PWMA->POE.PWM0 = 0;
					break;
				case DRVPWM_TIMER1:
					PWMA->POE.PWM1 = 0;				
					break;

			}	
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SelectClockSource				                                       		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*           u8ClockSourceSelector  - [in] DRVPWM_10K /DRVPWM_32K/DRVPWM_HCLK/DRVPWM_48M  				   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to select PWM clock source                                   	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SelectClockSource(uint8_t u8ClockSourceSelector)
{

	SYSCLK->CLKSEL1.PWM01_S = u8ClockSourceSelector;
	
}


