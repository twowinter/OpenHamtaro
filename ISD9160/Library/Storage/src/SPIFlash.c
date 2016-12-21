/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/
#include "SPIFlash.h"
#include <string.h>

#if defined ( __CC_ARM )
#pragma O2									// This pragma changes the optimization level, level Om range: O0~O3.
#elif defined ( __ICCARM__ )
#pragma optimize=medium			// optimization level: none, low, medium and high.
#elif defined ( __GNUC__ )
#pragma GCC optimization_level 2 // optimization level range: 0~3.
#endif

// Device ID for support flash type list
#define SPIFLASH_W25P		0x10			// W25P series
#define SPIFLASH_W25X		0x40			// W25X series
#define SPIFLASH_W25Q		0x50			// W25Q series
// W25P series
#define SPIFLASH_W25P10		0x10			// 1M-bits
#define SPIFLASH_W25P20		0x11			// 2M-bits
#define SPIFLASH_W25P40		0x12			// 4M-bits
#define SPIFLASH_W25P80		0x13			// 8M-bits
#define SPIFLASH_W25P16		0x14			// 16M-bits
#define SPIFLASH_W25P32		0x15			// 32M-bits

// W25X series
#define SPIFLASH_W25X10		0x40			// 1M-bits
#define SPIFLASH_W25X20		0x41			// 2M-bits
#define SPIFLASH_W25X40		0x42			// 4M-bits
#define SPIFLASH_W25X80		0x43			// 8M-bits
#define SPIFLASH_W25X16		0x44			//16M-bits
#define SPIFLASH_W25X32		0x45			//32M-bits
#define SPIFLASH_W25X64		0x46			//64M-bits
// W25Q series
#define SPIFLASH_W25Q10		0x50			// 1M-bits
#define SPIFLASH_W25Q20		0x51			// 2M-bits
#define SPIFLASH_W25Q40		0x52			// 4M-bits
#define SPIFLASH_W25Q80		0x53			// 8M-bits
#define SPIFLASH_W25Q16		0x54			//16M-bits
#define SPIFLASH_W25Q32		0x55			//32M-bits
#define SPIFLASH_W25Q64		0x56			//64M-bits
#define SPIFLASH_W25Q128	0x57			//128M-bits

#if (defined (__ISD9300__)) 
#define SPI_SS_NONE		(0x0ul<<SPI_SSCTL_SS_Pos)
#define SPI_SET_SS(spi,u32SS)									( (spi)->SSCTL = ( (spi)->SSCTL & ~SPI_SSCTL_SS_Msk ) | u32SS )
#define SPI_SET_SLAVE_ACTIVE_LEVEL(spi,u32Level)				( (spi)->SSCTL = ( (spi)->SSCTL & ~SPI_SSCTL_SSACTPOL_Msk ) | u32Level )
#define SPI_SET_TX_NUM(spi,u32TxNum) 
#define SPI_WRITE_TX0(spi, u32TxData)		SPI_WRITE_TX(spi, u32TxData)	
#define SPI_WRITE_TX1(spi, u32TxData)		SPI_WRITE_TX(spi, u32TxData)
#define SPI_READ_RX0(spi)					SPI_READ_RX(spi)
#define SPI_READ_RX1(spi)					SPI_READ_RX(spi)
#define SPI_GO(spi)							//SPI_TRIGGER(spi)
#endif

void
SPIFlash_SendRecOneData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32Data,
	UINT8  u8DataLen
)
{
	// Active chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, psSpiFlashHandler->u8SlaveDevice);
		
	// Set transmit Bit Length = u8DataLen
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,u8DataLen);
	// Transmit/Receive Numbers = 1
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	// Write data to TX0 register
	SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,u32Data);

	SPI_GO(psSpiFlashHandler->psSpiHandler);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );

	// Inactive chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, SPI_SS_NONE);
}

BOOL
SPIFlash_CheckBusy(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	return (SPIFlash_ReadStatusReg(psSpiFlashHandler, eSTATUS_REG1)& SPIFLASH_BUSY);
}

void
SPIFlash_WaitReady(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	while( SPIFlash_ReadStatusReg(psSpiFlashHandler, eSTATUS_REG1)& SPIFLASH_BUSY );
}


UINT8
SPIFlash_ReadStatusReg(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	E_SPIFLASH_STATUS_REGISTER eStatusReg
)
{
#if (defined (__ISD9300__))
	SPI_ClearRxFIFO(psSpiFlashHandler->psSpiHandler);
#endif
	SPIFlash_SendRecOneData(psSpiFlashHandler,(SPIFLASH_READ_STATUS|eStatusReg)<<8, 16);

	return (UINT8)SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_WriteStatusReg(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT8 u8Status
)
{
	SPIFlash_ChipWriteEnable(psSpiFlashHandler, TRUE);

	SPIFlash_SendRecOneData(psSpiFlashHandler,(SPIFLASH_WRITE_STATUS<<8)|u8Status, 16);
	SPIFlash_WaitReady(psSpiFlashHandler);
}


UINT32
SPIFlash_GetVersion(void)
{
	return SPIFLASH_VERSION_NUM;
}

void
SPIFlash_Open(
	SPI_T *psSpiHandler,
	UINT8 u8DrvSlaveDevice,
	UINT32 u32SpiClk,
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	psSpiFlashHandler->u8SlaveDevice = u8DrvSlaveDevice;
	psSpiFlashHandler->psSpiHandler = psSpiHandler;

#if (defined (__N572F072__) || defined (__N572P072__) || defined (__N571P032__))
	if (psSpiHandler == SPI0)// Enable high speed pins
	   SYS->GPA_HS = 0x1f;
	// Configure SPI parameters
	// Mode0--> SPI RX latched rising edge of clock; TX latced falling edge of clock; SCLK idle low
	SPI_Open(psSpiFlashHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, u32SpiClk);
#elif (defined (__N572F065__))
	// Configure SPI parameters
	// Mode0--> SPI RX latched rising edge of clock; TX latced falling edge of clock; SCLK idle low
	SPI_Open(psSpiFlashHandler->psSpiHandler, SPI_MODE_0, u32SpiClk);
#elif (defined (__ISD9100__))
	SPI_Open(psSpiFlashHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, u32SpiClk, 0);
#elif (defined (__ISD9300__) )
	SPI_Open(psSpiFlashHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, 8, u32SpiClk);
#endif

	// bit MSB first
	SPI_SET_MSB_FIRST(psSpiFlashHandler->psSpiHandler);
	// send/receve command in big endian; write/read data in little endian
	SPI_DISABLE_BYTE_REORDER(psSpiFlashHandler->psSpiHandler);
	// transmit/receive word will be executed in one transfer
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler, SPI_TXNUM_ONE);
	// defalut width 8 bits
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, 8);
	// set Slave active level as low selection
	SPI_SET_SLAVE_ACTIVE_LEVEL(psSpiFlashHandler->psSpiHandler, SPI_SS_ACTIVE_LOW);
	// set Suspend Interval = 4 SCLK clock cycles for interval between two successive transmit/receive.
	SPI_SET_SUSPEND_CYCLE(psSpiFlashHandler->psSpiHandler, 4);
	
	psSpiFlashHandler->u32FlashSize = 0;
	psSpiFlashHandler->u8Flag = 0;
	// Inactive chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, SPI_SS_NONE);

#if (defined (__ISD9300__)) 
	SPI_TRIGGER(psSpiFlashHandler->psSpiHandler);
#endif
	
}

void
SPIFlash_GetChipInfo(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	UINT8  u8DeviceID, u8ManufacID, u8Flag=0;

	UINT32 u32Value;
	UINT8  u8MemType;
	BOOL   bW25P_SPIFlash=FALSE;
   
	// W25P serious do not support JEDEC ID command
	// therefore, use Get ID command for W25P serious
	// Active chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, psSpiFlashHandler->u8SlaveDevice);

	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,24);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_TWO);
	SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,((UINT32)SPIFLASH_DEVICE_ID<<16));
	SPI_WRITE_TX1(psSpiFlashHandler->psSpiHandler,0);
	SPI_GO(psSpiFlashHandler->psSpiHandler);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	u32Value = SPI_READ_RX1(psSpiFlashHandler->psSpiHandler);
#if (defined (__ISD9300__)) 
	u32Value = SPI_READ_RX1(psSpiFlashHandler->psSpiHandler);
#endif
	u8DeviceID = (UINT8)u32Value;
	u8ManufacID = (UINT8)(u32Value>>8);
	// Inactive all slave devices
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, SPI_SS_NONE);

	// Get JEDEC ID command to detect W25X,W25Q MXIC and ATmel series
	// Only W25P serious not support JEDEC ID command
	SPIFlash_SendRecOneData(psSpiFlashHandler, (UINT32)SPIFLASH_JEDEC_ID<<24, 32);
	u32Value = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);
	if ( ((u32Value>>16)&0xff) == u8ManufacID )
	{
		u8DeviceID = ((UINT8)u32Value) - 1;
		u8MemType  = (UINT8)(u32Value>>8);

		if( ((u32Value>>16)&0xff) == 0xef )		
		{
			u8Flag = SPIFLASH_FLAG_WINBOND;			
			u8DeviceID += u8MemType;

			if ( u8MemType == 0x20 ) // W25P80,W25P32,W25P16
				bW25P_SPIFlash = TRUE;
		}
		else
		{
			u8Flag = SPIFLASH_FLAG_MXIC;
			u8DeviceID += 0x30;
		}
	}
	else if( ((u32Value>>16)&0xff) == 0x1f ) // Atmel SPIFlash
	{		
		u8DeviceID = (((u32Value>>8)&0x1f) - 2 );
		u8Flag = SPIFLASH_FLAG_ATMEL;		
	}

	if ( (u8DeviceID == 0) || ((u8DeviceID & 0xf) > 7) )
	{
		psSpiFlashHandler->u32FlashSize = 0;
		return;
	}
	psSpiFlashHandler->u32FlashSize = 1024*1024/8<<(u8DeviceID&0xf);

	u8DeviceID &= 0xf0;

	if(bW25P_SPIFlash)
	{
		psSpiFlashHandler->u8Flag = u8Flag|SPIFLASH_FLAG_ERASE_64K;	
		return;
	}

	if ( u8DeviceID == SPIFLASH_W25X )
		u8Flag = u8Flag|SPIFLASH_FLAG_ERASE_64K|SPIFLASH_FLAG_ERASE_4K;
	else if ( u8DeviceID == SPIFLASH_W25Q )
		u8Flag = u8Flag|SPIFLASH_FLAG_ERASE_64K|SPIFLASH_FLAG_ERASE_4K|SPIFLASH_FLAG_ERASE_32K;
	else if ( u8DeviceID == SPIFLASH_W25P )
		u8Flag = u8Flag|SPIFLASH_FLAG_ERASE_64K;
	else if( ((u32Value>>16)&0xff) == 0x1f ) // Atmel SPIFlash
		u8Flag = u8Flag|SPIFLASH_FLAG_ERASE_64K|SPIFLASH_FLAG_ERASE_4K|SPIFLASH_FLAG_ERASE_32K;
	else
	{
		psSpiFlashHandler->u32FlashSize = 0;
		return;
	}
	psSpiFlashHandler->u8Flag = u8Flag;
}

void
SPIFlash_ChipWriteEnable(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL bEnableWrite
)
{
	UINT8 u8Cmd;

	if ( bEnableWrite == TRUE )
		u8Cmd = SPIFLASH_WRITE_ENABLE;
	else
		u8Cmd = SPIFLASH_WRITE_DISABLE;
	
	SPIFlash_SendRecOneData(psSpiFlashHandler, u8Cmd, 8);
	SPIFlash_WaitReady(psSpiFlashHandler);
}

void
SPIFlash_GlobalProtect(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL bEnableGlobalProtect
)
{
	UINT8 u8Status=0,u8Flag = ((psSpiFlashHandler->u8Flag)&0xf0);

	if( (u8Flag == SPIFLASH_FLAG_WINBOND) || (u8Flag == SPIFLASH_FLAG_MXIC) )
	{
		u8Status = SPIFlash_ReadStatusReg(psSpiFlashHandler, eSTATUS_REG1);

		if(bEnableGlobalProtect)
			u8Status |= 0x1c; // Set 1 to BP0,BP1,BP2 in Status Register 
		else
		{
			if(u8Status&0x20) // Check BP3/TB bit (some flash have it) 
				u8Status &= 0xc3; // Set 0 to BP0,BP1,BP2,BP3/TB in Status Register
			else
				u8Status &= 0xe3; // Set 0 to BP0,BP1,BP2 in Status Register
		}

		SPIFlash_WriteStatusReg(psSpiFlashHandler, u8Status);
	}
	else if ( u8Flag == SPIFLASH_FLAG_ATMEL )
	{
		if(bEnableGlobalProtect)
			u8Status = 0x7f;	// writing a 7Fh to the first byte of the Status Register will perform a Global Protect
		else					// and keep the SPRL bit in the logical "0" state.
			u8Status = 0x00;	// writing a 00h to the first byte of the Status Register will perform a Global Unprotec
								// without changing the state of the SPRL bit
				
		SPIFlash_WriteStatusReg(psSpiFlashHandler, u8Status);
	}
}

void
SPIFlash_Erase(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32CmdAddr,
	UINT32 u32AddIncreament,
	UINT16 u16EraseCount
)
{
	while(u16EraseCount--)
	{
		SPIFlash_EraseStart(psSpiFlashHandler,u32CmdAddr);
		u32CmdAddr += u32AddIncreament;

		// Wait erase complete
		SPIFlash_WaitReady(psSpiFlashHandler);
	}
}

void
SPIFlash_EraseChip(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	SPIFlash_EraseChipStart(psSpiFlashHandler);
	// Wait erase complete
	SPIFlash_WaitReady(psSpiFlashHandler);
}

void
SPIFlash_PowerDown(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	BOOL	bEnable
)
{
	UINT8 u8Cmd;

	if ( bEnable )
		u8Cmd = SPIFLASH_POWER_DOWN;
	else
		u8Cmd = SPIFLASH_RELEASE_PD_ID;

	SPIFlash_SendRecOneData(psSpiFlashHandler,u8Cmd,8);
}

void
SPIFlash_WriteStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr
)
{

	SPIFlash_ChipWriteEnable(psSpiFlashHandler, TRUE);

	// Active chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, psSpiFlashHandler->u8SlaveDevice);

	// Send write page command
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,32);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,((UINT32)SPIFLASH_PAGE_PROGRAM<<24)|u32ByteAddr);
	SPI_GO(psSpiFlashHandler->psSpiHandler);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	// send/receve command in big endian; write/read data in little endian
	SPI_ENABLE_BYTE_REORDER(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_WriteEnd(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	// Inactive all slave devices
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, SPI_SS_NONE);
	// send/receve command in big endian; write/read data in little endian
	SPI_DISABLE_BYTE_REORDER(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_WritePage(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32PageAddr,
	PUINT8 pau8Data
)
{

	SPIFlash_WriteStart(psSpiFlashHandler, u32PageAddr);
	SPIFlash_WriteDataAlign(psSpiFlashHandler, pau8Data);
	SPIFlash_WriteEnd(psSpiFlashHandler);

	SPIFlash_WaitReady(psSpiFlashHandler);
}

void
SPIFlash_ReadStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr
)
{

	// Active chip select
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, psSpiFlashHandler->u8SlaveDevice);

	// Send fast read command
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,32);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,((UINT32)SPIFLASH_FAST_READ<<24)|(u32ByteAddr&0xffffff));
	SPI_GO(psSpiFlashHandler->psSpiHandler);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	// send dummy clcok
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,8);

#if (defined (__ISD9300__))
	SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,0);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	SPI_ClearRxFIFO(psSpiFlashHandler->psSpiHandler);
#endif
	
	//SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	SPI_GO(psSpiFlashHandler->psSpiHandler);
	while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );

	// send/receve command in big endian; write/read data in little endian
	SPI_ENABLE_BYTE_REORDER(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_ReadEnd(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	// Inactive all slave devices
	SPI_SET_SS(psSpiFlashHandler->psSpiHandler, SPI_SS_NONE);
	// send/receve command in big endian; write/read data in little endian
	SPI_DISABLE_BYTE_REORDER(psSpiFlashHandler->psSpiHandler);
}

void
SPIFlash_ReadData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	//	PA20 CYHuang12 speedup read function.
	UINT32 u32ReadData;
    UINT8  u8ProcBytes;
	
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	u8ProcBytes = ((UINT32)pau8Data)%4;
    if (u8ProcBytes!=0)
    {
        u8ProcBytes = 4 - u8ProcBytes;
        if ( u8ProcBytes > u32DataLen )
        	u8ProcBytes = u32DataLen;
		
		SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler,u8ProcBytes<<3);
#if (defined (__ISD9300__))
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,0);
#endif
	    SPI_GO(psSpiFlashHandler->psSpiHandler);
        u32DataLen-=u8ProcBytes;

        while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
        u32ReadData = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);

        *pau8Data++ = (UINT8)u32ReadData;
        if ( u8ProcBytes >= 2 )
        	*pau8Data++ = (UINT8)(u32ReadData>>8);
        if ( u8ProcBytes >= 3 )
        	*pau8Data++ = (UINT8)(u32ReadData>>16);
    }

    SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, 32);

    while (u32DataLen>=4)
    {
#if (defined (__ISD9300__))
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,0);
#endif
		SPI_GO(psSpiFlashHandler->psSpiHandler);
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
        u32ReadData = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);
        *((UINT32*)pau8Data) = u32ReadData;
        pau8Data+=4;
        u32DataLen-=4;
    }

    if (u32DataLen>0)
    {
        SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, u32DataLen<<3);
#if (defined (__ISD9300__))
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,0);
#endif
	    SPI_GO(psSpiFlashHandler->psSpiHandler);
        while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
        u32ReadData = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);

        *pau8Data++ = (UINT8)u32ReadData;
        if ( u32DataLen >= 2 )
        	*pau8Data++ = (UINT8)(u32ReadData>>8);
        if ( u32DataLen >= 3 )
        	*pau8Data++ = (UINT8)(u32ReadData>>16);
    }
}
	
void
SPIFlash_Read(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	SPIFlash_ReadStart( psSpiFlashHandler, u32ByteAddr );
	// Read data
	SPIFlash_ReadData(psSpiFlashHandler, pau8Data, u32DataLen);
	SPIFlash_ReadEnd(psSpiFlashHandler);
}

void
SPIFlash_ReadDataAlign(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	UINT32 *pu32Temp = (UINT32 *)pau8Data;
	// Read data
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, 32);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_TWO);

	do
	{
#if (defined (__ISD9300__))
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,0);
		SPI_WRITE_TX1(psSpiFlashHandler->psSpiHandler,0);
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
		*(UINT32 *)pu32Temp++ = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);
		*(UINT32 *)pu32Temp++ = SPI_READ_RX1(psSpiFlashHandler->psSpiHandler);
#else
		SPI_GO(psSpiFlashHandler->psSpiHandler);
        while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
		*(UINT32 *)pu32Temp++ = SPI_READ_RX0(psSpiFlashHandler->psSpiHandler);
		*(UINT32 *)pu32Temp++ = SPI_READ_RX1(psSpiFlashHandler->psSpiHandler);
#endif
		u32DataLen -= 8;
	}while(u32DataLen>0);
}

void
SPIFlash_BurstRead(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32ByteAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	SPIFlash_ReadStart( psSpiFlashHandler, u32ByteAddr );
	SPIFlash_ReadDataAlign(psSpiFlashHandler, pau8Data, u32DataLen);
	SPIFlash_ReadEnd(psSpiFlashHandler);
}

/*static void
SPIFlash_SendDataLess4Bytes(
	SPI_T *psSpiHandler,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	UINT32 u32WriteData;
	
	SPI_SET_DATA_WIDTH(psSpiHandler, u32DataLen*8);
	SPI_SET_TX_NUM(psSpiHandler,SPI_TXNUM_ONE);
	u32WriteData = *pau8Data ++;
	if ( u32DataLen >= 2 )
	   u32WriteData |= (*pau8Data ++)<<8;
	if ( u32DataLen == 3 )
	   u32WriteData |= (*pau8Data ++)<<16;
	SPI_WRITE_TX0(psSpiHandler,u32WriteData);
	SPI_GO(psSpiHandler);
	while( SPI_IS_BUSY(psSpiHandler) );
}*/

UINT32
SPIFlash_WriteData(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32SPIAddr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	UINT32 u32WriteCount, u32TotalWriteCount, u32ProcessByte, u32WriteData;
	
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	
	u32WriteCount = 256;
	if ( u32SPIAddr%256 )
		 u32WriteCount -= u32SPIAddr%256;
	if ( u32WriteCount > u32DataLen )
		u32WriteCount = u32DataLen;
	u32TotalWriteCount = u32WriteCount;

	if ( ((UINT32)pau8Data)%4 )
	{
		u32ProcessByte = 4 - ((UINT32)pau8Data)%4;
		if ( u32ProcessByte > u32WriteCount )
			u32ProcessByte = u32WriteCount;
		//SPIFlash_SendDataLess4Bytes(psSpiFlashHandler->psSpiHandler, pau8Data, u32ProcessByte);
		SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, u32ProcessByte*8);
		u32WriteData = *pau8Data ++;
		if ( u32ProcessByte >= 2 )
			u32WriteData |= (*pau8Data ++)<<8;
		if ( u32ProcessByte == 3 )
			u32WriteData |= (*pau8Data ++)<<16;
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,u32WriteData);
		SPI_GO(psSpiFlashHandler->psSpiHandler);
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
		u32WriteCount -=  u32ProcessByte;
		//pau8Data += u32ProcessByte;
	}

	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, 32);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_ONE);
	while(u32WriteCount >= 4)
	{
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,*((PUINT32)pau8Data));
		SPI_GO(psSpiFlashHandler->psSpiHandler);
		pau8Data += 4;
		u32WriteCount -= 4;
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	}
	if (u32WriteCount)
	{
		//SPIFlash_SendDataLess4Bytes(psSpiFlashHandler->psSpiHandler, pau8Data, u32WriteCount);
		SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, u32WriteCount*8);
		u32WriteData = *pau8Data ++;
		if ( u32WriteCount >= 2 )
			u32WriteData |= (*pau8Data ++)<<8;
		if ( u32WriteCount == 3 )
			u32WriteData |= (*pau8Data ++)<<16;
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler,u32WriteData);
		SPI_GO(psSpiFlashHandler->psSpiHandler);
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
		//pau8Data += u32WriteCount;
	}
	return u32TotalWriteCount;
}

void
SPIFlash_Write(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32Addr,
	PUINT8 pau8Data,
	UINT32 u32DataLen
)
{
	UINT32 u32WriteCount;
	
	while(u32DataLen!=0)
	{
		SPIFlash_WriteStart(psSpiFlashHandler, u32Addr);
		u32WriteCount = SPIFlash_WriteData(psSpiFlashHandler,u32Addr, pau8Data, u32DataLen);
		u32Addr += u32WriteCount;
		pau8Data += u32WriteCount;
		u32DataLen -= u32WriteCount;
		SPIFlash_WriteEnd(psSpiFlashHandler);
		// Wait write completely
		SPIFlash_WaitReady(psSpiFlashHandler);
	}
}

void
SPIFlash_EraseStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	UINT32 u32CmdAddr
)
{
	SPIFlash_ChipWriteEnable(psSpiFlashHandler, TRUE);
	SPIFlash_SendRecOneData(psSpiFlashHandler,u32CmdAddr,32);
}

void
SPIFlash_EraseChipStart(
	S_SPIFLASH_HANDLER *psSpiFlashHandler
)
{
	SPIFlash_ChipWriteEnable(psSpiFlashHandler, TRUE);
	SPIFlash_SendRecOneData(psSpiFlashHandler,SPIFLASH_CHIP_ERASE,8);
}

void
SPIFlash_WriteDataAlign(
	S_SPIFLASH_HANDLER *psSpiFlashHandler,
	PUINT8 pau8Data
)
{
	UINT32 u32DataLen;
	UINT32 *pu32Temp = (UINT32 *)pau8Data;
	
	SPI_SET_DATA_WIDTH(psSpiFlashHandler->psSpiHandler, 32);
	SPI_SET_TX_NUM(psSpiFlashHandler->psSpiHandler,SPI_TXNUM_TWO);
	u32DataLen = 256;
	do
	{
		//DrvSPI_BurstWriteData(psSpiHandler,(PUINT32)pau8Data);
		SPI_WRITE_TX0(psSpiFlashHandler->psSpiHandler, *pu32Temp++);
		SPI_WRITE_TX1(psSpiFlashHandler->psSpiHandler, *pu32Temp++);
		SPI_GO(psSpiFlashHandler->psSpiHandler);
		//pau8Data += 8;
		while( SPI_IS_BUSY(psSpiFlashHandler->psSpiHandler) );
	}while(	(u32DataLen -= 8) != 0 );
}

