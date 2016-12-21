/****************************************************************
 *                                                              *
 *  Copyright (c) Nuvoton Technology Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/
 
#ifndef __SDCARD_H__
#define __SDCARD_H__

// Include header file
#include "Platform.h"
#include "SysInfra.h"

#ifdef  __cplusplus
extern "C" {
#endif	// __cplusplus

#define SDCARD_MAJOR_NUM 5
#define SDCARD_MINOR_NUM 0
#define SDCARD_BUILD_NUM 0
#define SDCARD_VERSION_NUM    _SYSINFRA_VERSION(SDCARD_MAJOR_NUM, SDCARD_MINOR_NUM, SDCARD_BUILD_NUM)

// ------------------------------------------------------------------------------
// Define the Error Code
// ------------------------------------------------------------------------------
// E_SDCARD_INITIAL_FAIL				Invalid SDCard Fail
// E_SDCARD_READ_ERROR					Read Data From SDCard Fail
// E_SDCARD_READ_BUSY						Read Data Busy
// E_SDCARD_WRITE_ERROR					Write Data to SDCard Fail
// E_SDCARD_WRITE_BUSY 					Write Data Busy
#define E_SDCARD_INITIAL_FAIL        _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SDCARD, 1)
#define E_SDCARD_READ_ERROR          _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SDCARD, 2)
#define E_SDCARD_READ_BUSY           _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SDCARD, 3)
#define E_SDCARD_WRITE_ERROR         _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SDCARD, 4)
#define E_SDCARD_WRITE_BUSY          _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SDCARD, 5)

typedef void (*PFN_SDCARD_DELAY) (UINT32);
typedef void (*PFN_SDCARD_WAIT_BUSY_CB) (void);

//---------------------------------------------------------------
// SD Card Type 
//---------------------------------------------------------------
typedef enum{
	eTYPE_UNKNOWN_CARD = 0,
	eTYPE_SD_CARD,
	eTYPE_MMC_CARD,
	eTYPE_SDIO_CARD,
	eTYPE_SD20_HIGH_CARD
} E_SDCAR_TYPE;

typedef struct
{
	SPI_T   *psSpiHandler;				// SPI access handler
	BOOL 	bCmd8Accepted;				// Send cmd8 successfully or not
	UINT32  u32SDCardSize;				// SDCard memory size
	UINT32	u32RType;					// SDCard response type
	UINT8   u8SlaveDevice;              // SDCard is on device1/2
	UINT8   u8ReadDataCunt;             // SDCard read count must be less than 64 byte
	UINT8	u8aRspBuf[16];				// Command response buffer
	E_SDCAR_TYPE   eType;				// SDCard type
	PFN_SDCARD_WAIT_BUSY_CB	pfnSDCardWaitBusyCB;
} S_SDCARD_HANDLER;

/*******************************************************************/
/*             Read API declaration                                */
/*******************************************************************/
void 
SDCard_Read(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr, 
	UINT32 u32SecCnt, 
	PUINT8 pu8Buffer
);

BOOL
SDCard_ReadStart(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr
);

void SDCard_ReadStartToken(S_SDCARD_HANDLER *psSDCardHandler);

void
SDCard_ReadData(
	S_SDCARD_HANDLER *psSDCardHandler,
	PUINT32 pu32Buffer
);

BOOL
SDCard_ReadEnd(S_SDCARD_HANDLER *psSDCardHandler);

/*******************************************************************/
/*             Write API declaration                               */
/*******************************************************************/
void 
SDCard_Write(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr,
	UINT32 u32SecCnt,
	PUINT8 pu8Buffer
);

BOOL
SDCard_WriteStart(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr
);

void SDCard_WriteStartToken(S_SDCARD_HANDLER *psSDCardHandler);

void SDCard_WriteData(
	S_SDCARD_HANDLER *psSDCardHandler,
	PUINT32 pu32Buffer
);

void SDCard_WriteCRC(S_SDCARD_HANDLER *psSDCardHandler);

BOOL
SDCard_CheckWriteEndBusy(S_SDCARD_HANDLER *psSDCardHandler);

BOOL
SDCard_WriteEnd(S_SDCARD_HANDLER *psSDCardHandler);

/*******************************************************************/
/*             Protection API declaration                          */
/*******************************************************************/
void SDCard_RegWaitBusyCallBackFunc(
	S_SDCARD_HANDLER *psSDCardHandler,
	PFN_SDCARD_WAIT_BUSY_CB		pfnSDCardWaitBusyCB
);

ERRCODE 
SDCard_Open(
	SPI_T *psSpiHandler,
	UINT8 u8DrvSlaveDevice,
	UINT32 u32SpiClk,
	S_SDCARD_HANDLER *psSDCardHandler
);

void SDCard_Close(S_SDCARD_HANDLER *psSDCardHandler);

void
SDCard_WaitBusy(S_SDCARD_HANDLER *psSDCardHandler);

UINT32 
SDCard_GetCardSize(
	S_SDCARD_HANDLER *psSDCardHandler
);

E_SDCAR_TYPE 
SDCard_GetCardType(S_SDCARD_HANDLER *psSDCardHandler);

static __inline
UINT32
SDCard_GetSPIClock(S_SDCARD_HANDLER *psSDCardHandler)
{
	return SPI_GetBusClock(psSDCardHandler->psSpiHandler);
}

UINT32 
SDCard_GetVersion(void);

#ifdef  __cplusplus
}
#endif	// __cplusplus

#endif //__SDCARD_H__




