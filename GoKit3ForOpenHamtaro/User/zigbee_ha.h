

#ifndef _ZIGBEE_HA_H
#define _ZIGBEE_HA_H


// Use USART1 for ZB Moudule
#define ZB_IRQ_FUN  USART1_IRQHandler

#define MAX_ZB_BUFFER_LEN  256  //环形缓冲区最大长度


void ZB_HA_Init(void);

#endif
