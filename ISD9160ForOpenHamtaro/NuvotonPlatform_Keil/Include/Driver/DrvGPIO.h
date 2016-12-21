/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DRVUART_H
#define DRVUART_H

#include "ISD9xx.h"
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVGPIO_MAJOR_NUM	1
#define DRVGPIO_MINOR_NUM	00
#define DRVGPIO_BUILD_NUM	1

/*---------------------------------------------------------------------------------------------------------*/
/*  Version define with SysInfra				                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVGPIO_VERSION_NUM     _SYSINFRA_VERSION(DRVGPIO_MAJOR_NUM, DRVGPIO_MINOR_NUM, DRVGPIO_BUILD_NUM)
							   
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Error Code									                                                   */
/*---------------------------------------------------------------------------------------------------------*/
/* Error code */
//E_DRVGPIO_ARGUMENT			Argument error 
//E_DRVGPIO_GROUP_PIN			Can't set bit function when it can't be set individually
//E_DRVGPIO_BUSY				GPIO is in useing
#define E_DRVGPIO_ARGUMENT          _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVGPIO, 1)
#define E_DRVGPIO_GROUP_PIN         _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVGPIO, 2)
#define E_DRVGPIO_BUSY              _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVGPIO, 3)   


typedef enum 
{
	GPA = 0,
	GPB = 1 
} DRVGPIO_PORT;

typedef void (*GPIO_GPAB_CALLBACK)(uint32_t u32GpaStatus, uint32_t u32GpbStatus);
typedef void (*GPIO_EINT0_CALLBACK)(void);
typedef void (*GPIO_EINT1_CALLBACK)(void);


/* Define GPIO PIN number */
#define		DRVGPIO_PIN15 	BIT15
#define		DRVGPIO_PIN14 	BIT14
#define 	DRVGPIO_PIN13 	BIT13
#define 	DRVGPIO_PIN12 	BIT12
#define 	DRVGPIO_PIN11 	BIT11
#define 	DRVGPIO_PIN10 	BIT10
#define 	DRVGPIO_PIN9 	BIT9
#define 	DRVGPIO_PIN8	BIT8
#define 	DRVGPIO_PIN7 	BIT7
#define 	DRVGPIO_PIN6 	BIT6
#define 	DRVGPIO_PIN5 	BIT5
#define 	DRVGPIO_PIN4 	BIT4
#define 	DRVGPIO_PIN3 	BIT3
#define 	DRVGPIO_PIN2 	BIT2
#define 	DRVGPIO_PIN1 	BIT1
#define 	DRVGPIO_PIN0 	BIT0

/*
typedef enum {DRVGPIO_BIT0 = 0, DRVGPIO_BIT1, DRVGPIO_BIT2, DRVGPIO_BIT3, DRVGPIO_BIT4, DRVGPIO_BIT5
                , DRVGPIO_BIT6, DRVGPIO_BIT7, DRVGPIO_BIT8, DRVGPIO_BIT9, DRVGPIO_BIT10, DRVGPIO_BIT11
                , DRVGPIO_BIT12, DRVGPIO_BIT13, DRVGPIO_BIT14, DRVGPIO_BIT15} DRVGPIO_BIT;
*/
/*---------------------------------------------------------------------------------------------------------*/
/*  SYS IP Clcok Selector		     				                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	FUNC_GPIO 	, 	
	FUNC_PWM01 	,  	FUNC_PWM01B	,  	
	FUNC_I2C0 	,  	FUNC_I2C1 	,	FUNC_I2C2	,
	FUNC_I2S0 	,	FUNC_I2S1	,
	FUNC_SPI0	,	FUNC_SPI1	,
	FUNC_ACMP0	,	FUNC_ACMP1	,
	FUNC_UART0	,	FUNC_UART0_FLOW,
	FUNC_TMR0	,	FUNC_TMR1	,
	FUNC_MCLK0	,	FUNC_MCLK1	,
	FUNC_DMIC0	,	FUNC_DMIC1	,
	FUNC_SPK	,
	FUNC_NONE
} DRVGPIO_FUNC;
			  

typedef enum {IO_INPUT , IO_OUTPUT, IO_OPENDRAIN , IO_QUASI} DRVGPIO_IO;
//typedef enum {IO_LOWDRV, IO_HIGHDRV} DRVGPIO_DRIVE;
//typedef enum {IO_NO_PULL_UP, IO_PULL_UP} DRVGPIO_PULL_UP;
//typedef enum {IO_INT0, IO_INT1, IO_INT2, IO_INT3} DRVGPIO_INT_NUM;
typedef enum {IO_RISING, IO_FALLING, IO_BOTH_EDGE} DRVGPIO_INT_TYPE;
typedef enum {MODE_EDGE, MODE_LEVEL} DRVGPIO_INT_MODE;
typedef enum {DBCLKSRC_HCLK = 0 ,DBCLKSRC_10K =1} DRVGPIO_DBCLKSRC;	   

/*---------------------------------------------------------------------------------------------------------*/
/* Define GPIO functions prototype                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_Close(DRVGPIO_PORT port,int32_t i32Bit);
int32_t DrvGPIO_ClrBit(DRVGPIO_PORT port,int32_t i32Bit);
int32_t DrvGPIO_ClrBitMask(DRVGPIO_PORT port, uint32_t u32Bit);
int32_t DrvGPIO_DisableDebounce(DRVGPIO_PORT port, uint32_t u32Bit);
void DrvGPIO_DisableEINT0(void);
void DrvGPIO_DisableEINT1(void);
int32_t DrvGPIO_DisableInt(DRVGPIO_PORT port,uint32_t u32Bit);
int32_t DrvGPIO_EnableDebounce(DRVGPIO_PORT port,uint32_t u32Bit);
void DrvGPIO_EnableEINT0(DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode, GPIO_EINT0_CALLBACK pfEINT0Callback);
void DrvGPIO_EnableEINT1(DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode, GPIO_EINT1_CALLBACK pfEINT1Callback);
int32_t DrvGPIO_EnableInt(DRVGPIO_PORT port, uint32_t u32Bit, DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode);
int32_t DrvGPIO_GetBit(DRVGPIO_PORT port, int32_t i32Bit);
int32_t DrvGPIO_GetDebounceTime(void);
int32_t DrvGPIO_GetDoutBit(DRVGPIO_PORT port, int32_t i32Bit);
uint32_t DrvGPIO_GetIntStatus(DRVGPIO_PORT port);
int32_t DrvGPIO_GetPortBits(DRVGPIO_PORT port);
int32_t DrvGPIO_GetPortDoutBits(DRVGPIO_PORT port);
int32_t DrvGPIO_GetVersion (void);
int32_t DrvGPIO_InitFunction(DRVGPIO_FUNC function);
int32_t DrvGPIO_Open(DRVGPIO_PORT port,int32_t i32Bit,DRVGPIO_IO mode);
int32_t DrvGPIO_ReadPortMask(DRVGPIO_PORT port);
int32_t DrvGPIO_SetBit(DRVGPIO_PORT port,int32_t i32Bit);
int32_t DrvGPIO_SetBitMask(DRVGPIO_PORT port, int32_t i32Bit);
int32_t DrvGPIO_SetDebounceTime(uint32_t u32DebounceClk,DRVGPIO_DBCLKSRC clockSource);
void DrvGPIO_SetIntCallback(GPIO_GPAB_CALLBACK pfGPABCallback);
int32_t DrvGPIO_SetPortBits(DRVGPIO_PORT port,int32_t i32Data);
int32_t DrvGPIO_SetPortMask(DRVGPIO_PORT port, uint32_t u32Mask);

#endif








