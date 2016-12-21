#ifndef __SYSCLK_H__
#define __SYSCLK_H__

#include "Platform.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
/**
  * @brief  This function initiates internal RC as the clock source to generates desired system clock.
  * @param  u32SystemClk is the desired system clock(HCLK). For example, 23040000UL means 23MHz
  * @return The actually system clock
  */
UINT32 SysClk_InitiateRC(UINT32 u32SystemClk);

#ifdef __cplusplus
}
#endif

#endif
