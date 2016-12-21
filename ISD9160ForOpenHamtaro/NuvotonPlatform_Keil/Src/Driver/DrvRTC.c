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
#include "DrvRTC.h"
#include "DrvGPIO.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_GLOBALS

//#define RTC_DEBUG
#ifdef RTC_DEBUG
#define RTCDEBUG     printf
#else
#define RTCDEBUG(...)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_DRVRTC_CALLBACK  *g_pfnRTCCallBack_Tick    = NULL, *g_pfnRTCCallBack_Alarm   = NULL;
                   
static uint32_t volatile g_u32RTC_Count  = 0;
static char g_chHourMode = 0;
static int32_t volatile g_bIsEnableTickInt  = NULL;
static int8_t volatile g_bIsEnableAlarmInt  = NULL;

static uint32_t volatile g_u32Reg, g_u32Reg1,g_u32hiYear,g_u32loYear,g_u32hiMonth,g_u32loMonth,g_u32hiDay,g_u32loDay;
static uint32_t volatile g_u32hiHour,g_u32loHour,g_u32hiMin,g_u32loMin,g_u32hiSec,g_u32loSec;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_IRQHandler                                                                            */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               Install ISR to handle interrupt event                                                     */
/*---------------------------------------------------------------------------------------------------------*/

void RTC_IRQHandler(void)
{ 
   
	if (RTC->RIIR.TI ==0x1)      		   		/* tick interrupt occurred */
	{
 		  RTC->RIIR.TI = 1;
		  
		  g_u32RTC_Count++;                    	/* maintain RTC tick count */

          if (g_pfnRTCCallBack_Tick != NULL)    /* execute tick callback function */
          {
              g_pfnRTCCallBack_Tick();
          }

    }

 	if (RTC->RIIR.AI ==0x1)                		/* alarm interrupt occurred */
    {
          
		  RTC->RIIR.AI = 1;
		  
		  if (g_pfnRTCCallBack_Alarm != NULL) 	/* execute alarm callback function */
          {
              g_pfnRTCCallBack_Alarm();
          }


    }
}

 
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_SetFrequencyCompensation                                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 i32FrequencyX100  -[in]   the RTC oscillator clock X100, ex:3277365 means 32773.65        */
/* Returns:                                                                                                */
/*               E_SUCCESS					Success.                                                       */
/*               E_DRVRTC_ERR_FCR_VALUE		Wrong Compenation VALUE                                        */
/* DESCRIPTION                                                                                             */
/*                                                                                                         */
/*               Set Frequecy Compenation Data                                                             */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_SetFrequencyCompensation(int32_t i32FrequencyX100)
{
    int32_t i32RegInt,i32RegFra ;

	/* Compute Interger and Fraction for RTC register*/
    i32RegInt = (i32FrequencyX100/100) - DRVRTC_FCR_REFERENCE;
    i32RegFra = (((i32FrequencyX100%100)) * 60) / 100;
    
	/* Judge Interger part is reasonable */
    if ( (i32RegInt < 0) | (i32RegInt > 15) )
    {
        return E_DRVRTC_ERR_FCR_VALUE ;
    }
	
	DrvRTC_WriteEnable();

	outpw(&RTC->FCR, (uint32_t)((i32RegInt<<8) | i32RegFra));

    return E_SUCCESS;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_WriteEnable                                                                        */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               E_SUCCESS		            Success.                                                       */
/*               E_DRVRTC_ERR_FAILED       	FAILED                                                         */
/* DESCRIPTION                                                                                             */
/*               Access Password to AER to make access other register enable                               */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_WriteEnable (void)
{
    int32_t i32i = 0;

	int i32retry = 100;
    
	/*-------------------------------------------------------------------------------------------------*/
    /* After 200ms, Access enable wiil auto-clear. As soon as possible to do your setting              */
    /*-------------------------------------------------------------------------------------------------*/
	RETRY:

	i32i = 0;
	
	RTC->AER.AER = 0xA965;
	
    for (i32i = 0 ; i32i < DRVRTC_WAIT_COUNT ; i32i++)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* check RTC_AER[16] to find out RTC write enable                                                  */
        /*-------------------------------------------------------------------------------------------------*/
 		RTC->AER.AER = 0xA965;
		
		if (RTC->AER.ENF ==1)	
		    break;
	}

	
	if (i32i == DRVRTC_WAIT_COUNT)
    {
        RTCDEBUG ("\nRTC: RTC_WriteEnable, set write enable FAILED!\n");

		i32retry--;

        if(!i32retry)
	   		return E_DRVRTC_ERR_FAILED;
						
		goto RETRY;
    }

    
	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_Init                                                                               */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               E_SUCCESS            	Success.                                                           */
/*               E_DRVRTC_ERR_EIO		Initial RTC FAILED.                                                */
/* DESCRIPTION                                                                                             */
/*               Initial RTC and install ISR                                                               */
/*---------------------------------------------------------------------------------------------------------*/


int32_t DrvRTC_Init (void)
{
    int32_t i32i =0;
	
	int32_t i32delay=1000;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Initial time data struct and some parameters.                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    g_pfnRTCCallBack_Alarm = NULL;
    g_pfnRTCCallBack_Tick = NULL;

    g_u32RTC_Count = 0;
	
	UNLOCKREG();
	/* Enable 32K Clock */
	SYSCLK->PWRCON.XTL32K_EN =1;
	  
	/* Waiting for 32K stable */
  	while(i32delay--);
	
	/* Enable RTC Clock */
	SYSCLK->APBCLK.RTC_EN =1;
	
	LOCKREG();

    /*-----------------------------------------------------------------------------------------------------*/
    /* When RTC is power on, write 0xa5eb1357 to RTC_INIR to reset all logic.                              */
    /*-----------------------------------------------------------------------------------------------------*/
	RTC->INIR = DRVRTC_INIT_KEY;

    for (i32i = 0 ; i32i < DRVRTC_WAIT_COUNT ; i32i++)
    {

		if(RTC->INIR == 0x1)  /* Check RTC_INIR[0] to find out RTC reset signal */
        { 
           break;
        }
    }

    if (i32i == DRVRTC_WAIT_COUNT)
    {
        RTCDEBUG("\nRTC: RTC_Init, initial RTC FAILED!\n");
        return E_DRVRTC_ERR_EIO;
    }
    
    return E_SUCCESS;
}

 
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_Read                                                                               */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               eTime                    		Currnet_Timer/ Alarm_Time                                  */
/*               S_DRVRTC_TIME_DATA_T *spt   	Time Data                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS               Success.                                                          */
/*               E_DRVRTC_ERR_EIO		 Initial RTC FAILED.                                               */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Read current date/time or alarm date/time from RTC                                        */
/*---------------------------------------------------------------------------------------------------------*/


int32_t DrvRTC_Read (E_DRVRTC_TIME_SELECT eTime, S_DRVRTC_TIME_DATA_T *sPt)
{
    

	uint32_t u32Tmp;

    sPt->u8cClockDisplay = RTC->TSSR.HR24;                                 /* 12/24-hour */
    sPt->u32cDayOfWeek   = RTC->DWR.DWR;                                   /* Day of week */

    switch (eTime)
    {
        case DRVRTC_CURRENT_TIME:										   /* Get Current Time */
        {
			g_u32hiYear  = RTC->CLR.YEAR10 ;
    		g_u32loYear  = RTC->CLR.YEAR1;
    		g_u32hiMonth = RTC->CLR.MON10;
    		g_u32loMonth = RTC->CLR.MON1;
    		g_u32hiDay   = RTC->CLR.DAY10;
    		g_u32loDay   = RTC->CLR.DAY1;

		    g_u32hiHour =  RTC->TLR.HR10;
    		g_u32loHour =  RTC->TLR.HR1;
    		g_u32hiMin  =  RTC->TLR.MIN10;
    		g_u32loMin  =  RTC->TLR.MIN1;
    		g_u32hiSec  =  RTC->TLR.SEC10;
    		g_u32loSec  =  RTC->TLR.SEC1;
            break;
		}
        case DRVRTC_ALARM_TIME:									 	/* Get Alarm Time */
        {
			g_u32hiYear  = RTC->CAR.YEAR10 ;
    		g_u32loYear  = RTC->CAR.YEAR1;
    		g_u32hiMonth = RTC->CAR.MON10;
    		g_u32loMonth = RTC->CAR.MON1;
    		g_u32hiDay   = RTC->CAR.DAY10;
    		g_u32loDay   = RTC->CAR.DAY1;

		    g_u32hiHour =  RTC->TAR.HR10;
    		g_u32loHour =  RTC->TAR.HR1;
    		g_u32hiMin  =  RTC->TAR.MIN10;
    		g_u32loMin  =  RTC->TAR.MIN1;
    		g_u32hiSec  =  RTC->TAR.SEC10;
    		g_u32loSec  =  RTC->TAR.SEC1;
            break;
        }
		default:
        {
            return E_DRVRTC_ERR_ENOTTY;
        }
    }

    u32Tmp = (g_u32hiYear * 10);									/* Compute to 20XX year */
    u32Tmp+= g_u32loYear;
    sPt->u32Year   =   u32Tmp  + DRVRTC_YEAR2000;
    
    u32Tmp = (g_u32hiMonth * 10);									/* Compute 0~12 month */
    sPt->u32cMonth = u32Tmp + g_u32loMonth;
    
    u32Tmp = (g_u32hiDay * 10);										/* Compute 0~31 day */
    sPt->u32cDay   =  u32Tmp  + g_u32loDay;

    if (sPt->u8cClockDisplay == DRVRTC_CLOCK_12)					/* Compute12/24 hout */
    {
        u32Tmp = (g_u32hiHour * 10);
        u32Tmp+= g_u32loHour;
        sPt->u32cHour = u32Tmp;                                		/* AM: 1~12. PM: 21~32. */

        if (sPt->u32cHour >= 21)
        {
            sPt->u8cAmPm = DRVRTC_PM;
            sPt->u32cHour -= 20;
        }
        else
        {
            sPt->u8cAmPm = DRVRTC_AM;
        }
        
        u32Tmp = (g_u32hiMin  * 10);
        u32Tmp+= g_u32loMin;
        sPt->u32cMinute = u32Tmp;
        
        u32Tmp = (g_u32hiSec  * 10);
        u32Tmp+= g_u32loSec;
        sPt->u32cSecond = u32Tmp;

    }
    else
    {   															/* RTC_CLOCK_24 */
		sPt->u8cAmPm = DRVRTC_AM;  // by default

        u32Tmp = (g_u32hiHour * 10);
        u32Tmp+= g_u32loHour;
        sPt->u32cHour   = u32Tmp;
        
        u32Tmp = (g_u32hiMin  * 10);
        u32Tmp+= g_u32loMin;
        sPt->u32cMinute = u32Tmp;
        
        u32Tmp = (g_u32hiSec  * 10);
        u32Tmp+= g_u32loSec;
        sPt->u32cSecond = u32Tmp;
    }

    return E_SUCCESS;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_Write                                                                              */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               eTime                    		Currnet_Timer/ Alarm_Time                                  */
/*               S_DRVRTC_TIME_DATA_T *sPt      Time Data                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS               Success.                                                          */
/*               E_DRVRTC_ERR_EIO        Initial RTC FAILED.                                               */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Write current date/time or alarm date/time from RTC                                        */
//*--------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_Write(E_DRVRTC_TIME_SELECT eTime, S_DRVRTC_TIME_DATA_T *sPt)
{
    uint32_t u32Reg;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check RTC time data value is reasonable or not.                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if ( ((sPt->u32Year - DRVRTC_YEAR2000) > 99)|
         ((sPt->u32cMonth == 0) || (sPt->u32cMonth > 12))|
         ((sPt->u32cDay   == 0) || (sPt->u32cDay   > 31)))
    {
        RTCDEBUG ("\nRTC: Year value is incorrect\n");
        return E_DRVRTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32Year - DRVRTC_YEAR2000) > 99 )
    {
        RTCDEBUG ("\nRTC: Year value is incorrect\n");
        return E_DRVRTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32cMonth == 0) || (sPt->u32cMonth > 12) )
    {
        RTCDEBUG ("\nRTC: Month value is incorrect\n");
        return E_DRVRTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32cDay == 0) || (sPt->u32cDay > 31) )
    {
        RTCDEBUG ("\nRTC: Day value is incorrect\n");
        return E_DRVRTC_ERR_CALENDAR_VALUE;
    }

    if (sPt->u8cClockDisplay == DRVRTC_CLOCK_12)
    {
        if ( (sPt->u32cHour == 0) || (sPt->u32cHour > 12) )
        {
            RTCDEBUG ("\nRTC: Hour value is incorrect\n");
            return E_DRVRTC_ERR_TIMESACLE_VALUE;
        }
    }
    else if (sPt->u8cClockDisplay == DRVRTC_CLOCK_24)
    {
        if (sPt->u32cHour > 23)
        {
            RTCDEBUG ("\nRTC: Hour value is incorrect\n");
            return E_DRVRTC_ERR_TIMESACLE_VALUE;
        }
    }
    else
    {
        RTCDEBUG ("\nRTC: Clock mode is incorrect\n");
        return E_DRVRTC_ERR_TIMESACLE_VALUE;
    }

    if (sPt->u32cMinute > 59)
    {
        RTCDEBUG ("\nRTC: Minute value is incorrect\n");
        return E_DRVRTC_ERR_TIME_VALUE;
    }

    if (sPt->u32cSecond > 59)
    {
        RTCDEBUG ("\nRTC: Second value is incorrect\n");
        return E_DRVRTC_ERR_TIME_VALUE;
    }

    if (sPt->u32cDayOfWeek > 6)
    {
        RTCDEBUG ("\nRTC: Day of week value is incorrect\n");
        return E_DRVRTC_ERR_DWR_VALUE;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Important, call RTC_Open() before write data into any register.                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    g_u32Reg = DrvRTC_WriteEnable();
    if (g_u32Reg != 0)
    {
        return E_DRVRTC_ERR_FAILED;
    }

    switch (eTime)
    {

        case DRVRTC_CURRENT_TIME:

		g_pfnRTCCallBack_Tick = NULL;

            /*---------------------------------------------------------------------------------------------*/
            /* Second, set RTC time data.                                                                  */
            /*---------------------------------------------------------------------------------------------*/

            if (sPt->u8cClockDisplay == DRVRTC_CLOCK_12)
            {
                g_chHourMode = DRVRTC_CLOCK_12;
				DrvRTC_WriteEnable();
				RTC->TSSR.HR24 = DRVRTC_CLOCK_12;
                RTCDEBUG ("RTC: 12-hour\n");
                /*-----------------------------------------------------------------------------------------*/
                /* important, range of 12-hour PM mode is 21 upto 32                                       */
                /*-----------------------------------------------------------------------------------------*/
                if (sPt->u8cAmPm == DRVRTC_PM)
                    sPt->u32cHour += 20;
            }
            else                                                                  /* RTC_CLOCK_24 */
            {
                g_chHourMode = DRVRTC_CLOCK_24;
				DrvRTC_WriteEnable();
    			RTC->TSSR.HR24 = DRVRTC_CLOCK_24;
                RTCDEBUG ("RTC: 24-hour\n");
            }
			

		    
			/*---------------------------------------------------------------------------------------------*/
            /* Second, set RTC time data.                                                                  */
            /*---------------------------------------------------------------------------------------------*/

			u32Reg     = ((sPt->u32Year - DRVRTC_YEAR2000) / 10) << 20;
		    u32Reg    |= (((sPt->u32Year - DRVRTC_YEAR2000) % 10) << 16);
		    u32Reg    |= ((sPt->u32cMonth  / 10) << 12);
		    u32Reg    |= ((sPt->u32cMonth  % 10) << 8);
		    u32Reg    |= ((sPt->u32cDay    / 10) << 4);
		    u32Reg    |= (sPt->u32cDay     % 10);
            g_u32Reg = u32Reg;
			
			DrvRTC_WriteEnable();          
			outpw(&RTC->CLR, (uint32_t)g_u32Reg);
			RTCDEBUG ("RTC: REG_RTC_CLR[0x%08x]\n", inpw(&RTC->CLR));  
            
		    u32Reg     = ((sPt->u32cHour / 10) << 20);
		    u32Reg    |= ((sPt->u32cHour % 10) << 16);
		    u32Reg    |= ((sPt->u32cMinute / 10) << 12);
		    u32Reg    |= ((sPt->u32cMinute % 10) << 8);
		    u32Reg    |= ((sPt->u32cSecond / 10) << 4);
		    u32Reg    |= (sPt->u32cSecond % 10);
			g_u32Reg = u32Reg;
		
			DrvRTC_WriteEnable();
           	outpw(&RTC->TLR, (uint32_t)g_u32Reg);
			outpw(&RTC->DWR, sPt->u32cDayOfWeek);
			 
			RTCDEBUG ("RTC: REG_RTC_TLR[0x%08x]\n", inpw(&RTC->TLR));   
			
            return E_SUCCESS;


         case DRVRTC_ALARM_TIME:

            g_pfnRTCCallBack_Alarm = NULL;                                /* Initial call back function.*/
            
			/*---------------------------------------------------------------------------------------------*/
            /* Set Calender alarm time data.                                                               */
            /*---------------------------------------------------------------------------------------------*/
		    u32Reg     = ((sPt->u32Year - DRVRTC_YEAR2000) / 10) << 20;
		    u32Reg    |= (((sPt->u32Year - DRVRTC_YEAR2000) % 10) << 16);
		    u32Reg    |= ((sPt->u32cMonth  / 10) << 12);
		    u32Reg    |= ((sPt->u32cMonth  % 10) << 8);
		    u32Reg    |= ((sPt->u32cDay    / 10) << 4);
		    u32Reg    |= (sPt->u32cDay     % 10);
            g_u32Reg = u32Reg;
			DrvRTC_WriteEnable();
			
			outpw(&RTC->CAR, (uint32_t)g_u32Reg);
			RTCDEBUG ("RTC: REG_RTC_CAR[0x%08x]\n", inpw(&RTC->CAR));  
			
						 
            if (g_chHourMode == DRVRTC_CLOCK_12)
            {
                if (sPt->u8cAmPm == DRVRTC_PM)       /* important, range of 12-hour PM mode is 21 upto 32 */
                    sPt->u32cHour += 20;
            }

			/*---------------------------------------------------------------------------------------------*/
            /* Set Time alarm time data.                                                                   */
            /*---------------------------------------------------------------------------------------------*/
		    u32Reg     = ((sPt->u32cHour / 10) << 20);
		    u32Reg    |= ((sPt->u32cHour % 10) << 16);
		    u32Reg    |= ((sPt->u32cMinute / 10) << 12);
		    u32Reg    |= ((sPt->u32cMinute % 10) << 8);
		    u32Reg    |= ((sPt->u32cSecond / 10) << 4);
		    u32Reg    |= (sPt->u32cSecond % 10);

			g_u32Reg = u32Reg;
            DrvRTC_WriteEnable();
           	outpw(&RTC->TAR, (uint32_t)g_u32Reg);
			RTCDEBUG ("RTC: REG_RTC_TAR[0x%08x]\n", inpw(&RTC->TAR));   


            return E_SUCCESS;


	        default:
	        {
	            return E_DRVRTC_ERR_ENOTTY;
	        }
    }

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_IsLeapYear                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               1:  This year is LEAP year                                                                */
/*               0:  This year is NOT LEAP year                                                            */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               According current time setting, return this is LEAP year or not.                          */
//*--------------------------------------------------------------------------------------------------------*/

int32_t DrcRTC_IsLeapYear(void)
{
	return (RTC->LIR.LIR == 0x1)?1:0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_GetIntTick                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None.                                                                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               Software Tick Count in tick interrupt                                                     */
/*               ex.If the RTC tick mode is 1 tick /sec. After 10 second , it should be return 10.         */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               The function is used to get current Software tick count after enable tick interrupt.      */
//*--------------------------------------------------------------------------------------------------------*/

int32_t DrvRTC_GetIntTick(void)
{
   return g_u32RTC_Count;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_ResetIntTick                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None.                                                                                     */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               The function is used to reset the tick count counting in interrupt                        */
//*--------------------------------------------------------------------------------------------------------*/

void DrvRTC_ResetIntTick(void)
{
   g_u32RTC_Count = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_EnableWakeUp                                                                       */
/*                                                                                                         */
/* Parameters:        	                                                                                    */
/*               None      																						 			 */
/* Returns:                                                                                                */
/*               E_SUCCESS      Operation Successful                                       					 */
/* Description:                                                                                            */
/*               This function is used to enable wakeup CPU function                                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_EnableWakeUp(void)
{
	uint32_t u32Reg;
	u32Reg = DrvRTC_WriteEnable();
	
	if (u32Reg != 0)
    {
    	return E_DRVRTC_ERR_EIO ;
    }
	
	RTC->TTR.TWKE = 1;

	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_DisableWakeUp                                                                       */
/*                                                                                                         */
/* Parameters:        	                                                                                    */
/*               None      																						 			 */
/* Returns:                                                                                                */
/*               E_SUCCESS      Operation Successful                                       					 */
/* Description:                                                                                            */
/*               This function is used to disable wakeup CPU function                                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_DisableWakeUp(void)
{
	uint32_t u32Reg;
	u32Reg = DrvRTC_WriteEnable();
	
	if (u32Reg != 0)
    {
    	return E_DRVRTC_ERR_EIO ;
    }
	
	RTC->TTR.TWKE = 0;

	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_EnableInt                                                                          */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               str_IntSrc     Interrupt Source:  DRVRTC_TICK_INT / DRVRTC_ALARM_INT       			   */
/*               pfncallback    Callback function pointer                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS      Operation Successful                                                       */
/*               E_DRVRTC_ERR_ENOTTY    Wrong Parameter                                                    */
/* Description:                                                                                            */
/*               This function is used to enable RTC specified interrupt and install callback function     */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_EnableInt(DRVRTC_INT_SOURCE str_IntSrc, PFN_DRVRTC_CALLBACK pfncallback)
{
	switch (str_IntSrc)
    {

    	case DRVRTC_TICK_INT:
        {
        	g_bIsEnableTickInt   	= TRUE;
			RTC->RIIR.TI 		= 1; 
   			RTC->RIER.TIER 			= 1; 
			g_pfnRTCCallBack_Tick  	= pfncallback;
   			break;
        }
        case DRVRTC_ALARM_INT:
        {
            g_bIsEnableAlarmInt  	= TRUE;
    		RTC->RIIR.AI 		= 1; 
			RTC->RIER.AIER 			= 1; 
			g_pfnRTCCallBack_Alarm 	= pfncallback;
            break;
        }
        default:
        {
            return E_DRVRTC_ERR_ENOTTY;
        }
    }

    //NVIC_SetPriority(RTC_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
	NVIC_EnableIRQ(RTC_IRQn); 

	return E_SUCCESS;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_DisableInt                                                                         */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               i32IntSrc  Interrupt Source:  DRVRTC_TICK_INT / DRVRTC_ALARM_INT /DRVRTC_ALL_INT          */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to disable RTC specified interrupt and remove callback function     */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_DisableInt(DRVRTC_INT_SOURCE i32IntSrc)
{
	
	if((i32IntSrc & DRVRTC_TICK_INT) == DRVRTC_TICK_INT	)
	{
		g_bIsEnableTickInt  = FALSE;	
		RTC->RIER.TIER 		= 0; 
		RTC->RIIR.TI 		= 1; 
	}
	else if((i32IntSrc & DRVRTC_ALARM_INT) == DRVRTC_ALARM_INT )
	{
        g_bIsEnableAlarmInt = FALSE;
    	RTC->RIER.AIER 		= 0; 
		RTC->RIIR.AI 		= 1; 
	}
	else
	{
		return E_DRVRTC_ERR_ENOTTY;
	}
	return E_SUCCESS;	

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvRTC_SetTickMode                                                                            */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               uMode[IN]	         the structure of DRVRTC_TICK. It is ued to set the RTC time           */
/*                                   tick period for Periodic Time Tick Interrupt request                  */
/*                                   It consists of                                                        */
/*                                      DRVRTC_TICK_1_SEC ~ DRVRTC_TICK_1_128_SEC                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_SUCCESS               Operation Successful                                              */
/*               E_DRVRTC_ERR_EIO        Access Enable failed                                              */
/*               E_DRVRTC_ERR_ENOTTY     Parameter is wrong                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to set time tick period for periodic time tick Interrupt.           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_SetTickMode(uint8_t ucMode)
{
    g_u32RTC_Count = 0;
	
    if (DrvRTC_WriteEnable() != 0)   				/* Write PASSWORD to access enable*/
    {
    	return E_DRVRTC_ERR_EIO ;
    }
    
	if (ucMode > DRVRTC_TICK_1_128_SEC)             /* Tick mode 0 to 7 */
    {
    	return E_DRVRTC_ERR_ENOTTY ;
    }
            
  	RTC->TTR.TTR = ucMode;            

	return E_SUCCESS;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvRTC_Close                                                                              */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None            							                                               */
/* Returns:                                                                                                */
/*               E_SUCCESS                Success.                                                         */
/*               E_DRVRTC_ERR_ENODEV      Interface number incorrect.                                      */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Disable NVIC channel of RTC and both tick and alarm interrupt..                           */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvRTC_Close (void)
{

    g_bIsEnableTickInt = FALSE;
    
  	NVIC_DisableIRQ(RTC_IRQn);
   
	DrvRTC_DisableInt(DRVRTC_ALL_INT);

    return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvRTC_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               None	                                                         						   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               The DrvRTC version number                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to get the DrvPWM version number                                    */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvRTC_GetVersion (void)
{
	return DRVRTC_VERSION_NUM;
}







