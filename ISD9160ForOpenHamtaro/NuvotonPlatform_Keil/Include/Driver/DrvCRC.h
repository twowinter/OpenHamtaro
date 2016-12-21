/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVCRC_H__
#define __DRVCRC_H__

#include "ISD9xx.h"
#include "System/SysInfra.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVCRC_MAJOR_NUM 1
#define DRVCRC_MINOR_NUM 00
#define DRVCRC_BUILD_NUM 001

#define DRVCRC_VERSION_NUM    _SYSINFRA_VERSION(DRVCRC_MAJOR_NUM,DRVCRC_MINOR_NUM,DRVCRC_BUILD_NUM)  

/*---------------------------------------------------------------------------------------------------------*/
/* Define Error Code                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVSPI_ERR_BURST_CNT                 Wrong Burst Number			
// E_DRVSPI_ERR_TRANSMIT_INTERVAL         Wrong Transmit Number
// E_DRVSPI_ERR_BIT_LENGTH                Wrong Bit Length
// E_DRVSPI_ERR_INIT                      Init Fail
// E_DRVSPI_ERR_BUSY                      SPI is busy
// E_DRVSPI_ERR_PORT                      SPI Port does not exist
#define E_DRVCRC_ERR_ODD_PACKET           _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVCRC, 0)
#define E_DRVCRC_ERR_PACKET_TOO_LONG      _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVCRC, 1)
#define E_DRVCRC_ERR_BIT_LENGTH           _SYSINFRA_ERRCODE(TRUE,  MODULE_ID_DRVCRC, 2)

typedef int32_t ERRCODE;
typedef unsigned char BOOL;

typedef enum {
	eDRVCRC_MSB,
	eDRVCRC_LSB
} E_DRVCRC_ENDIAN;



/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvCRC_Open(void);
ERRCODE DrvCRC_Init(E_DRVCRC_ENDIAN eLSB, int32_t i32PacketLen);
int16_t DrvCRC_Calc( uint32_t *Data, int32_t i32PacketLen);
void DrvCRC_Close(void);
uint32_t DrvCRC_GetVersion(void);

#endif
