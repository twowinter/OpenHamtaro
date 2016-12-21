#ifndef _SPIFLASHUTIL_H_
#define _SPIFLASHUTIL_H_
#include "Platform.h"
#include "BufCtrl.h"
#include "AudioCommon.h"
#include "SPIFlash.h"

typedef BOOL (*PFN_SPIFLASHUTIL_WRITE_CALLBACK)( void );
// -------------------------------------------------------------------------------------------------------------------
// Funcitons to write encoded data as a encode audio file to SPI flash with 4K page earsing capibility
// The function calling sequences are:
//
// SPIFlahUtil_StartWriteEncodeData()
//		|
//		+---------------------------+
//		V                           |
// SPIFlashUtil_WriteEncodeData()---+
//		|
//		V
// SPIFlashUtil_EndWriteEncodeData()
//


//---------------------------------------------------------------------------------------------------------
//	Description
//		Initiate 4k page partial write and write header to SPI flash before writing encoded data
//
//	Parameter
//		psAudioChunkHeader[in] :
//			Address of audio file header which must be written first before start writing encode data
//
//  	u32StorageStartAddr[in] :
//É‹		The storage start address of the audio file header.
//
//  	pfnSpiFlashWriteCallbck[in] :
//			The function to be execute at checking SPI flash is ready.
//			This can let other function be be executed before writing data to SPI flash.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlahUtil_StartWriteEncodeData(S_AUDIOCHUNK_HEADER *psAudioChunkHeader, 
	UINT32 u32StorageStartAddr, PFN_SPIFLASHUTIL_WRITE_CALLBACK pfnSpiFlashWriteCallbck);

//---------------------------------------------------------------------------------------------------------
//	Description
//		Write encoded data to SPI flash.
//
//		The writing function will check the buffer control.
//		If there are data, this funtion will write to SPI flash.
//		This function will not wait SPI flash from busy to ready, in order to save the waiting busy time!
//
//	Parameter
//  	psEncodeBufCtrl[in] :
//			Address of buffer control which record buffer read and write status.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlashUtil_WriteEncodeData(S_BUF_CTRL *psEncodeBufCtrl);

//---------------------------------------------------------------------------------------------------------
//	Description
//		Wait for last encoded data written to SPIFlash and
//		update the header written in calling "SPIFlashUtil_StartWriteEncodeData()"
//
//	Parameter
//		None
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlashUtil_EndWriteEncodeData(void);
// -------------------------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------------------------
// Funcitons to write data to SPI flash with 4K page earsing capibility
// The function calling sequences are:
//
// SPIFlashUtil_4KPagePartialWriteInitiate()
//		|
//		V
// SPIFlashUtil_4KPagePartialWriteStart()
//		|
//		+-------------------------------------------------+
//		|                                                 |
//		+--------------------------------------------+    |
//		V                                            |    |
// SPIFlashUtil_4KPagePartialWriting(): return TRUE -+    |
//		|               |                                 |
//      |            return TRUE                          |
//		|               |                                 |
//		|               +---------------------------------+
//		|
//		V
// SPIFlashUtil_4KPagePartialWritWaitComplete()
//

//---------------------------------------------------------------------------------------------------------
//	Description
//		Initiate the 4k page partial write without waiting completed.
//		The 1st 4k page will     be erased if the start address is     4K alignment.
//		The 1st 4k page will not be erased if the start address is not 4K alignment.
//
//	Parameter
//  	u32StorageStartAddr[in] :
//É‹		The storage start address of the write data.
//
//  	pfnSpiFlashWriteCallbck[in] :
//			The function to be execute at checking SPI flash is ready.
//			This can let other function be be executed before writing data to SPI flash.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlashUtil_4KPagePartialWriteInitiate(UINT32 u32Start4kPageAddr, PFN_SPIFLASHUTIL_WRITE_CALLBACK pfnSpiFlashWriteCallbck);

//---------------------------------------------------------------------------------------------------------
//	Description
//		Prepare and start write data to SPI flash without waiting completed.
//
//		This funciton will keep address of  data and the size of data.
//		The data will be writting to SPI flash at calling SPIFlashUtil_4KPagePartialWriting().
//		Before the SPIFlashUtil_4KPagePartialWriting() return FALSE, the data can not be changed.
//
//	Parameter
//  	pau8Data[in] :
//			pointer of write data.
//			The data will be writting to SPI flash at calling SPIFlashUtil_4KPagePartialWriting().
//			Before the SPIFlashUtil_4KPagePartialWriting() return FALSE, the data should not be changed.
//
//  	u32DataSize[in] :
//			the size of write data.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlashUtil_4KPagePartialWriteStart(UINT8 *pau8Data, UINT32 u32DataSize);

//---------------------------------------------------------------------------------------------------------
//	Description
//		Check previous written completed and write data to SPI flash without waiting completed.
//		This function must be called until it return FALSE.
//		
//		At this funciton return FALSE, there are 2 condition to go on:
//		1. End written:			call "SPIFlashUtil_4KPagePartialWritWaitComplete()" 
//		2. Continue to write:	call "SPIFlashUtil_4KPagePartialWriteStart())"
//
//	Parameter
//		none
//
//	Return Value
//		TRUE:
//			the SPI flash is in busy or there are data must be written
//		FALSE:
//			write data completed but SPI flash maybe in the busy state
//---------------------------------------------------------------------------------------------------------
BOOL SPIFlashUtil_4KPagePartialWriting(void);

//---------------------------------------------------------------------------------------------------------
//	Description
//		Wait SPI flash to complete writing the last data sent in the last SPIFlashUtil_4KPagePartialWriting() call.
//		
//		Becasue SPIFlashUtil_4KPagePartialWriting() send the last write data to SPI flash but not wait SPI flash to be
//		ready. Therfore 
//		
//		At this funciton return FALSE, there are 2 condition to go on:
//		1. End written:			call "SPIFlashUtil_4KPagePartialWritWaitComplete()" 
//		2. Continue to write:	call "SPIFlashUtil_4KPagePartialWriteStart())"
//
//	Parameter
//		none
//
//	Return Value
//		TRUE:
//			there are data writting and the SPI flash is in busy
//		FALSE:
//			write data completed but SPI flash maybe in the busy state
//---------------------------------------------------------------------------------------------------------
#define SPIFlashUtil_4KPagePartialWritWaitComplete()	while( SPIFlashUtil_4KPagePartialWriting() == TRUE )
	

//---------------------------------------------------------------------------------------------------------
//	Description
//		Write data and wait SPI flash to be ready.
//
//	Parameter
//  	pau8Data[in] :
//			pointer of write data.
//			The data will be writting to SPI flash at calling SPIFlashUtil_4KPagePartialWriting().
//			Before the SPIFlashUtil_4KPagePartialWriting() return FALSE, the data should not be changed.
//
//  	u32DataSize[in] :
//			the size of write data.
//
//	Return Value
//		None
//---------------------------------------------------------------------------------------------------------
void SPIFlashUtil_4KPagePartialWriteWithWait(UINT8 *pau8WriteData, UINT32 u32WriteDataCount);


#endif
