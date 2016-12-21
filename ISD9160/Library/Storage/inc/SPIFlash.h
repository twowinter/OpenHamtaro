/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

// Include header file
#include "Platform.h"
#include "SysInfra.h"


#ifdef  __cplusplus
extern "C"
{
#endif

// Version number
#define SPIFLASH_MAJOR_NUM	5
#define SPIFLASH_MINOR_NUM	0
#define SPIFLASH_BUILD_NUM	0
#define SPIFLASH_VERSION_NUM	_SYSINFRA_VERSION(SPIFLASH_MAJOR_NUM, SPIFLASH_MINOR_NUM, SPIFLASH_BUILD_NUM)

// ------------------------------------------------------------------------------
// Define the Error Code
// ------------------------------------------------------------------------------
// E_SPIFLASH_BUSY				Read/Write Data Busy
#define E_SPIFLASH_BUSY         _SYSINFRA_ERRCODE(TRUE, MODULE_ID_SPIFLASH, 1)

// SPI Flash Status1
#define SPIFLASH_SPR			0x80	// Status Register Protect
#define SPIFLASH_R				0x40	// Reserved Bit
#define SPIFLASH_BP3     		0x20	// Block Protect Bit 3
#define SPIFLASH_BP2			0x10	// Block Protect Bit 2
#define SPIFLASH_BP1			0x08	// Block Protect Bit 1
#define SPIFLASH_BP0			0x04	// Block Protect Bit 0
#define SPIFLASH_WEL			0x02	// Write Enable Latch
#define SPIFLASH_BUSY			0x01	// BUSY
#define SPIFLASH_BP				(SPIFLASH_BP3|SPIFLASH_BP2|SPIFLASH_BP1|SPIFLASH_BP0)

// SPI Flash Command
#define SPIFLASH_ZERO			0x00
#define SPIFLASH_DUMMY			0xFF
#define SPIFLASH_WRITE_ENABLE	0x06
#define SPIFLASH_WRITE_DISABLE	0x04
#define SPIFLASH_READ_STATUS	0x05
#define SPIFLASH_WRITE_STATUS	0x01
#define SPIFLASH_READ_DATA		0x03 // Using fast read to replace normal read
#define SPIFLASH_FAST_READ		0x0B
#define SPIFLASH_FAST_RD_DUAL	0x3B
#define SPIFLASH_PAGE_PROGRAM	0x02
#define SPIFLASH_QPAGE_PROGRAM  0x32
#define SPIFLASH_64K_ERASE		0xD8
#define SPIFLASH_4K_ERASE		0x20
#define SPIFLASH_32K_ERASE		0x52
#define SPIFLASH_SUSPEND_EP		0x75
#define SPIFLASH_RESUME_EP		0x7A
#define SPIFLASH_CHIP_ERASE		0xC7
#define SPIFLASH_POWER_DOWN		0xB9
#define SPIFLASH_RELEASE_PD_ID	0xAB
#define SPIFLASH_DEVICE_ID		0x90
#define SPIFLASH_JEDEC_ID		0x9F

#define SPIFLASH_PAGE_SIZE	256

#define SPIFLASH_FLAG_ERASE_64K			0x01
#define SPIFLASH_FLAG_ERASE_4K			0x02
#define SPIFLASH_FLAG_ERASE_32K			0x04
#define SPIFLASH_FLAG_DUAL_SPI			0x08
#define SPIFLASH_FLAG_QUAD_SPI			0x09
#define SPIFLASH_FLAG_WINBOND			0x10
#define SPIFLASH_FLAG_MXIC				0x20
#define SPIFLASH_FLAG_ATMEL				0x40

typedef struct
{
	SPI_T   *psSpiHandler;				// SPI access handler
	UINT32  u32FlashSize;				// SPIFlash memory size
	UINT8   u8SlaveDevice;              // SPIFlash is on device1/2
	UINT8   u8Flag;
} S_SPIFLASH_HANDLER;

typedef enum
{
	eSTATUS_REG1 = 0x00,
	eSTATUS_REG2 = 0x30
} E_SPIFLASH_STATUS_REGISTER;

// APIs declaration


/*******************************************************************/
/*             Write API declaration                               */
/*******************************************************************/
void
SPIFlash_Write(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32Addr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

void
SPIFlash_WritePage(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32PageAddr,
	PUINT8 pau8Data
);

void
SPIFlash_WriteStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr
);

void
SPIFlash_WriteEnd(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

UINT32
SPIFlash_WriteData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32SPIAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

void
SPIFlash_WriteDataAlign(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data
);

/*******************************************************************/
/*             Read API declaration                                */
/*******************************************************************/
void
SPIFlash_Read(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

void
SPIFlash_BurstRead(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

void
SPIFlash_ReadStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr
);

void
SPIFlash_ReadEnd(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

void
SPIFlash_ReadData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

void
SPIFlash_ReadDataAlign(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data,
	UINT32 u32DataLen
);

/*******************************************************************/
/*             Erase API declaration                               */
/*******************************************************************/
void
SPIFlash_Erase(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32CmdAddr,
	UINT32 u32AddIncreament,
	UINT16 u16EraseCount
);

static __inline
void
SPIFlash_Erase64K(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT8  u8IndexOf64K,
	UINT16 u16EraseCount
)
{
	SPIFlash_Erase(psSpiFlashHandler, ((UINT32)SPIFLASH_64K_ERASE<<24)|(u8IndexOf64K<<16), (1<<16), u16EraseCount );
}

static __inline
void
SPIFlash_Erase4K(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT16 u16IndexOf4K,
	UINT16 u16EraseCount
)
{
	SPIFlash_Erase(psSpiFlashHandler, (SPIFLASH_4K_ERASE<<24)|(u16IndexOf4K<<12), (1<<12), u16EraseCount );
}

static __inline
void
SPIFlash_Erase32K(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT16 u16IndexOf32K,
	UINT16 u16EraseCount
)
{
	SPIFlash_Erase(psSpiFlashHandler, (SPIFLASH_32K_ERASE<<24)|(u16IndexOf32K<<15), (1<<15), u16EraseCount );
}

void
SPIFlash_EraseChip(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

// none blocking releate APIs
void
SPIFlash_EraseStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32CmdAddr
);

static __inline
void
SPIFlash_Erase64KStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT8  u8IndexOf64K
)
{
	SPIFlash_EraseStart(psSpiFlashHandler, ((UINT32)SPIFLASH_64K_ERASE<<24)|(u8IndexOf64K<<16));
}

static __inline
void
SPIFlash_Erase4KStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT16 u16IndexOf4K
)
{
	SPIFlash_EraseStart(psSpiFlashHandler, (SPIFLASH_4K_ERASE<<24)|(u16IndexOf4K<<12));
}

static __inline
void
SPIFlash_Erase32KStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT16 u16IndexOf32K
)
{
	SPIFlash_EraseStart(psSpiFlashHandler, (SPIFLASH_32K_ERASE<<24)|(u16IndexOf32K<<15) );
}

void
SPIFlash_EraseChipStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

/*******************************************************************/
/*             Protection API declaration                          */
/*******************************************************************/
void
SPIFlash_ChipWriteEnable(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL bEnableWrite
);

void
SPIFlash_GlobalProtect(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL bEnableGlobalProtect
);

/*******************************************************************/
/*             Miscellaneous API declaration                       */
/*******************************************************************/
void
SPIFlash_Open(
	SPI_T *psSpiHandler,
	UINT8 u8DrvSlaveDevice,
	UINT32 u32SpiClk,
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

static __inline
void
SPIFlash_Close(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	SPI_Close(psSpiFlashHandler->psSpiHandler);
}

static __inline
UINT32
SPIFlash_GetSPIClock(
   S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	return SPI_GetBusClock(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_SendRecOneData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32Data,
	UINT8  u8DataLen
);

void
SPIFlash_GetChipInfo(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

UINT8
SPIFlash_ReadStatusReg(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	E_SPIFLASH_STATUS_REGISTER eStatusReg
);

void
SPIFlash_WriteStatusReg(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT8 u8Status
);

void
SPIFlash_PowerDown(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL	bEnable
);

void
SPIFlash_WaitReady(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

BOOL
SPIFlash_CheckBusy(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
);

UINT32
SPIFlash_GetVersion(void);


#ifdef  __cplusplus
}
#endif

#endif	// __SPIFLASH_H__

