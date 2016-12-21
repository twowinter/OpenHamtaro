/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _DRVI2C_H
#define _DRVI2C_H

#include "ISD9xx.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVI2C_MAJOR_NUM	1
#define DRVI2C_MINOR_NUM	00
#define DRVI2C_BUILD_NUM	1

/*---------------------------------------------------------------------------------------------------------*/
/*  Version define with SysInfra				                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVI2C_VERSION_NUM     _SYSINFRA_VERSION(DRVI2C_MAJOR_NUM, DRVI2C_MINOR_NUM, DRVI2C_BUILD_NUM)
							   
/*---------------------------------------------------------------------------------------------------------*/
/*  Define Error Code									                                                   */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVI2C_ERR_ARGUMENT  		Wrong Argument
#define E_DRVI2C_ERR_ARGUMENT   _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVI2C, 1)

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C port 								                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum {I2C_PORT0 = 0} E_I2C_PORT;

typedef void (*I2C_CALLBACK)(uint32_t status);

/*---------------------------------------------------------------------------------------------------------*/
/* Define I2C Call back function type                                                               	   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	I2CFUNC   = 0, 
	ARBITLOSS = 1,
	BUSERROR  = 2,
	TIMEOUT	  = 3
} E_I2C_CALLBACK_TYPE;

/*---------------------------------------------------------------------------------------------------------*/
/* Define I2C Call back function Data Struct                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    I2C_CALLBACK	I2CCallBackFn;
    I2C_CALLBACK	ArbitLossCallBackFn;    
    I2C_CALLBACK	BusErrCallBackFn;
    I2C_CALLBACK	TimeoutCallBackFn;

} I2C_CALLBACK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Define I2C functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void     DrvI2C_ClearIntFlag(E_I2C_PORT port);
void     DrvI2C_ClearTimeoutFlag(E_I2C_PORT port);
int32_t  DrvI2C_Close(E_I2C_PORT port);
void     DrvI2C_Ctrl(E_I2C_PORT port, uint8_t u8start, uint8_t u8stop, uint8_t u8intFlag, uint8_t u8ack);
int32_t  DrvI2C_DisableInt(E_I2C_PORT port);
int32_t  DrvI2C_EnableInt(E_I2C_PORT port);
int32_t  DrvI2C_EnableTimeoutCount(E_I2C_PORT port, int32_t i32enable, uint8_t u8div4);
uint32_t DrvI2C_GetClock(E_I2C_PORT port, uint32_t u32clock);
uint8_t  DrvI2C_GetIntFlag(E_I2C_PORT port);
uint32_t DrvI2C_GetStatus(E_I2C_PORT port);
uint32_t DrvI2C_GetVersion(void);
int32_t  DrvI2C_InstallCallback(E_I2C_PORT port, E_I2C_CALLBACK_TYPE Type, I2C_CALLBACK callbackfn);
int32_t  DrvI2C_Open(E_I2C_PORT port, uint32_t u32clock_Hz, uint32_t u32baudrate_Hz);
uint8_t  DrvI2C_ReadData(E_I2C_PORT port);
int32_t  DrvI2C_SetAddress(E_I2C_PORT port, uint8_t u8slaveNo, uint8_t u8slave_addr, uint8_t u8GC_Flag);
int32_t  DrvI2C_SetAddressMask(E_I2C_PORT port, uint8_t u8slaveNo, uint8_t u8slaveAddrMask);
int32_t  DrvI2C_SetClock(E_I2C_PORT port, uint32_t u32clock_Hz, uint32_t u32baudrate_Hz);
int32_t  DrvI2C_UninstallCallBack(E_I2C_PORT port, E_I2C_CALLBACK_TYPE Type);
void     DrvI2C_WriteData(E_I2C_PORT port, uint8_t u8data);

#endif

