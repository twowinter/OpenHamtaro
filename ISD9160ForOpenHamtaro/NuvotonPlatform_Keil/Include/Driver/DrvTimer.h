/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVTIMER_H__
#define __DRVTIMER_H__

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVTIMER_MAJOR_NUM 1
#define DRVTIMER_MINOR_NUM 00
#define DRVTIMER_BUILD_NUM 1


#define DRVTIMER_VERSION_NUM    _SYSINFRA_VERSION(DRVTIMER_MAJOR_NUM, DRVTIMER_MINOR_NUM, DRVTIMER_BUILD_NUM)
/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code	                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVTIMER_CMD					Wrong Command
// E_DRVTIMER_CHANNEL				Wrong Channel
// E_DRVTIMER_CLOCK_RATE			Wrong Clcok Rate
// E_DRVTIMER_EIO					Initial Timer Failed. 
#define E_DRVTIMER_CMD              _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVTIMER, 1)      /* Command error */
#define E_DRVTIMER_CHANNEL          _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVTIMER, 2)
#define E_DRVTIMER_CLOCK_RATE       _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVTIMER, 3)
#define E_DRVTIMER_EIO              _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVTIMER, 4)      /* Timer IO error */
#define E_DRVWDT_CMD                _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVTIMER, 5)      /* Invalid CMD   */ 

/*---------------------------------------------------------------------------------------------------------*/
/* Global interface variables declarations                                                                 */                                                                            
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*TIMER_CALLBACK)(uint32_t data);	/* function pointer */
typedef void (*WDT_CALLBACK)(uint32_t data);	/* function pointer */
/*---------------------------------------------------------------------------------------------------------*/
/* Define TIMER Channel                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {
    TMR0  =   0 ,
    TMR1  =   1        
}TIMER_CHANNEL;

/*---------------------------------------------------------------------------------------------------------*/
/* Define TIMER Ioctl Command                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {
    TIMER_IOC_START_COUNT            =   0 ,
    TIMER_IOC_STOP_COUNT             =   1 ,
    TIMER_IOC_ENABLE_INT             =   2 ,
    TIMER_IOC_DISABLE_INT            =   3 ,
    TIMER_IOC_RESET_TIMER            =   4 ,
    TIMER_IOC_SET_PRESCALE           =   5 ,
    TIMER_IOC_SET_INITIAL_COUNT      =   6 
} TIMER_CMD; 

/*---------------------------------------------------------------------------------------------------------*/
/* Define WDT Ioctl Command                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
//typedef enum {
//    WDT_IOC_START_TIMER       =   0,
//    WDT_IOC_STOP_TIMER        =   1,
//    WDT_IOC_ENABLE_INT        =   2,
//    WDT_IOC_DISABLE_INT       =   3,
//    WDT_IOC_RESET_TIMER       =   4,
//    WDT_IOC_ENABLE_RESET_FUNC =   5,
//    WDT_IOC_DISABLE_RESET_FUNC=   6,
//	WDT_IOC_SET_INTERVAL      =   7
//} WDT_CMD; 
typedef enum {
    E_WDT_IOC_START_TIMER       = 0,
    E_WDT_IOC_STOP_TIMER        = 1,
    E_WDT_IOC_ENABLE_INT        = 2,
    E_WDT_IOC_DISABLE_INT       = 3,
    E_WDT_IOC_ENABLE_WAKEUP     = 4, 
    E_WDT_IOC_DISABLE_WAKEUP    = 5, 
    E_WDT_IOC_RESET_TIMER       = 6,
    E_WDT_IOC_ENABLE_RESET_FUNC = 7,
    E_WDT_IOC_DISABLE_RESET_FUNC= 8,
	E_WDT_IOC_SET_INTERVAL      = 9
} E_WDT_CMD; 
/*---------------------------------------------------------------------------------------------------------*/
/* Define TIME EVENT STRUCT                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct timeEvent_t
{
    int32_t              active;
    int32_t              initTick;
    int32_t              curTick;
    TIMER_CALLBACK       funPtr;
    uint32_t             transParam;
} TIMER_EVENT_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Define TIMER OPREATION MODE                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum{                   
    ONESHOT_MODE    	  =   0 ,
    PERIODIC_MODE   	  =   1 ,
    TOGGLE_MODE     	  =   2 , 
	UNINTERREUPT_MODE     =   3 
    
}TIMER_OPMODE ;

/*---------------------------------------------------------------------------------------------------------*/
/* Define WDT INTERVAL                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
//typedef enum{
//    LEVEL0      =   0 ,         /* 2^4 clocks	                                                           */
//    LEVEL1      =   1 ,         /* 2^6 APB clocks                                                          */
//    LEVEL2      =   2 ,         /* 2^8 APB clocks                                                          */ 
//    LEVEL3      =   3 ,         /* 2^10 APB clocks                                                         */
//	LEVEL4      =   4 ,         /* 2^12 APB clocks                                                         */
//	LEVEL5      =   5 ,         /* 2^14 APB clocks                                                         */
//	LEVEL6      =   6 ,         /* 2^16 APB clocks                                                         */
//	LEVEL7      =   7           /* 2^18 APB clocks                                                         */
//}WDT_INTERVAL;
typedef enum{
    E_WDT_LEVEL0      = 0,         /* 2^4 WDT_CLK	                                                           */
    E_WDT_LEVEL1      = 1,         /* 2^6 WDT_CLK                                                            */
    E_WDT_LEVEL2      = 2,         /* 2^8 WDT_CLK                                                            */ 
    E_WDT_LEVEL3      = 3,         /* 2^10 WDT_CLK                                                           */
	E_WDT_LEVEL4      = 4,         /* 2^12 WDT_CLK                                                           */
	E_WDT_LEVEL5      = 5,         /* 2^14 WDT_CLK                                                           */
	E_WDT_LEVEL6      = 6,         /* 2^16 WDT_CLK                                                           */
	E_WDT_LEVEL7      = 7          /* 2^18 WDT_CLK                                                           */
} E_WDT_INTERVAL;

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototype                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvTIMER_Init(void);
void DrvTIMER_ClearTimerEvent(TIMER_CHANNEL channel, uint32_t uTimeEventNo);
void DrvTIMER_Delay(uint32_t uTicks);
void DrvTIMER_SetEXTClockFreq(uint32_t u32ClockFreq);
int32_t DrvTIMER_Open(TIMER_CHANNEL channel, uint32_t uTicksPerSecond, TIMER_OPMODE mode);
int32_t DrvTIMER_Ioctl(TIMER_CHANNEL channel,TIMER_CMD uCmd,uint32_t uArg1);
int32_t DrvTIMER_Close(TIMER_CHANNEL channel);
int32_t DrvTIMER_SetTimerEvent(TIMER_CHANNEL channel, uint32_t uTimeTick, TIMER_CALLBACK pvFun ,uint32_t parameter);
int32_t DrvTIMER_ResetTicks(TIMER_CHANNEL channel);
int32_t DrvTIMER_GetStatus(TIMER_CHANNEL ch);
int32_t DrvTIMER_GetVersion(void);
int32_t DrvTIMER_EnableInt(TIMER_CHANNEL channel);
uint32_t DrvTIMER_GetTicks(TIMER_CHANNEL channel);
void DrvWDT_ResetCount(void);
void DrvWDT_Close(void);
void DrvWDT_InstallISR(TIMER_CALLBACK pvNewISR);  
void DrvWDT_Open(E_WDT_INTERVAL WDTlevel);
int32_t DrvWDT_Ioctl(E_WDT_CMD uWDTCmd, uint32_t uArgument);

#endif	// __DRVTIMER_H__



