#ifndef _GIZWITS_PRODUCT_H
#define _GIZWITS_PRODUCT_H

#include <stdint.h>
#include <stm32f10x.h>
#include "gizwits_protocol.h"

#define SOFTWARE_VERSION "03010100"
#define HARDWARE_VERSION "03010100"

#ifndef SOFTWARE_VERSION
    #error "no define SOFTWARE_VERSION"
#endif

#ifndef HARDWARE_VERSION
    #error "no define HARDWARE_VERSION"
#endif


#define EN_HAM  1


#define TIMER TIM3
#define TIMER_IRQ TIM3_IRQn
#define TIMER_RCC RCC_APB1Periph_TIM3
#define TIMER_IRQ_FUN TIM3_IRQHandler

#define UART_BAUDRATE 9600
#define UART_PORT     2
#define UART          USART2
#define UART_IRQ      USART2_IRQn
#define UART_IRQ_FUN  USART2_IRQHandler

#if (UART_PORT == 1)
#define UART_GPIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_GPIO_CLK          RCC_APB2Periph_GPIOA

#define UART_AFIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_AFIO_CLK          RCC_APB2Periph_AFIO

#define UART_CLK_Cmd           RCC_APB2PeriphClockCmd
#define UART_CLK               RCC_APB2Periph_USART1 

#define UART_GPIO_PORT         GPIOA
#define UART_RxPin             GPIO_Pin_10
#define UART_TxPin             GPIO_Pin_9
#endif

#if (UART_PORT == 2)
#define UART_GPIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_GPIO_CLK          RCC_APB2Periph_GPIOA

#define UART_AFIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_AFIO_CLK          RCC_APB2Periph_AFIO

#define UART_CLK_Cmd           RCC_APB1PeriphClockCmd
#define UART_CLK               RCC_APB1Periph_USART2 

#define UART_GPIO_PORT         GPIOA
#define UART_RxPin             GPIO_Pin_3
#define UART_TxPin             GPIO_Pin_2
#endif


#if (UART_PORT == 3)

#define UART_GPIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_GPIO_CLK          RCC_APB2Periph_GPIOC

#define UART_AFIO_Cmd          RCC_APB2PeriphClockCmd
#define UART_AFIO_CLK          RCC_APB2Periph_AFIO

#define UART_CLK_Cmd           RCC_APB1PeriphClockCmd
#define UART_CLK               RCC_APB1Periph_USART3 

#define UART_GPIO_PORT         GPIOC
#define UART_RxPin             GPIO_Pin_11
#define UART_TxPin             GPIO_Pin_10

#endif

#define REPORT_TIME_MAX 2000 //2S


void timerInit(void);
void uartInit(void);
int8_t uartWrite(uint8_t *buf, uint32_t len);
void platformInit(void);
void platformHandle(void);
#endif
