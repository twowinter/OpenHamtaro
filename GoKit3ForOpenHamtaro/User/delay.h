#ifndef __DELAY_H
#define __DELAY_H 	

#include <stm32f10x.h>

void delayInit(uint8_t SYSCLK);
void delayMs(uint16_t nms);
void delayUs(uint32_t nus);


#endif 

