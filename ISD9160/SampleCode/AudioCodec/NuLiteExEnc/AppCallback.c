/*---------------------------------------------------------------------------------------------------------*/
/*																										   */
/* Copyright (c) Nuvoton Technology	Corp. All rights reserved.											   */
/*																										   */
/*---------------------------------------------------------------------------------------------------------*/

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- Lookup table for callback functions.
//		- Implement these functions:
//			* Read storage funciton.
//			* Write storage function.
//			* Read MIDI wave table function.
//			* Process user event function.
//
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------

#include "App.h"
#include "Framework.h"
#include "SPIFlash.h"

extern S_SPIFLASH_HANDLER 	g_sSpiFlash;

UINT32 SPIFlash_ReadDataCallback(void *pu8Buf, UINT32 u32StartAddr, UINT32 u32Count);
UINT32 UserEvent_ProcessCallback(UINT16 u16EventIndex, UINT16 u16EventSubIndex);

S_AUDIO_CALLBACK const g_asAppCallBack[] =
{
	{
		SPIFlash_ReadDataCallback,	// Read data callback
		NULL,						// Write data callback
		UserEvent_ProcessCallback,	// User event procss callback
		NULL						// Read MIDI wavetable callback
	}
};

UINT32 SPIFlash_ReadDataCallback(void *pu8Buf, UINT32 u32StartAddr, UINT32 u32Count)
{
	SPIFlash_Read(&g_sSpiFlash, u32StartAddr, pu8Buf, u32Count);
	return u32Count;
}

UINT32 UserEvent_ProcessCallback(UINT16 u16EventIndex, UINT16 u16EventSubIndex)
{
	ULTRAIO_EVENTHANDLER(u16EventIndex,u16EventSubIndex);
	return 0;
}
