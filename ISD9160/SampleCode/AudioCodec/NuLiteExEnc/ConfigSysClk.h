#ifndef __CONFIGSYSCLK_H__
#define __CONFIGSYSCLK_H__

#include "Platform.h"
#include "SysClk.h"

#ifdef __cplusplus
extern "C"
{
#endif
/**
  * @brief   Select the system(HCLK) frequency.
  * @details Select system clock to be one of the followings
  * 48, 32 MHz		
  */
#define SYSTEM_CLK		48

#define SYSCLK_INITIATE() SysClk_InitiateRC(1000000UL*SYSTEM_CLK)
	
#ifdef __cplusplus
}
#endif

#endif
