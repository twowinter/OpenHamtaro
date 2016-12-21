#include "SPIFlashUtil.h"

extern S_SPIFLASH_HANDLER g_sSpiFlash;
#define SPIFLASH_4K_BLOCK	4096

//----------------------------------------------------------------------------------------------------------
// Varialbes for SPI flash 4k page erasing and partial write
UINT16 g_u16PartialWriteDataLength = 0;
UINT8 *g_pu8PartialWriteData;
UINT32 g_u32SPIFlashRecAddr;	// Current recoding address in SPIFlash.

UINT32 g_u32SPIFlashRecStartAddr;
PFN_SPIFLASHUTIL_WRITE_CALLBACK g_pfnSpiFlashWriteCallbck;
	
void SPIFlashUtil_4KPagePartialWriteInitiate(UINT32 u32Start4kPageAddr,
	PFN_SPIFLASHUTIL_WRITE_CALLBACK pfnSpiFlashWriteCallbck )
{
	g_pfnSpiFlashWriteCallbck = pfnSpiFlashWriteCallbck;
	g_u16PartialWriteDataLength = 0;
	g_u32SPIFlashRecAddr = u32Start4kPageAddr;
	if ( u32Start4kPageAddr%SPIFLASH_4K_BLOCK )
	{
		// Start Address is not 4K alignment, pre-erase 4K page
		SPIFlash_Erase4K(&g_sSpiFlash,u32Start4kPageAddr/SPIFLASH_4K_BLOCK, 1);
	}
}

void SPIFlashUtil_4KPagePartialWriteStart(UINT8 *pau8Data, UINT32 u32DataSize)
{
	g_u16PartialWriteDataLength = u32DataSize;
	g_pu8PartialWriteData = pau8Data;
}

BOOL SPIFlashUtil_4KPagePartialWriting(void)
{
#define SPIFLASH_CTRL_ERASE	1
	static UINT8 u8SPiFlashCtrl = 0;
	UINT32 u32DoWriteByte;
	
	// SPI Flash should not do any thing if in busy state(programming or erasing 4K page) 
	if ( SPIFlash_CheckBusy(&g_sSpiFlash) == TRUE )
		return TRUE;

	// This is used to execute other jobs which can read SPI Flash before start progamming or earsing
	// to avoid the SPI flash write processing occupying all SPI flash!
	if ( g_pfnSpiFlashWriteCallbck!=NULL )
		g_pfnSpiFlashWriteCallbck();
	
	if ( g_u16PartialWriteDataLength == 0 )
		return FALSE;

	if ((g_u32SPIFlashRecAddr%(SPIFLASH_4K_BLOCK) == 0)	// Check 4k boundary
		&&(u8SPiFlashCtrl != SPIFLASH_CTRL_ERASE))		// Check should do 4k page erasing or not
	{
		SPIFlash_Erase4KStart(&g_sSpiFlash,g_u32SPIFlashRecAddr>>12);
		u8SPiFlashCtrl = SPIFLASH_CTRL_ERASE;
		return TRUE;
	} 
	u8SPiFlashCtrl = 0;

	SPIFlash_WriteStart(&g_sSpiFlash,g_u32SPIFlashRecAddr);
	u32DoWriteByte = SPIFlash_WriteData(&g_sSpiFlash,g_u32SPIFlashRecAddr, g_pu8PartialWriteData, g_u16PartialWriteDataLength);										 
	SPIFlash_WriteEnd(&g_sSpiFlash);

	g_u16PartialWriteDataLength -= u32DoWriteByte;
	g_pu8PartialWriteData += u32DoWriteByte;
	g_u32SPIFlashRecAddr += u32DoWriteByte;


	return TRUE;
}

void SPIFlashUtil_4KPagePartialWriteWithWait(UINT8 *pau8WriteData, UINT32 u32WriteDataCount)
{	
	SPIFlashUtil_4KPagePartialWriteStart(pau8WriteData, u32WriteDataCount);
	SPIFlashUtil_4KPagePartialWritWaitComplete();
}

void SPIFlahUtil_StartWriteEncodeData(S_AUDIOCHUNK_HEADER *psAudioChunkHeader, 
	UINT32 u32StorageStartAddr, PFN_SPIFLASHUTIL_WRITE_CALLBACK pfnSpiFlashWriteCallbck)
{
	g_u32SPIFlashRecStartAddr = u32StorageStartAddr;
	SPIFlashUtil_4KPagePartialWriteInitiate(u32StorageStartAddr, pfnSpiFlashWriteCallbck);
	SPIFlashUtil_4KPagePartialWriteWithWait((UINT8*)psAudioChunkHeader, sizeof(S_AUDIOCHUNK_HEADER));
}

void SPIFlashUtil_EndWriteEncodeData(void)
{
	S_AUDIOCHUNK_HEADER sAudioChunkHeader;
	
	SPIFlashUtil_4KPagePartialWritWaitComplete(); 			// This is to wait the last SPI flash writing to completed

	SPIFlash_Read(&g_sSpiFlash, g_u32SPIFlashRecStartAddr, (UINT8*)&sAudioChunkHeader, sizeof(S_AUDIOCHUNK_HEADER));
	sAudioChunkHeader.u32TotalSize =  (g_u32SPIFlashRecAddr - g_u32SPIFlashRecStartAddr) - sizeof(S_AUDIOCHUNK_HEADER);
	SPIFlash_Write(&g_sSpiFlash, g_u32SPIFlashRecStartAddr, (UINT8*)&sAudioChunkHeader, sizeof(S_AUDIOCHUNK_HEADER));
}

void SPIFlashUtil_WriteEncodeData(S_BUF_CTRL *psEncodeBufCtrl)
{		
	if ( SPIFlashUtil_4KPagePartialWriting() == FALSE )
	{
		if ( psEncodeBufCtrl->u16BufReadIdx != psEncodeBufCtrl->u16BufWriteIdx )
		{
			SPIFlashUtil_4KPagePartialWriteStart(((UINT8*)(psEncodeBufCtrl->pi16Buf)) + psEncodeBufCtrl->u16BufReadIdx, psEncodeBufCtrl->u16FrameSize);
			if ( (psEncodeBufCtrl->u16BufReadIdx+=psEncodeBufCtrl->u16FrameSize) >= psEncodeBufCtrl->u16BufCount )
				psEncodeBufCtrl->u16BufReadIdx = 0;
		}
	}
}
