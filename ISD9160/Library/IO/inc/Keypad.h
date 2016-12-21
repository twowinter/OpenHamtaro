/**************************************************************************//**
 * @file     Keypad.h
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 14/09/16 02:05p $
 * @brief    Keypad(include Direct,Matrix and Touch Key) via GPIO interface to control. 
 *
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#ifdef  __cplusplus
extern "C"
{
#endif

/** @addtogroup LIBRARY_IO Input/Output Library
  @{
*/

/** @addtogroup LIBRARY_IO_KEYPAD Keypad Library
  @{
*/

/** @addtogroup LIBRARY_IO_KEYPAD_EXPORTED_CONSTANTS Keypad Library Exported Constants
  @{
*/
	
#define KEYPAD_GPIOA	           (0)                  /*!< GPIOA defined for Keyoad Handler \hideinitializer */
#define KEYPAD_GPIOB	           (1)                  /*!< GPIOB defined for Keyoad Handler \hideinitializer */

#define KEYPAD_FALLING	           (0)                  /*!< Key falling state \hideinitializer */
#define KEYPAD_RISING	           (1)                  /*!< Key rising state \hideinitializer */
#define KEYPAD_PRESSING            (2)                  /*!< Key pressing state \hideinitializer */

typedef void (*PFN_KEYPAD_CALLBACK)(uint32_t u32Param); /*!< Keypad callback function defined \hideinitializer */

// Direct trigger event handler
typedef struct 
{
	PFN_KEYPAD_CALLBACK	m_pfnTgr_Callback;		// callback function of event handler. NULL: end of handler array
	uint32_t	        m_u32Param;			    // parameter that will be passed to handler
	uint16_t            m_u16Pin;				// Pin bit mask	direct trigger input
	uint8_t             m_u8Port;				// GPIO Port. 0: GPIOA, 1: GPIOB
	uint8_t             m_u8Mode;				// 0: falling, 1: rising, 2: pressing
} S_KEYPAD_TGR_HANDLER;

// Key matrix event handler
typedef struct 
{
	PFN_KEYPAD_CALLBACK	m_pfnKey_Callback;		// callback function of event handler. NULL: end of handler array
	uint32_t            m_u32Param;				// parameter that will be passed to handler
	uint16_t            m_u16Pin_I;             // Pin bit mask of key matrix input
	uint16_t            m_u16Pin_O;			    // Pin bit mask of key matrix output
	uint8_t             m_u8Port_I;             // input GPIO port. 0: GPIOA, 1: GPIOB
	uint8_t	            m_u8Port_O;             // Output GPIO port. 0: GPIOA, 1: GPIOB
	uint8_t             m_u8Mode;               // 0: falling, 1: rising, 2: pressing
} S_KEYPAD_KEY_HANDLER;

typedef struct 
{
	uint16_t 			m_u16Press;				// Pressing threshold.
	uint16_t			m_u16Release;			// Release threshold.
}S_KEYPAD_TOUCH_THRESHOLD;

// Touch key event handler
typedef struct 
{
	PFN_KEYPAD_CALLBACK	m_pfnTouch_Callback;	// callback function of event handler. NULL: end of handler array
	uint32_t	        m_u32Param;			    // parameter that will be passed to handler
	uint16_t            m_u16Pin;				// Pin bit touch key input
	uint8_t             m_u8Mode;				// 0: falling, 1: rising, 2: pressing
} S_KEYPAD_TOUCH_HANDLER;

/*@}*/ /* end of group LIBRARY_IO_KEYPAD_EXPORTED_CONSTANTS */

/** @addtogroup LIBRARY_IO_KEYPAD_EXPORTED_FUNCTIONS Keypad Exported Functions
  @{
*/

/**
  * @brief  1. Initial keypad.
  *         2. Set Debounce and pressing events interval count.
  *         3. Time unit is decided by application.
  *         4. Application should initiated a timer to call ScanKeypad() periodically.
  * @param  u16DebounceCount is the debounce count for change pin status.
  * @param  u16PressingCount is the interval count between 2 pressing events.
  * @return None
  */
void Keypad_InitKeypad( uint16_t u16DebounceCount, uint16_t u16PressingCount );

/**
  * @brief  1. Initiate direct trigger pins.
  *         2. Set trigger event handlers.
  * @param  u16PinMaskA is the mask of PGIOA to be input pins.
  * @param  u16PinMaskB is the mask of PGIOB to be input pins.
  * @param  psTgrHandler is the direct trigger event handler array.
  */
void Keypad_InitTgr(uint16_t u16PinMaskA,uint16_t u16PinMaskB,const S_KEYPAD_TGR_HANDLER* psTgrHandler);

/**
  * @brief  Reset static variable for trigger key.
  */
void Keypad_ResetTgr(void);

/**
  * @brief  1.This function scan GPIOA or GPIOB pins for direct trigger operation.
  *         2.It calls event handler if Debounce is timer up.
  * @note   1.This API can be called anywhere to scan trigger status, 
  *           and should be called timely to prevent status loss.
  *         2.But should be careful to call it in ISR to prevent block other ISR 
  *           if event handler will take too much CPU time
  * @return 0: trigger event does not happen or event handler is called,
  *         1: event happen but handler is not found.
  */
uint8_t Keypad_ScanTgr(void);

/**
  * @brief  This API is called in timer interrupt to decrease Debounce counter periodically.
  * @return 1: Decrease Debounce counter (in Debounce), 
  *         0: not decrease Debounce counter (not in Debounce).
  */
uint8_t Keypad_TgrDecDebounceCounter(void);

/**
  * @brief  Initial key matix pins, and event handlers.
  * @param  u16InPinMaskA is the mask of GPIOA to be input pins of key matrix.
  * @param  u16InPinMaskB is the mask of GPIOB to be input pins of key matrix.
  * @param  u16OutPinMaskA is the mask of GPIOA to be output pins of key matrix.
  * @param  u16OutPinMaskB is the mask of GPIOB to be output pins of key matrix.
  * @param  psKeyHandler is the matrix key event handler array.
  * @param  pu16KeyPinValueBuf is to provided an array to be pin value buffer by application.
  *         Array size is "output pin number".
  *         This buffer must exist as keypad scaning.
  */
void Keypad_InitKey(uint16_t u16InPinMaskA,uint16_t u16InPinMaskB,uint16_t u16OutPinMaskA,uint16_t u16OutPinMaskB,
                    const S_KEYPAD_KEY_HANDLER* psKeyHandler,uint16_t* pu16KeyPinValueBuf);

/**
  * @brief  Reset static variable for matrix key.
  */
void Keypad_ResetKey(void);

/**
  * @brief  1.This function scan GPIOA or GPIOB pins for matrix key operation.
  *         2.It calls event handler if Debounce is timer up.
  * @note   1.This API can be called anywhere to scan key matrix status, 
  *           and should be called timely to prevent status loss.
  *         2.But should be careful to call it in ISR to prevent block other ISR 
  *           if event handler will take too much CPU time
  * @return 0: key event does not happen or event handler is called,
  *         1: event happen but handler is not found.
  */
uint8_t Keypad_ScanKey(void);

/**
  * @brief  This API is called in timer interrupt to decrease Debounce counter periodically.
  * @return 1: Decrease Debounce counter (in Debounce), 
  *         0: not decrease Debounce counter (not in Debounce).
  */
uint8_t Keypad_KeyDecDebounceCounter(void);

/**
  * @brief  Initial touch key pins, and event handlers.
  * @param  u16Pin is the mask of capture pin index to be input pins of key touch.
  * @param  psTouchHandler is the touch key event handler array.
  * @param  pu16TouchPinValueBuf is to provided an array to be event value buffer by application.
  */
void Keypad_InitTouch( uint16_t u16Pin, 
					   const S_KEYPAD_TOUCH_THRESHOLD* psThreshold, 
					   const S_KEYPAD_TOUCH_HANDLER* psTouchHandler, 
					   uint16_t* pu16TouchPinValueBuf );

/**
  * @brief  This function calls event handler if Debounce is timer up.
  * @note   1.This API can be called anywhere to scan key touch status, 
  *           and should be called timely to prevent status loss.
  *         2.But should be careful to call it in ISR to prevent block other ISR 
  *           if event handler will take too much CPU time
  * @return 0: key event does not happen or event handler is called,
  *         1: event happen but handler is not found.
  */
uint8_t Keypad_ScanTouch(void);

/**
  * @brief  1.This function scan capture pins for touch key operation.
			2.It tags "capture flag" for activing "Keypad_ScanTouch".
  * @note   This API should put into irq handler(ex. CAPS_IRQHandler)
  *         and should be called timely to prevent status loss.
  */
void Keypad_TouchCapSense(void);

/**
  * @brief  This API is counting press counter for pressing status.
  */
void Keypad_KeyTouchPressCounter(void);

/*@}*/ /* end of group LIBRARY_IO_KEYPAD_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group LIBRARY_IO_KEYPAD */

/*@}*/ /* end of group LIBRARY_IO */

#ifdef  __cplusplus
}
#endif

#endif
