#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "stm32f10x.h"

void watchdogInit(uint8_t timeoutS);
void watchdogFeed(void);

#endif

