/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/		 

// ---------------------------------------------------------------------------------------------------------
//	Functions:
//		- System clock configuration.
//		- Keypad configuration.
//		- SPI Flash configuration.
//		- Speaker configuration.
//		- MIC configuration.
//		- Output pin configuration.
//		- UltraIO configuration.
//		- Application Initiation.
//		- Processing loop:
//			* Codec processing(use functions in "AppFunctions.c").
//			* Voice effect processing(use functions in "AppFunctions.c").
//			* Keypad check and execution actions(use functions in "InputKeyActions.c").
//			* Etc.
//	
//	Reference "Readme.txt" for more information.
// ---------------------------------------------------------------------------------------------------------

#include "App.h"
#include "Framework.h"
#include "Keypad.h"
#include "SPIFlash.h"
#include "ConfigSysClk.h"
#include "MicSpk.h"

#if( !defined(__CHIP_SERIES__) )
#error "Please update SDS version >= v5.0."
#endif

// SPI flash handler.
S_SPIFLASH_HANDLER g_sSpiFlash;
// Application control.
volatile UINT8 g_u8AppCtrl;
// Application handler.
S_APP g_sApp;

extern void App_Initiate(void);
extern BOOL App_ProcessPlay(void);
extern BOOL App_StopPlay(void);
extern BOOL App_ProcessRec(void);
extern BOOL App_StopRec(void);

UINT8 SPIFlash_Initiate(void)
{ 
	UINT16 ui16Temp;
	UINT32 ui32Temp;
	UINT32 u32Count;

	// SPI0: GPA1=SSB00, GPA2=SCLK0, GPA3=MISO0, GPA4=MOSI0 
	SYS->GPA_MFP  = 
		(SYS->GPA_MFP & (~(SYS_GPA_MFP_PA0MFP_Msk|SYS_GPA_MFP_PA1MFP_Msk|SYS_GPA_MFP_PA2MFP_Msk|SYS_GPA_MFP_PA3MFP_Msk)) )
		| (SYS_GPA_MFP_PA0MFP_SPI_MOSI0|SYS_GPA_MFP_PA1MFP_SPI_SCLK|SYS_GPA_MFP_PA2MFP_SPI_SSB0|SYS_GPA_MFP_PA3MFP_SPI_MISO0);	
	
	// Reset IP module
	CLK_EnableModuleClock(SPI0_MODULE);
	SYS_ResetModule(SPI0_RST);
	SPIFlash_Open(SPI0, SPI_SS0, SPI0_CLOCK, &g_sSpiFlash );

	// Make SPI flash leave power down mode if some where or some time had made it entring power down mode
	SPIFlash_PowerDown(&g_sSpiFlash, FALSE);
	
	// Check SPI flash is ready for accessing
	u32Count = ui32Temp = 0;
	while(u32Count!=100)
	{
		SPIFlash_Read(&g_sSpiFlash, 0, (PUINT8) &ui16Temp, 2);
		if ( ui32Temp != (UINT32)ui16Temp )
		{
			ui32Temp = (UINT32)ui16Temp;
			u32Count = 0;
		}
		else
			u32Count++;
	}

	// The following code can be remove to save code if the flash size is not necessary for this application
	SPIFlash_GetChipInfo(&g_sSpiFlash);
	if (g_sSpiFlash.u32FlashSize == 0)
		return 0;
	
	// The above code can be remove to save code if the flash size is not necessary for this application
	return 1;
}

//---------------------------------------------------------------------------------------------------------
// Main Function                                                           
//---------------------------------------------------------------------------------------------------------
INT32 main()
{
										
	SYSCLK_INITIATE();				// Configure CPU clock source and operation clock frequency.
									// The configuration functions are in "SysClkConfig.h"
	
	CLK_EnableLDO(CLK_LDOSEL_3_3V);	// Enable ISD9100 interl 3.3 LDO.
	
	if (! SPIFlash_Initiate())		// Initiate SPI interface and checking flows for accessing SPI flash.
		while(1); 					// loop here for easy debug

	OUTPUTPIN_INITIATE();			// Initiate output pin configuration.
									// The output pins configurations are defined in "ConfigIO.h".
	
	KEYPAD_INITIATE();				// Initiate keypad configurations including direct trigger key and matrix key
									// The keypad configurations are defined in "ConfigIO.h".
	
	ULTRAIO_INITIATE();				// Initiate ultraio output configurations.
									// The ultraio output pin configurations are defined in "ConfigUltraIO.h"
	
	PDMA_INITIATE();				// Initiate PDMA.
									// After initiation, the PDMA engine clock NVIC are enabled.
									// Use PdmaCtrl_Open() to set PDMA service channel for desired IP.
									// Use PdmaCtrl_Start() to trigger PDMA operation.
									// Reference "PdmaCtrl.h" for PDMA related APIs.
									// PDMA_INITIATE() must be call before SPK_INITIATE() and MIC_INITIATE(), if open MIC or speaker.
	
	SPK_INITIATE();					// Initiate speaker including pop-sound canceling.
									// After initiation, the APU is paused.
									// Use SPK_Resume(0) to start APU operation.
									// Reference "MicSpk.h" for speaker related APIs.

	MIC_INITIATE();					// Initiate MIC.
									// After initiation, the ADC is paused.
									// Use ADC_Resume() to start ADC operation.
									// Reference "MicSpk.h" for MIC related APIs.
	
																	
	App_Initiate();					// Initiate application for audio decode.
	while (1)
	{
		if ( g_u8AppCtrl&APPCTRL_RECORD )
		{
			if ( App_ProcessRec() == FALSE )
				App_StopRec();
		}
		else if ( g_u8AppCtrl&APPCTRL_PLAY )
		{
			if ( App_ProcessPlay() == FALSE )
				App_StopPlay();
		}

		TRIGGER_KEY_CHECK();		// Check and execute direct trigger key actions defined in "InputKeyActions.c"
									// Default trigger key handler is "Default_KeyHandler()"
									// The trigger key configurations are defined in "ConfigIO.h".
		
		MATRIX_KEY_CHECK();			// Check and execute matrix key actions defined in "InputKeyActions.c"
									// Default matrix key handler is "Default_KeyHandler()"
									// The matrix key configurations are defined in "ConfigIO.h".

		TOUCH_KEY_CHECK();        // Check and execute touch key actions defined in "InputKeyActions.c"
									// Default touch key handler is "Default_KeyHandler()"
									// The touch key configurations are defined in "ConfigIO.h".
	}
}
