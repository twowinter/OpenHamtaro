/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DRVSYS_H
#define DRVSYS_H

#include "ISD9xx.h"

/* SysTick constants */
#define SYSTICK_ENABLE              0                                          /* Config-Bit to start or stop the SysTick Timer                         */
#define SYSTICK_TICKINT             1                                          /* Config-Bit to enable or disable the SysTick interrupt                 */
#define SYSTICK_CLKSOURCE           2                                          /* Clocksource has the offset 2 in SysTick Control and Status Register   */
#define SYSTICK_MAXCOUNT       ((1<<24) -1)                                    /* SysTick MaxCount     */ 

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define	DRVSYS_MAJOR_NUM 1
#define	DRVSYS_MINOR_NUM 00
#define	DRVSYS_BUILD_NUM 1

#define DRVSYS_VERSION_NUM		_SYSINFRA_VERSION(DRVSYS_MAJOR_NUM,DRVSYS_MINOR_NUM,DRVSYS_BUILD_NUM)  

//E_DRVSYS_ERR_UNLOCK_FAIL      Unlock key address failed
//E_DRVSYS_ERR_LOCK_FAIL      Lock key address failed
//E_DRVSYS_ERR_ARGUMENT    	  Wrong Argument
//E_DRVSYS_ERR_IPSRC    	  Wrong IP clock selection
//E_DRVSYS_ERR_IPDIV    	  Wrong IP divider selection
#define E_DRVSYS_ERR_UNLOCK_FAIL    _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVSYS, 1)
#define E_DRVSYS_ERR_LOCK_FAIL    _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVSYS, 2)
#define E_DRVSYS_ERR_ARGUMENT      	_SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVSYS, 3)	
#define E_DRVSYS_ERR_IPSRC      _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVSYS, 4)	
#define E_DRVSYS_ERR_IPDIV      _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVSYS, 5)	

/*---------------------------------------------------------------------------------------------------------*/
/*  SYS Clcok Source Selector						                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
	E_DRVSYS_EXT_12M = 0,
	E_DRVSYS_INT_22M = 1,	
}E_DRVSYS_SRC_CLK;


/*---------------------------------------------------------------------------------------------------------*/
/*  IP reset						                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
	E_SYS_GPIO_RST  = 1,
	E_SYS_TMR0_RST  = 2,
	E_SYS_TMR1_RST  = 3,
	E_SYS_TMR2_RST  = 4,
	E_SYS_TMR3_RST  = 5,
	E_SYS_I2C0_RST  = 8,
	E_SYS_I2C1_RST  = 9,
	E_SYS_SPI0_RST  = 12,
	E_SYS_SPI1_RST  = 13,
	E_SYS_SPI2_RST  = 14,
	E_SYS_SPI3_RST  = 15,
	E_SYS_UART0_RST = 16,
	E_SYS_UART1_RST = 17,
	E_SYS_UART2_RST = 18,	
	E_SYS_PWM03_RST = 20,
	E_SYS_PWM47_RST = 21,
	E_SYS_ACMP_RST  = 22,
	E_SYS_PS2_RST   = 23,
	E_SYS_CAN0_RST  = 24,
	//E_SYS_CAN1_RST  = 25,
	E_SYS_USBD_RST  = 27,
	E_SYS_ADC_RST   = 28,
	E_SYS_I2S_RST   = 29,	
	E_SYS_PDMA_RST  = 32,
}E_SYS_IP_RST;

/*---------------------------------------------------------------------------------------------------------*/
/*  SYS IP Clcok Selector		     				                                                       */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum 
{
	E_SYS_WDG_CLK   = 4,
	E_SYS_RTC_CLK   = 5,
	E_SYS_TMR0_CLK  = 6,
	E_SYS_TMR1_CLK  = 7,
	E_SYS_I2C0_CLK  = 8,
	E_SYS_SPI0_CLK  = 12,
	E_SYS_DPWM_CLK  = 13,
	E_SYS_UART0_CLK = 16,
	E_SYS_BIQ_CLK 	= 18,	
	E_SYS_CRC_CLK 	= 19,	
	E_SYS_PWM01_CLK = 20,
	E_SYS_ACMP_CLK  = 22,
	E_SYS_SBRAM_CLK = 26,
	E_SYS_ADC_CLK   = 28,
	E_SYS_I2S_CLK   = 29,
	E_SYS_ANA_CLK   = 30,
	E_SYS_PDMA_CLK  = 33,
	E_SYS_ISP_CLK   = 34
}E_SYS_IP_CLK;


/*---------------------------------------------------------------------------------------------------------*/
/*  SYS IP Clcok Divider		     				                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
	E_SYS_ADC_DIV,
//	E_SYS_CAN_DIV,
	E_SYS_UART_DIV,
//	E_SYS_USB_DIV,
	E_SYS_HCLK_DIV

}E_SYS_IP_DIV;


/*---------------------------------------------------------------------------------------------------------*/
/*  SYS IP Clcok Selector		     				                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
//	E_SYS_FRQDIV_CLKSRC,
	E_SYS_I2S_CLKSRC,

	E_SYS_PWM01_CLKSRC,
//	E_SYS_CAN_CLKSRC,
	E_SYS_UART_CLKSRC,
	E_SYS_TMR1_CLKSRC,
	E_SYS_TMR0_CLKSRC,
	E_SYS_ADC_CLKSRC,
	E_SYS_WDG_CLKSRC,
	E_SYS_DPWM_CLKSRC
}E_SYS_IP_CLKSRC;


/*---------------------------------------------------------------------------------------------------------*/
/*  SYS Oscillator Control		     				                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
	E_SYS_NONE,
	E_SYS_XTL32K,
	E_SYS_OSC49M,
	E_SYS_OSC10K
}E_SYS_OSC_CTRL;


typedef void (*BOD_CALLBACK)(void);
typedef void (*PWRWU_CALLBACK)(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Define GCR functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void 	 DrvSYS_ResetIP(E_SYS_IP_RST eIpRst);
void 	 DrvSYS_ResetCPU(void);
void 	 DrvSYS_ResetChip(void);
void 	 DrvSYS_EnableBOD(int32_t i32Enable);
void 	 DrvSYS_SelectBODVolt(uint8_t u8Volt);
void 	 DrvSYS_EnableBODRst(int32_t i32Enable, BOD_CALLBACK bodcallbackFn);
void 	 DrvSYS_EnableBODLowPowerMode(int32_t i32Enable);
void 	 DrvSYS_EnableLowVoltRst(int32_t i32Enable);
void 	 DrvSYS_EnableTempatureSensor(int32_t i32Enable);
void 	 DrvSYS_SetPORDisCode(uint32_t u32Code);
void 	 DrvSYS_SetRCAdjValue(uint32_t u32Adj);
void 	 DrvSYS_SetIPClock(E_SYS_IP_CLK eIpClk, int32_t i32Enable);
uint32_t DrvSYS_GetEXTClock(void);
int32_t  DrvSYS_UnlockKeyAddr(void);
int32_t  DrvSYS_LockKeyAddr(void);
//uint32_t DrvSYS_ReadProductID(void);
uint32_t DrvSYS_GetRstSrc(void);
uint32_t DrvSYS_ClearRstSrc(uint32_t u32Src);
uint32_t DrvSYS_GetBODState(void);

void 	 DrvSYS_SetIPClock(E_SYS_IP_CLK eIpClk, int32_t i32Enable);
int32_t  DrvSYS_SetHCLKSource(uint8_t u8ClkSrcSel);
int32_t  DrvSYS_SetSysTickSource(uint8_t u8ClkSrcSel);
int32_t  DrvSYS_SetIPClockSource(E_SYS_IP_CLKSRC eIpClkSrc, uint8_t u8ClkSrcSel);
int32_t  DrvSYS_SetClockDivider(E_SYS_IP_DIV eIpDiv , int32_t i32value);
int32_t  DrvSYS_SetOscCtrl(E_SYS_OSC_CTRL eOscCtrl, int32_t i32Enable);
void     DrvSYS_EnablePWRWUInt(int32_t i32Enable, PWRWU_CALLBACK pdwucallbackFn, int32_t i32enWUDelay);
void 	 DrvSYS_EnablePowerDown(int32_t i32Enable);
void 	 DrvSYS_SetPowerDownWaitCPU(int32_t i32Enable);
void 	 DrvSYS_SetPllSrc(E_DRVSYS_SRC_CLK ePllSrc);
void 	 DrvSYS_SetPLLPowerDown(int32_t i32Enable);
void     DrvSYS_Delay(uint32_t us);

uint32_t DrvSYS_GetPllContent(uint32_t u32ExtClockKHz, uint32_t u32PllClockKHz);
uint32_t DrvSYS_GetPLLClock(void);
uint32_t DrvSYS_GetHCLK(void);
int32_t  DrvSYS_Open(uint32_t u32ExtClockKHz, uint32_t u32PllClockKHz);
int32_t  DrvSYS_EnableFreqDividerOutput(int32_t i32Enable, uint8_t u8Divider);

#endif

