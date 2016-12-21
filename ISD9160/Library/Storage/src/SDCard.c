/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Include related header files                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#include "Platform.h"
#include "SDCard.h"

#if defined ( __CC_ARM )
#pragma O2									// This pragma changes the optimization level, level Om range: O0~O3.
#elif defined ( __ICCARM__ )
#pragma optimize=medium			// optimization level: none, low, medium and high.
#elif defined ( __GNUC__ )
#pragma GCC optimization_level 2 // optimization level range: 0~3.
#endif

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

//-----------------------------------------------------------------------------
// Define SD command with its response format:
//	command length(8bits), response number(8bits)
// See:
//		"SD Memory Card Specifications" Part-1
//		"MultiMediaCard Product Manual"
//		"SDIO Card Specification Version 1.0"

#define SD_R1		1						// Reponse 1    //Normal Response Command
#define SD_R2		2						// Reponse 2
#define SD_R3		3						// Reponse 3

#define CRC_7		BIT2
#define SD_R0		0						// No response
#define SD_R1b		CRC_7					// Reponse 1b
#define SD_R4		0						// Reponse 4
#define SD_R5		CRC_7// SDIO: add SD_R5 SDIO.pdf p.16
#define SD_R6		CRC_7					// Reponse 6
#define SD_R7		CRC_7					// Reponse 7


//#define SD_DEF_CMD(Command,Response)		(((Command)<<8)|(Response))
#define SD_DEF_CMD(Command,Response)		(Command)

#define SD_CMD0		SD_DEF_CMD( 0,SD_R0)	/// Reset card to idle state

#define SD_CMD1		SD_DEF_CMD( 1,SD_R3)	// MMC only: Get/Set OCR(Operation Condition Register)
#define SD_CMD2		SD_DEF_CMD( 2,SD_R2)	// Get CID(Card IDentification register)
#define SD_CMD3		SD_DEF_CMD( 3,SD_R6)	// Get RCA(Relative Card Address register)
#define SD_CMD4		SD_DEF_CMD( 4,SD_R0)	// Set DSR(Driver State Register)
#define SD_CMD5		SD_DEF_CMD( 5,SD_R4)	// SDIO Only: Get/Set OCR(Operation Condition Register)
#define SD_CMD6		SD_DEF_CMD( 6,SD_R1)	// SD2.0 card only, for switch High Speed SD clock
#define SD_CMD7		SD_DEF_CMD( 7,SD_R1)	/// Select card

#define SD_CMD8		SD_DEF_CMD( 8,SD_R7)	// Send Interface Condition
#define SD_CMD9		SD_DEF_CMD( 9,SD_R1)	// Get CSD(Card-Specific Data register)
#define SD_CMD12	SD_DEF_CMD(12,SD_R1b)	// Stop transmission
#define SD_CMD16	SD_DEF_CMD(16,SD_R1)	// Set block length

#define SD_CMD17	SD_DEF_CMD(17,SD_R1)	// Read single block
#define SD_CMD18	SD_DEF_CMD(18,SD_R1)	// Read multiple block

#define SD_CMD24	SD_DEF_CMD(24,SD_R1)	// Write single block
#define SD_CMD25	SD_DEF_CMD(25,SD_R1)	// Write multiole block

#define SD_CMD52	SD_DEF_CMD(52,SD_R1)    // SDIO: IO_RW_DIRECT command SDIO.pdf p.15
#define SD_CMD53    SD_DEF_CMD(53,SD_R5)    // SDIO: IO_RW_EXTENED command SDIO.pdf p.17
#define SD_CMD55	SD_DEF_CMD(55,SD_R1)	// Indicate the next command is an
											// application specific command
#define SD_CMD58	SD_DEF_CMD(58,SD_R3)

// Applciation specific commands(must preceed with SD_CMD55)
#define SD_ACMD6	SD_DEF_CMD( 6,SD_R1)	// Set bus witdth
#define SD_ACMD41	SD_DEF_CMD(41,SD_R3)	// SD only:Get/Set OCR(Operation Condition Register)
#define SD_ACMD51	SD_DEF_CMD(51,SD_R1)	// SD only:Get/Set OCR(Operation Condition Register)

#define CMD_TIMEOUT		(0x50)
#define WRITE_TIMEOUT	(0x150000)

void SDCard_RegWaitBusyCallBackFunc(
	S_SDCARD_HANDLER *psSDCardHandler,
	PFN_SDCARD_WAIT_BUSY_CB		pfnSDCardWaitBusyCB
)
{
	psSDCardHandler->pfnSDCardWaitBusyCB = pfnSDCardWaitBusyCB;
}

void
SDCard_WaitBusy(S_SDCARD_HANDLER *psSDCardHandler)
{
	while(SPI_IS_BUSY(psSDCardHandler->psSpiHandler))
	{
		if(psSDCardHandler->pfnSDCardWaitBusyCB!=NULL)
			psSDCardHandler->pfnSDCardWaitBusyCB();
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 GenerateCRC                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 u32Data         Input Data                                                         */
/*               UINT32 u32GenPoly      CRC7:0x1200   CRC16:0x1021                                         */
/*               UINT32 u32Accum        CRC value                                                          */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               UINT32 u32Accum      CRC value                                                            */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to generate CRC value                                               */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32 
GenerateCRC(UINT32 u32Data, UINT32 u32GenPoly, UINT32 u32Accum)
{
	volatile UINT8 i;

	u32Data <<= 8;
	for (i=8; i>0; i--)
	{
		if ((u32Data ^ u32Accum) & 0x8000)
			u32Accum = (u32Accum << 1) ^ u32GenPoly;
		else
			u32Accum <<= 1;
		u32Data <<= 1;
	}
	return u32Accum;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 pfnSDCARD_Delay	                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 nCount                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None   	                                                                               */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to make delay times                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static void pfnSDCARD_Delay(UINT32 nCount)
{
	UINT32 i;
	//if (pfntimedelay!=NULL)
	//{
	//	pfntimedelay(nCount);
	//}else{
		for(;nCount!=0;nCount--)
			for(i=10000;i>0;i--);
	//}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SingleWrite                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 u32Data        Data to send                                                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None			                                                                           */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to send data though SPI to general clock for SDCARD operation       */
/*---------------------------------------------------------------------------------------------------------*/
static void SingleWrite(S_SDCARD_HANDLER *psSDCardHandler, UINT32 u32Data)
{
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,8);
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_ONE);
	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,u32Data);
	SPI_GO(psSDCardHandler->psSpiHandler);
	
	SDCard_WaitBusy(psSDCardHandler);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCmdAndResponse		                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 nCmd          Set command register                                                 */
/*               UINT32 nArg          Set command argument                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               TRUE	get response		                                                               */
/*               FALSE	1.SD Card busy, 2.Card moved, 3.Timeout	                                           */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to Send SDCARD CMD and Receive Response                             */
/*---------------------------------------------------------------------------------------------------------*/
void SendSDCmd_1_NB(S_SDCARD_HANDLER *psSDCardHandler, UINT32 nCmd,UINT32 nArg)
{
	UINT32 volatile i;
	UINT8  u8CmdArg[8];
	PUINT32  pu32CmdArg1,pu32CmdArg2;	
	UINT16 u16CRC7=0;
	
	// Wait Ncs
	u8CmdArg[0] = 0xFF;
	
	// Send Cmd Data (Command+CRC)
	u8CmdArg[1] = 0x40 | nCmd;
	u8CmdArg[2] = (UINT8)((nArg & 0xff000000) >> 24);
	u8CmdArg[3] = (UINT8)((nArg & 0x00ff0000) >> 16);
	u8CmdArg[4] = (UINT8)((nArg & 0x0000ff00) >> 8);
	u8CmdArg[5] = (UINT8)(nArg & 0x000000ff);

	for (i=1; i<=5; i++)		
		u16CRC7 = GenerateCRC(u8CmdArg[i], 0x1200, u16CRC7);

	u16CRC7 = u16CRC7 >> 9;
	u16CRC7 = (u16CRC7 << 1) | 0x01;
	u8CmdArg[6] = (UINT8)(u16CRC7 & 0xff);
	
	// Wait Ncr
	u8CmdArg[7] = 0xFF;
	
	// Active chip select
	SPI_SET_SS(psSDCardHandler->psSpiHandler, psSDCardHandler->u8SlaveDevice);

	pu32CmdArg1 = ((PUINT32)(&u8CmdArg[0]));
	pu32CmdArg2 = ((PUINT32)(&u8CmdArg[4]));

	// Send 2 data each time, Data Count is 32 Bits
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,32);
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_TWO);
	SPI_ENABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);// Enable Little Endian (BYTE_ENDIAN)

	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,*pu32CmdArg1);
	SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,*pu32CmdArg2);

}

static void SendSDCmd_2_NB(S_SDCARD_HANDLER *psSDCardHandler)
{
	SPI_DISABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);	// Disable Little Endian (BYTE_ENDIAN)
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,8);
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_ONE);
	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0x000000FF);	
}

static BOOL 
SendSDCmd_WaitBusy_Rsp_NB(S_SDCARD_HANDLER *psSDCardHandler)
{

	psSDCardHandler->u8aRspBuf[0] = (0x000000FF & SPI_READ_RX0(psSDCardHandler->psSpiHandler));

	if ((psSDCardHandler->u8aRspBuf[0] & 0x80) == 0)
		return TRUE;
	else
	{
		SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,8);
		SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_ONE);
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0x000000FF);		
		return FALSE;			
	}
}

static BOOL 
SendSDCmd_WaitBusy_Token_NB(S_SDCARD_HANDLER *psSDCardHandler, UINT8 u8WaitToken)
{

	psSDCardHandler->u8aRspBuf[0] = (0x000000FF & SPI_READ_RX0(psSDCardHandler->psSpiHandler));

	if (psSDCardHandler->u8aRspBuf[0]  == u8WaitToken)
		return TRUE;
	else
	{
		SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,8);
		SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_ONE);
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0x000000FF);				
		return FALSE;			
	}
}

static BOOL
GetSDCmdResp(S_SDCARD_HANDLER *psSDCardHandler)
{
	BOOL bStatus;
	UINT32 i;

	if (psSDCardHandler->u32RType == SD_R1)
		bStatus =  TRUE;
	else if (psSDCardHandler->u32RType == SD_R2)
	{
		SingleWrite(psSDCardHandler, 0xFFFFFFFF);
		psSDCardHandler->u8aRspBuf[1] = SPI_READ_RX0(psSDCardHandler->psSpiHandler);
		bStatus =  TRUE;				
	}
	else if ( (psSDCardHandler->u32RType == SD_R3) || (psSDCardHandler->u32RType == SD_R7) )
	{
		for (i=1; i<5; i++)
		{
			SingleWrite(psSDCardHandler, 0xFFFFFFFF);
			psSDCardHandler->u8aRspBuf[i] = SPI_READ_RX0(psSDCardHandler->psSpiHandler);									
		}
		
		bStatus =  TRUE;
	}
	else
		bStatus = FALSE;

	// Inactive all slave devices
	SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);

	return bStatus; 
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCmdAndResponse		                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 nCmd          Set command register                                                 */
/*               UINT32 nArg          Set command argument                                                 */
/*               UINT32 nRType        Set Response type                                                    */
/*               UINT32 nTryCount     Set count for timerout                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               TRUE	get response		                                                               */
/*               FALSE	1.SD Card busy, 2.Card moved, 3.Timeout	                                           */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to Send SDCARD CMD and Receive Response                             */
/*---------------------------------------------------------------------------------------------------------*/
static BOOL 
SDCmdAndResponse(S_SDCARD_HANDLER *psSDCardHandler, UINT32 nCmd,UINT32 nArg,UINT32 nRType, UINT32 nTryCount)	
{
	psSDCardHandler->u32RType = nRType;
	
	
#if (defined (__ISD9300__))
	SPI_ClearRxFIFO(psSDCardHandler->psSpiHandler);
#endif
	
	SendSDCmd_1_NB(psSDCardHandler, nCmd, nArg);	
	SPI_GO(psSDCardHandler->psSpiHandler);
	SDCard_WaitBusy(psSDCardHandler);

	SendSDCmd_2_NB(psSDCardHandler);
	SPI_GO(psSDCardHandler->psSpiHandler);
	SDCard_WaitBusy(psSDCardHandler);
	
	while(1)
	{
		if(SendSDCmd_WaitBusy_Rsp_NB(psSDCardHandler) == FALSE)
		{
			SPI_GO(psSDCardHandler->psSpiHandler);
			SDCard_WaitBusy(psSDCardHandler);
			
			if (nTryCount > 0)
			{
				nTryCount--;
				if (nTryCount == 0)
					return FALSE;
			}
		}
		else
			break;
	}	
	
	//---------------------------------------
	// get response
	//---------------------------------------	
	return GetSDCmdResp(psSDCardHandler);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDGetInfo		                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               Get SDCARD Size			                                                               */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to send CMD9 and get relative value to compute SDCARD Size          */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32 
SDGetInfo(S_SDCARD_HANDLER *psSDCardHandler)
{
	UINT32 i;
	UINT32 R_LEN, C_Size, MULT;

	SDCmdAndResponse(psSDCardHandler, SD_CMD9, 0x0, SD_R1, CMD_TIMEOUT);

	// Active chip select
	SPI_SET_SS(psSDCardHandler->psSpiHandler, psSDCardHandler->u8SlaveDevice);

	
	//---------------------------------------
	// wait start token
	//---------------------------------------
	while(1)
	{
		SingleWrite(psSDCardHandler, 0xFF);
		
		if ((UINT8)(SPI_READ_RX0(psSDCardHandler->psSpiHandler)) == 0xfe)
			break;
	}
	//---------------------------------------
	// get data
	//---------------------------------------
	for (i=0; i<16; i++)
	{
		SingleWrite(psSDCardHandler, 0xFF);
		psSDCardHandler->u8aRspBuf[i] = SPI_READ_RX0(psSDCardHandler->psSpiHandler);
	}

	//---------------------------------------
	// delay
	//---------------------------------------
	SingleWrite(psSDCardHandler, 0xFFF);

	// Inactive all slave devices
	SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);

	//---------------------------------------
	// Calculate SD Card Size (kByte)
	//---------------------------------------
	if (psSDCardHandler->eType == eTYPE_SD20_HIGH_CARD)
	{

		C_Size = ((psSDCardHandler->u8aRspBuf[7] & 0x3f) << 16)|
				 ((psSDCardHandler->u8aRspBuf[8] & 0xff) <<  8)|
				 ((psSDCardHandler->u8aRspBuf[9] & 0xff));
				 
		psSDCardHandler->u32SDCardSize = (C_Size+1)*512;
	}
	else   
	{
		R_LEN = (psSDCardHandler->u8aRspBuf[5] & 0x0f);    
		C_Size = ((psSDCardHandler->u8aRspBuf[6] & 0x03) << 10)|
				 ((psSDCardHandler->u8aRspBuf[7] & 0xff) <<  2)|
				 (((psSDCardHandler->u8aRspBuf[8] & 0xc0) >> 6) &0x03);
		MULT = ((psSDCardHandler->u8aRspBuf[9] & 0x03) << 1)|((psSDCardHandler->u8aRspBuf[10] & 0x80) >> 7);
		MULT = 1 << (MULT + 2);
		psSDCardHandler->u32SDCardSize = ((C_Size+1) * MULT) * (1 << R_LEN);
		psSDCardHandler->u32SDCardSize = psSDCardHandler->u32SDCardSize/1024;	
	}
	
	return psSDCardHandler->u32SDCardSize;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCmdReset                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None		                                                                               */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to send CMD0(RESET) to enter SPI mode and reset system              */
/*---------------------------------------------------------------------------------------------------------*/
static BOOL 
SDCmdReset(S_SDCARD_HANDLER *psSDCardHandler)
{

  volatile UINT16 u8CmdCnt_2=1000;
	volatile UINT16 ii;

	//while(u8CmdCnt_2)
	//{
		// Inactive all slave devices
		SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);
		pfnSDCARD_Delay(0x20);	
		

		//--------------------------------------------------------
		//	Send 74 SD clcok in SD mode for Toshiba SD Card
		//--------------------------------------------------------
		for (ii=0; ii<10; ii++)
		{
			SingleWrite(psSDCardHandler, 0xFFFFFFFF);
		}

		pfnSDCARD_Delay(0x20);		
		
		// send RESET command (CMD_0)
		// Active chip select
		SPI_SET_SS(psSDCardHandler->psSpiHandler, psSDCardHandler->u8SlaveDevice);

		pfnSDCARD_Delay(0x20);		

		//--------------------------------------------------------
		//	1. Transcend 64MB SD card MUST add Ncs in RESET command.
		//	2. SanDisk 256MB  SD card can't add Ncs in RESET command.
		//	3. A Valid Reset Command Is 0x40,0x00,0x00,0x00,0x00,0x95
		//--------------------------------------------------------
		SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler,8);
		SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler,SPI_TXNUM_TWO);
		
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffff40);
		SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffff00);
		SPI_GO(psSDCardHandler->psSpiHandler);
	
		SDCard_WaitBusy(psSDCardHandler);
		
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffff00);
		SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffff00);
		SPI_GO(psSDCardHandler->psSpiHandler);
	
		SDCard_WaitBusy(psSDCardHandler);	
		
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffff00);
		SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffff95);
		SPI_GO(psSDCardHandler->psSpiHandler);
	
		SDCard_WaitBusy(psSDCardHandler);	
			
		//--------------------------------------------------------
		// wait Ncr
		//--------------------------------------------------------
		SingleWrite(psSDCardHandler, 0xFFFFFFFF);
			
		//--------------------------------------------------------
		// wait response
		//--------------------------------------------------------
#if (defined (__ISD9300__))
		SPI_ClearRxFIFO(psSDCardHandler->psSpiHandler);
#endif
		while(u8CmdCnt_2)
		{
			SingleWrite(psSDCardHandler, 0xFFFFFFFF);
			
			psSDCardHandler->u8aRspBuf[0] = SPI_READ_RX0(psSDCardHandler->psSpiHandler);

			if ((psSDCardHandler->u8aRspBuf[0] & 0x80) == 0)
			{				
				// Inactive all slave devices
				SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);
				pfnSDCARD_Delay(0x10);
				return TRUE;				
			}
			u8CmdCnt_2--;
		}
	//}
	return FALSE;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDInit	                                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               VOID                                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               TRUE: SD initial success			                                                       */
/*               FALSE : SD initial fail			                                                       */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to initial SDCARD flow according SDCARD spec.                       */
/*---------------------------------------------------------------------------------------------------------*/
static BOOL 
SDInit(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32	u32SpiClock
)
{
	//--------------------------------------------------------
  // Switch SPI Clcok to Lower Speed	(300k Hz)
  //--------------------------------------------------------

	SPI_SetBusClock(psSDCardHandler->psSpiHandler, 300000);

	
	pfnSDCARD_Delay(0x10);	
	
	//--------------------------------------------------------
	//					INITIALIZE SD card
	//
	// send Reset command
	//--------------------------------------------------------
	if(SDCmdReset(psSDCardHandler)==FALSE)
		return FALSE;

	if (SDCmdAndResponse(psSDCardHandler, SD_CMD8, 0x000001AA, SD_R7, 0x1000) == TRUE)
	{
		if ((psSDCardHandler->u8aRspBuf[0] & 0xFE) == 0x00)
		{
			if (psSDCardHandler->u8aRspBuf[4] == 0xAA)
				psSDCardHandler->bCmd8Accepted = TRUE;
		}
	}

	//-------------------------------------------------------------------- 
	// Send ACMD_41 for 		
	// 1. Send Host capacity support information, HCS=1 (Bit[30])
	// 2. Acitvates the card's initialization process
	//--------------------------------------------------------------------
	do
	{
 		if (SDCmdAndResponse(psSDCardHandler, SD_CMD55, 0x00, SD_R1, 0x1000) == FALSE)
			return FALSE;

		if (psSDCardHandler->bCmd8Accepted == TRUE)
		{
			if (!(SDCmdAndResponse(psSDCardHandler, SD_ACMD41, 0x40000000, SD_R1, CMD_TIMEOUT)))
				return FALSE;		
		}			
		else
		{
			if (!(SDCmdAndResponse(psSDCardHandler, SD_ACMD41, 0x00, SD_R1, CMD_TIMEOUT)))
				return FALSE;
		}
		
	}while(psSDCardHandler->u8aRspBuf[0] != 0);
	

	//--------------------------------------------------------------------
	// Send CMD_58 for 
	// 1. Wait SD card ready
	// 2. Get SD card capacity inforamtion (CCS)
	//--------------------------------------------------------------------	
	psSDCardHandler->eType = eTYPE_SD_CARD;
	
	do
	{
	
		if (SDCmdAndResponse(psSDCardHandler, SD_CMD58, 0x0, SD_R3, CMD_TIMEOUT) == FALSE)
			return FALSE;	
		
	}while((psSDCardHandler->u8aRspBuf[1] & 0x80) == 0);
	
	if ((psSDCardHandler->u8aRspBuf[1] & 0x40) == 0x40)	// check if card is SDHC card
		psSDCardHandler->eType = eTYPE_SD20_HIGH_CARD;
	
	if (SDGetInfo(psSDCardHandler) == 0)
		return FALSE;

	//--------------------------------------------------------------------					
	// Switch SPI Clcok to Higher Speed
	//--------------------------------------------------------------------
	SPI_SetBusClock(psSDCardHandler->psSpiHandler, u32SpiClock);

	pfnSDCARD_Delay(0x10);
	
	return TRUE;
 }

ERRCODE 
SDCard_Open(
	SPI_T *psSpiHandler,
	UINT8 u8DrvSlaveDevice,
	UINT32 u32SpiClk,
	S_SDCARD_HANDLER *psSDCardHandler	
)
{
	psSDCardHandler->psSpiHandler = psSpiHandler;
	psSDCardHandler->u8SlaveDevice = u8DrvSlaveDevice;
	
#if (defined (__N572F072__) || defined (__N572P072__) || defined (__N571P032__))
	if (psSpiHandler == SPI0)// Enable high speed pins
	  SYS->GPA_HS = 0x1f;
	// Configure SPI parameters
	// Mode0--> SPI RX latched rising edge of clock; TX latced falling edge of clock; SCLK idle low
	SPI_Open(psSDCardHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, u32SpiClk);
#elif (defined (__N572F065__))
	// Configure SPI parameters
	// Mode0--> SPI RX latched rising edge of clock; TX latced falling edge of clock; SCLK idle low
	SPI_Open(psSDCardHandler->psSpiHandler, SPI_MODE_0, u32SpiClk);
#elif (defined (__ISD9100__))
	SPI_Open(psSDCardHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, u32SpiClk, 0);
#elif (defined (__ISD9300__) )
	SPI_Open(psSDCardHandler->psSpiHandler, SPI_MASTER, SPI_MODE_0, 8, u32SpiClk);
#endif

	// bit MSB first
	SPI_SET_MSB_FIRST(psSDCardHandler->psSpiHandler);
	// send/receve command in big endian; write/read data in little endian
	SPI_DISABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);
	// transmit/receive word will be executed in one transfer
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_ONE);
	// defalut width 8 bits
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 8);
	// set Slave active level as low selection
	SPI_SET_SLAVE_ACTIVE_LEVEL(psSDCardHandler->psSpiHandler, SPI_SS_ACTIVE_LOW);
	// set Suspend Interval = 3 SCLK clock cycles for interval between two successive transmit/receive.
	SPI_SET_SUSPEND_CYCLE(psSDCardHandler->psSpiHandler, 3);

	// Inactive chip select
	SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);
	
#if (defined (__ISD9300__)) 
	SPI_TRIGGER(psSDCardHandler->psSpiHandler);
#endif

	psSDCardHandler->u32SDCardSize = 0;
	psSDCardHandler->eType = eTYPE_UNKNOWN_CARD;
	psSDCardHandler->bCmd8Accepted = FALSE;
	psSDCardHandler->pfnSDCardWaitBusyCB = 0;
	
	if (SDInit(psSDCardHandler, u32SpiClk))
		return E_SUCCESS;
	else
		return E_SDCARD_INITIAL_FAIL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCard_Close                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None		                                                                               */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to close SDCARD                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void SDCard_Close(S_SDCARD_HANDLER *psSDCardHandler)
{
	SPI_Close(psSDCardHandler->psSpiHandler);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCard_GetCardSize                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               PUINT32 pu32TotCardSize                                                                   */
/*               			                                                                               */
/* Returns:                                                                                                */
/*               TRUE	The size is already saved                                                          */
/*               FALSE	The size is zero                                                                   */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to get card total sector after SDCARD is opened                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 
SDCard_GetCardSize(
	S_SDCARD_HANDLER *psSDCardHandler
)
{
	
	return (psSDCardHandler->u32SDCardSize);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCard_Read		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 u32SectorAddr     Sector Address                                                   */
/*               UINT32 u32SecCnt         Sector Count                                                     */
/*               PUINT8 pu8Buffer         A buffer to receive data                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               			                                                                               */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to Read Multi-Sector Data                                           */
/*---------------------------------------------------------------------------------------------------------*/
BOOL
SDCard_CheckWriteEndBusy(S_SDCARD_HANDLER *psSDCardHandler)
{
	SDCard_WaitBusy(psSDCardHandler);

	if( SendSDCmd_WaitBusy_Token_NB(psSDCardHandler, 0xFF) == FALSE )
	{
		SPI_GO(psSDCardHandler->psSpiHandler);
		return FALSE;
	}

	return TRUE;
}

BOOL
SDCard_ReadStart(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr
)
{
	UINT32 volatile u32Addr;

	//-----------------------------------
	// Buffer to SD Multiple block read
	//-----------------------------------
	if (psSDCardHandler->eType != eTYPE_SD20_HIGH_CARD)
		u32Addr = 512 * u32SecAddr;
	else		
		u32Addr = u32SecAddr;

	if(SDCmdAndResponse(psSDCardHandler, SD_CMD18, u32Addr, SD_R1, CMD_TIMEOUT)==FALSE)	// multi-block read command
		return FALSE;	

	SPI_SET_SS(psSDCardHandler->psSpiHandler, psSDCardHandler->u8SlaveDevice);
	//-----------------------------------
	// NAc => at least 8 clocks (at most is defined CSD)
	//-----------------------------------
	SingleWrite(psSDCardHandler, 0xFFFFFFFF);
	
	return TRUE;
}

void SDCard_ReadStartToken(S_SDCARD_HANDLER *psSDCardHandler)
{
	SPI_DISABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);
	//-----------------------------------	
	// wait start token
	//-----------------------------------
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_ONE);
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 8);	

#if (defined (__ISD9300__))
	SPI_ClearRxFIFO(psSDCardHandler->psSpiHandler);
#endif
	
	while(1)
	{				
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffffff);
		SPI_GO(psSDCardHandler->psSpiHandler);

		SDCard_WaitBusy(psSDCardHandler);

		if ((UINT8)(SPI_READ_RX0(psSDCardHandler->psSpiHandler)) == 0xfe)
			break;
	}
	
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_TWO);
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 32);

	SPI_ENABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);

	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffffff);
	SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffffff);
	SPI_GO(psSDCardHandler->psSpiHandler);

	SDCard_WaitBusy(psSDCardHandler);

	psSDCardHandler->u8ReadDataCunt = 0;

}

void
SDCard_ReadData(
	S_SDCARD_HANDLER *psSDCardHandler,
	PUINT32 pu32Buffer
)
{
	//if(g_u8ReadDataCunt<64)
	//{
		//*pu32Buffer++ = SPI_READ_RX0(psSDCardHandler->psSpiHandler);
		//*pu32Buffer	  = SPI_READ_RX1(psSDCardHandler->psSpiHandler);

		//g_u8ReadDataCunt++;

		//if(g_u8ReadDataCunt<64)
		//{
		//	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffffff);
		//	SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffffff);
		//	SPI_GO(psSDCardHandler->psSpiHandler);
		//}
	//	return TRUE;
	//}
	//else
	//	return FALSE;
	
	*pu32Buffer++ = SPI_READ_RX0(psSDCardHandler->psSpiHandler);
	*pu32Buffer	  = SPI_READ_RX1(psSDCardHandler->psSpiHandler);
	psSDCardHandler->u8ReadDataCunt++;
	if (psSDCardHandler->u8ReadDataCunt < 64)
	{
		SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffffff);
		SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,0xffffffff);
		SPI_GO(psSDCardHandler->psSpiHandler);
	}
}

BOOL
SDCard_ReadEnd(S_SDCARD_HANDLER *psSDCardHandler)
{
	SPI_DISABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);	// Disable Little Endian (BYTE_ENDIAN)

	if (SDCmdAndResponse(psSDCardHandler,12, 0x00000000, 1, CMD_TIMEOUT) == FALSE)		
		return FALSE;
			
	// Inactive chip select
	SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);
	return TRUE;
}

void 
SDCard_Read(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr, 
	UINT32 u32SecCnt, 
	PUINT8 pu8Buffer
)
{

	UINT32 volatile i;
	UINT32 volatile u32Addr, u32Cnt = 0;
	UINT32 *pu32Ptr;


	SDCard_ReadStart(psSDCardHandler, u32SecAddr);

	pu32Ptr = (UINT32 *)pu8Buffer;

	while(u32Cnt < u32SecCnt)
	{
		
		SDCard_ReadStartToken(psSDCardHandler);

		for (i=0; i<512/8; i++)
		{
			SDCard_ReadData(psSDCardHandler, pu32Ptr);

			SDCard_WaitBusy(psSDCardHandler);

			pu32Ptr+=2;
		}			
		
		u32Cnt++;
	}
	
	SDCard_ReadEnd(psSDCardHandler);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCard_Write		                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               UINT32 u32SecAddr     Sector Address                                            		   */
/*               UINT32 u32SecCnt      Sector Count                                                        */
/*               PUINT8 pu8Buffer      A Buffer to send                                                    */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_DRVSDCARD_WRITE_ERROR		Error in Write function                                    */
/*               E_SUCCESS			            Success                                                    */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to Write Multi-Sector Data                                          */
/*---------------------------------------------------------------------------------------------------------*/
BOOL
SDCard_WriteStart(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr
)
{
	UINT32 volatile u32Addr;

	if (psSDCardHandler->eType != eTYPE_SD20_HIGH_CARD)
		u32Addr = 512 * u32SecAddr;
	else		
		u32Addr = u32SecAddr;

	if(!SDCmdAndResponse(psSDCardHandler ,SD_CMD25, u32Addr, SD_R1, CMD_TIMEOUT))	// multi-block write command
		return FALSE;
	
	SPI_SET_SS(psSDCardHandler->psSpiHandler, psSDCardHandler->u8SlaveDevice);

	return TRUE;
}

void SDCard_WriteStartToken(S_SDCARD_HANDLER *psSDCardHandler)
{
	//--------------------------
	// start token  
	//--------------------------
	SingleWrite(psSDCardHandler, 0xfffffffc);
	
	//------------
	// data token
	//------------
	// Send 2 data each time, Data Count is 32 Bits 
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_TWO);
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 32);
	SPI_ENABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);// Enable Little Endian (BYTE_ENDIAN)
}

void SDCard_WriteData(
	S_SDCARD_HANDLER *psSDCardHandler,
	PUINT32 pu32Buffer
)
{
	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,*pu32Buffer++);
	SPI_WRITE_TX1(psSDCardHandler->psSpiHandler,*pu32Buffer);
	SPI_GO(psSDCardHandler->psSpiHandler);
}

void SDCard_WriteCRC(S_SDCARD_HANDLER *psSDCardHandler)
{
	// Disable Little Endian (BYTE_ENDIAN)
	SPI_DISABLE_BYTE_REORDER(psSDCardHandler->psSpiHandler);	
	// Send 2 data each time, Data Count is 16 Bits	
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_ONE);
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 16);
	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0x0000);
	SPI_GO(psSDCardHandler->psSpiHandler);
	
	SDCard_WaitBusy(psSDCardHandler);

#if (defined (__ISD9300__))
	SPI_ClearRxFIFO(psSDCardHandler->psSpiHandler);
#endif	
	
	SPI_SET_TX_NUM(psSDCardHandler->psSpiHandler, SPI_TXNUM_ONE);
	SPI_SET_DATA_WIDTH(psSDCardHandler->psSpiHandler, 8);
	SPI_WRITE_TX0(psSDCardHandler->psSpiHandler,0xffffffff);
	SPI_GO(psSDCardHandler->psSpiHandler);	

	
}

BOOL
SDCard_WriteEnd(S_SDCARD_HANDLER *psSDCardHandler)
{
	volatile UINT32 u32BusyCnt=0;
	
	SingleWrite(psSDCardHandler, 0xFFFFFFFF);
	//-------------
	// stop token
	//-------------
	SingleWrite(psSDCardHandler, 0xfffffffd);
	
#if (defined (__ISD9300__))
	SPI_ClearRxFIFO(psSDCardHandler->psSpiHandler);
#endif
	
	while(1)
	{
		SingleWrite(psSDCardHandler, 0xFFFFFFFF);
	
		if ((SPI_READ_RX0(psSDCardHandler->psSpiHandler) & 0xFF) == 0xff)
			break;
		
		u32BusyCnt++;
		if (u32BusyCnt >= WRITE_TIMEOUT)
			return FALSE;			
	}
	
	// Inactive chip select
	SPI_SET_SS(psSDCardHandler->psSpiHandler, SPI_SS_NONE);

	return TRUE;
}


void 
SDCard_Write(
	S_SDCARD_HANDLER *psSDCardHandler,
	UINT32 u32SecAddr,
	UINT32 u32SecCnt,
	PUINT8 pu8Buffer
)
{
	UINT32 volatile i;
	UINT32 volatile u32Cnt = 0;
	volatile UINT32 u32BusyCnt;

 	SDCard_WriteStart(psSDCardHandler, u32SecAddr);
	
	//-----------------
	// Start Send LOOP
	//-----------------
	while(u32Cnt < u32SecCnt)
	{		
		SDCard_WriteStartToken(psSDCardHandler);

		for (i=0; i<512/8; i++)
		{							
			SDCard_WriteData(psSDCardHandler, (PUINT32)pu8Buffer);
			
			SDCard_WaitBusy(psSDCardHandler);

			pu8Buffer+=8;
		}
		 
		SDCard_WriteCRC(psSDCardHandler);
	
		while(SDCard_CheckWriteEndBusy(psSDCardHandler)!=TRUE);
		u32Cnt++;
	}

	SDCard_WriteEnd(psSDCardHandler);

}

UINT32 
SDCard_GetVersion(void)
{
	return (SDCARD_MAJOR_NUM << 16) | (SDCARD_MINOR_NUM << 8) | SDCARD_BUILD_NUM;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: 	 SDCard_GetCardType		                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               g_eCardType   E_SDCAR_TYPE  SD20_HIGH_CARD or SD_CARD                                     */
/*               			                                                                               */
/*                                                                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This function is used to get SDCARD type                                                  */
/*---------------------------------------------------------------------------------------------------------*/
E_SDCAR_TYPE 
SDCard_GetCardType(S_SDCARD_HANDLER *psSDCardHandler)
{
	return psSDCardHandler->eType; 
}
