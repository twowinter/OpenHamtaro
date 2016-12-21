/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

#include "Platform.h"
#include "ConfigApp.h"

// Key configuration
#if ((CONFIG_MATRIX_KEY>0) || (CONFIG_TRIGGER_KEY>0) || (CONFIG_TOUCH_KEY>0) )
	#include "Keypad.h"

	extern void InputKey_Initiate(void);
	#define KEYPAD_INITIATE()			InputKey_Initiate()
#else
	#define KEYPAD_INITIATE()	
#endif	//((_CONFIG_MATRIX_KEY_>0) || (_CONFIG_INPUT_KEY_>0))

#if (CONFIG_MATRIX_KEY>0)
	#define MATRIX_KEY_CHECK() 		Keypad_ScanKey()
	#define MATRIX_KEY_DEBOUNCE()	Keypad_KeyDecDebounceCounter()
	#define MATRIX_KEY_RESET()		Keypad_ResetKey()
#else
	#define MATRIX_KEY_CHECK()
	#define MATRIX_KEY_DEBOUNCE()
	#define MATRIX_KEY_RESET()
#endif

#if (CONFIG_TRIGGER_KEY>0)
	#define TRIGGER_KEY_CHECK() 		Keypad_ScanTgr()
	#define TRIGGER_KEY_DEBOUNCE()	Keypad_TgrDecDebounceCounter()
	#define TRIGGER_KEY_RESET()			Keypad_ResetTgr()	
#else
	#define TRIGGER_KEY_CHECK()
	#define TRIGGER_KEY_DEBOUNCE()
	#define TRIGGER_KEY_RESET()
#endif

#if ( defined(CONFIG_TOUCH_KEY) && CONFIG_TOUCH_KEY>0 )
	#define TOUCH_KEY_CHECK() 		Keypad_ScanTouch()
	#define TOUCH_KEY_CAP()			Keypad_TouchCapSense()
	#define TOUCH_KEY_PRESS_COUNT()	Keypad_KeyTouchPressCounter();		
#else
	#define TOUCH_KEY_CHECK()
	#define TOUCH_KEY_CAP()
	#define TOUCH_KEY_PRESS_COUNT()
#endif

// Output pin configuration
#if (CONFIG_OUTPUT_PIN>0)	
	extern void OutputPin_Initiate(void);	   
	#define OUTPUTPIN_INITIATE() 				OutputPin_Initiate()
#else
	#define OUTPUTPIN_INITIATE()
#endif  //(_CONFIG_OUTPUT_PIN_>0)

// Input state configuration
#if (CONFIG_INPUT_STATE > 0 )
	#include "InState.h"
#endif

// Output state configuration
#if (CONFIG_OUTPUT_STATE > 0 )
	#include "IO/OutState.h"
#endif

// ----------------------------------------------------------------------------------------------
// UltraIO configurations
#if ( ULTRAIO_FW_CURVE_ENABLE || ULTRAIO_HW_CURVE_ENABLE )
	#include "UltraIO.h"
	#define ULTRAIO_INITIATE() 									UltraIO_Initiate() 
	#define ULTRAIO_START() 									UltraIO_Start() 
	#define ULTRAIO_STOP() 										UltraIO_Stop() 
	#define ULTRAIO_EVENTHANDLER(u32CmdIndex, u32SubIndex)		UltraIO_EventHandler(u32CmdIndex, u32SubIndex)
#else
	#define ULTRAIO_INITIATE() 	
	#define ULTRAIO_START()
	#define ULTRAIO_STOP()	
	#define ULTRAIO_EVENTHANDLER(u32CmdIndex, u32SubIndex)
#endif
 
#if ( ULTRAIO_FW_CURVE_ENABLE ) 
	#include "UltraIO.h"
	#if ( ULTRAIO_FW_TMR == 2 )
	#define ULTRAIO_OUTCURVETMR0()	
	#define ULTRAIO_OUTCURVETMR1()
	#define ULTRAIO_OUTCURVETMR2()		UltraIO_OuputCurve()
	#elif (ULTRAIO_FW_TMR == 1)
	#define ULTRAIO_OUTCURVETMR0()	
	#define ULTRAIO_OUTCURVETMR1()		UltraIO_OuputCurve()
	#define ULTRAIO_OUTCURVETMR2()
	#elif (ULTRAIO_FW_TMR == 0)
	#define ULTRAIO_OUTCURVETMR0()		UltraIO_OuputCurve()
	#define ULTRAIO_OUTCURVETMR1()		
	#define ULTRAIO_OUTCURVETMR2()
	#endif
#else 
	#define ULTRAIO_OUTCURVETMR0()
	#define ULTRAIO_OUTCURVETMR1()
	#define ULTRAIO_OUTCURVETMR2()
#endif

// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// ADC configuration
#if ( ADC_ENABLE )
	extern void MIC_Open(void);
	extern void MIC_Close(void);
	#define MIC_INITIATE()		MIC_Open()
	#define MIC_UNINITIATE()	MIC_Close()
#else
	#define MIC_INITIATE()
	#define MIC_UNINITIATE()	
#endif
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// APU configuration
#if ( APU_ENABLE )
	extern void SPK_Open(void);
	extern void SPK_Close(void);
	#define SPK_INITIATE()		SPK_Open()
	#define SPK_UNINITIATE()	SPK_Close()
#else
	#define SPK_INITIATE()
	#define SPK_UNINITIATE()	
#endif
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// PDMA configuration
#if ( PDMA_ENABLE )
	#include "PdmaCtrl.h"
	extern void PdmaCtrl_Init(void);
	extern void PdmaCtrl_UnInit(void);
	#define PDMA_INITIATE()		PdmaCtrl_Init()
	#define PDMA_UNINITIATE()	PdmaCtrl_UnInit()
#else
	#define PDMA_INITIATE()
	#define PDMA_UNINITIATE()	
#endif

// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#endif //_FRAMEWORK_H_
