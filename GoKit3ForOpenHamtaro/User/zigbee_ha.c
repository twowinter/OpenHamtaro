


#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>
#include "Hal_Usart/hal_uart.h"
#include "dal_rb.h"
#include "zigbee_ha.h"

static dal_rb_t zbRb;
static char zbBuf[MAX_ZB_BUFFER_LEN];



void ZB_IRQ_FUN(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  { 		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		USART_SendData(USART1,USART_ReceiveData(USART1));
		//Loop until the end of transmission
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	}
}



void ZB_HA_Init(void)
{    
    zbRb.rbCapacity = MAX_ZB_BUFFER_LEN;
    zbRb.rbBuff = zbBuf;
    DalRbCreate(&zbRb);
}

