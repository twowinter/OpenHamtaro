/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVSPI_H__
#define __DRVSPI_H__

#include "ISD9xx.h"
#include "System/SysInfra.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVSPI_MAJOR_NUM 1
#define DRVSPI_MINOR_NUM 00
#define DRVSPI_BUILD_NUM 001

#define DRVSPI_VERSION_NUM    _SYSINFRA_VERSION(DRVSPI_MAJOR_NUM,DRVSPI_MINOR_NUM,DRVSPI_BUILD_NUM)  

/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVSPI_ERR_BURST_CNT                 Wrong Burst Number			
// E_DRVSPI_ERR_TRANSMIT_INTERVAL         Wrong Transmit Number
// E_DRVSPI_ERR_BIT_LENGTH                Wrong Bit Length
// E_DRVSPI_ERR_INIT                      Init Fail
// E_DRVSPI_ERR_BUSY                      SPI is busy
// E_DRVSPI_ERR_PORT                      SPI Port does not exist
#define E_DRVSPI_ERR_BURST_CNT            _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 0)
#define E_DRVSPI_ERR_TRANSMIT_INTERVAL    _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 1)
#define E_DRVSPI_ERR_BIT_LENGTH           _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 2)
#define E_DRVSPI_ERR_INIT                 _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 3)
#define E_DRVSPI_ERR_BUSY                 _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 4)
#define E_DRVSPI_ERR_PORT				  _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVSPI, 5)

/*---------------------------------------------------------------------------------------------------------*/
/* SPI Setting                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*PFN_DRVSPI_CALLBACK)(uint32_t userData);
typedef int32_t ERRCODE;
typedef unsigned char BOOL;

typedef enum {
	eDRVSPI_PORT0
} E_DRVSPI_PORT;

typedef enum {
	eDRVSPI_MASTER,
	eDRVSPI_SLAVE,
	eDRVSPI_JOYSTICK
} E_DRVSPI_MODE;

typedef enum {
	eDRVSPI_TYPE0,
	eDRVSPI_TYPE1,
	eDRVSPI_TYPE2,
	eDRVSPI_TYPE3,
	eDRVSPI_TYPE4,
	eDRVSPI_TYPE5,
	eDRVSPI_TYPE6,
	eDRVSPI_TYPE7
} E_DRVSPI_TRANS_TYPE;

typedef enum {
	eDRVSPI_LSB_FIRST,
	eDRVSPI_MSB_FIRST
} E_DRVSPI_ENDIAN;

typedef enum {
	eDRVSPI_EDGE_TRIGGER,
	eDRVSPI_LEVEL_TRIGGER
} E_DRVSPI_SSLTRIG;

typedef enum {
	eDRVSPI_ACTIVE_LOW_FALLING,
	eDRVSPI_ACTIVE_HIGH_RISING
} E_DRVSPI_SS_ACT_TYPE;

typedef enum {
	eDRVSPI_NONE,
	eDRVSPI_SS0,
	eDRVSPI_SS1,
	eDRVSPI_SS0_SS1
} E_DRVSPI_SLAVE_SEL;

typedef enum {
	eDRVSPI_JOYSTICK_CS_ACTIVE,
	eDRVSPI_JOYSTICK_DATA_READY,
	eDRVSPI_JOYSTICK_CS_DEACT,
	eDRVSPI_JOYSTICK_NONE
} E_DRVSPI_JOYSTICK_INT_FLAG;

typedef enum {
	eDRVSPI_JOYSTICK_TRANSMIT_MODE,
	eDRVSPI_JOYSTICK_RECEIVE_MODE
} E_DRVSPI_JOYSTICK_RW_MODE;

typedef enum {
	eDRVSPI_TX_DMA,
	eDRVSPI_RX_DMA
} E_DRVSPI_DMA_MODE;

typedef struct {
	 E_DRVSPI_SLAVE_SEL 	eSlaveSel;
	 E_DRVSPI_SS_ACT_TYPE 	eSlaveActiveLevel;
	 BOOL 					bASSEnable;
	 E_DRVSPI_SSLTRIG 		eSlaveTriggerType;
} S_DRVSPI_SSR;

/*---------------------------------------------------------------------------------------------------------*/
/* SPI Port                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SPI_PORT0 0
//#define SPI_PORT1 1
//#define SPI_PORT2 2
//#define SPI_PORT3 3

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_Open(E_DRVSPI_PORT eSpiPort, E_DRVSPI_MODE eMode, E_DRVSPI_TRANS_TYPE eType, int32_t i32BitLength);
void DrvSPI_Close(E_DRVSPI_PORT eSpiPort);
void DrvSPI_Set2BitSerialDataIOMode(E_DRVSPI_PORT eSpiPort, BOOL bEnable);
void DrvSPI_SetEndian(E_DRVSPI_PORT eSpiPort, E_DRVSPI_ENDIAN eEndian);
ERRCODE DrvSPI_SetBitLength(E_DRVSPI_PORT eSpiPort, int32_t i32BitLength);
ERRCODE DrvSPI_SetByteSleep(E_DRVSPI_PORT eSpiPort, BOOL bEnable);
ERRCODE DrvSPI_SetByteEndian(E_DRVSPI_PORT eSpiPort, BOOL bEnable);

void DrvSPI_SetSlaveSelectMode(E_DRVSPI_PORT eSpiPort, S_DRVSPI_SSR *psSSMode);
void DrvSPI_SetTriggerMode(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SSLTRIG eSSTriggerMode);
void DrvSPI_SetSlaveSelectActiveLevel(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SS_ACT_TYPE eSSActType);
BOOL DrvSPI_GetLevelTriggerStatus(E_DRVSPI_PORT eSpiPort);
void DrvSPI_EnableAutoCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel);
void DrvSPI_DisableAutoCS(E_DRVSPI_PORT eSpiPort);
void DrvSPI_SetCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel);
void DrvSPI_ClrCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel);

BOOL DrvSPI_Busy(E_DRVSPI_PORT eSpiPort);
ERRCODE DrvSPI_BurstTransfer(E_DRVSPI_PORT eSpiPort, int32_t i32BurstCnt, int32_t i32Interval);
uint32_t DrvSPI_SetClock(E_DRVSPI_PORT eSpiPort, uint32_t u32Clock1, uint32_t u32Clock2);
uint32_t DrvSPI_GetClock1(E_DRVSPI_PORT eSpiPort);
uint32_t DrvSPI_GetClock2(E_DRVSPI_PORT eSpiPort);
void DrvSPI_SetVariableClockPattern(E_DRVSPI_PORT eSpiPort, uint32_t u32Pattern);
void DrvSPI_SetVariableClockFunction(E_DRVSPI_PORT eSpiPort, BOOL bEnable);
void DrvSPI_EnableInt(E_DRVSPI_PORT eSpiPort, PFN_DRVSPI_CALLBACK pfnCallback, uint32_t u32UserData);
void DrvSPI_DisableInt(E_DRVSPI_PORT eSpiPort);

BOOL DrvSPI_SingleReadWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data, uint32_t pu32DataIn);
BOOL DrvSPI_SingleRead(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data);
BOOL DrvSPI_SingleWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data);
BOOL DrvSPI_BurstRead(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf);
BOOL DrvSPI_BurstWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf);
uint32_t DrvSPI_DumpRxRegister(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf, uint32_t u32DataCount);
uint32_t DrvSPI_SetTxRegister(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf, uint32_t u32DataCount);
void DrvSPI_SetGo(E_DRVSPI_PORT eSpiPort);

E_DRVSPI_JOYSTICK_INT_FLAG DrvSPI_GetJoyStickIntType(E_DRVSPI_PORT eSpiPort);
void DrvSPI_SetJoyStickStatus(E_DRVSPI_PORT eSpiPort, BOOL bReady);
E_DRVSPI_JOYSTICK_RW_MODE DrvSPI_GetJoyStickMode(E_DRVSPI_PORT eSpiPort);
void DrvSPI_StartPDMA(E_DRVSPI_PORT eSpiPort, E_DRVSPI_DMA_MODE eDmaMode, BOOL bEnable);
uint32_t DrvSPI_GetVersion(void);

#endif
