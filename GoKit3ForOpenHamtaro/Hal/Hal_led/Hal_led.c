/**
********************************************************
*
* @file      Hal_led.c
* @author    Gizwtis
* @version   V03010100
* @date      2016-07-05
*
* @brief     机智云.只为智能硬件而生
*            Gizwits Smart Cloud  for Smart Products
*            链接|增值ֵ|开放|中立|安全|自有|自由|生态
*            www.gizwits.com
*
*********************************************************/
#include "Hal_led/Hal_led.h"


void ledGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(GPIO_LED1_CLK | GPIO_LED2_CLK | GPIO_LED3_CLK | GPIO_LED4_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_LED1_PIN;
    GPIO_Init(GPIO_LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_LED2_PIN;
    GPIO_Init(GPIO_LED2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_LED3_PIN;
    GPIO_Init(GPIO_LED3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_LED4_PIN;
    GPIO_Init(GPIO_LED4_PORT, &GPIO_InitStructure);

    ledOff(LED1);
    ledOff(LED2);
    ledOff(LED3);
    ledOff(LED4);
}

void ledOn(uint8_t ledNum)
{
    switch (ledNum)
    {
	    case LED1:
	        GPIO_SetBits(GPIO_LED1_PORT,GPIO_LED1_PIN);
	        break;
	    case LED2:
	        GPIO_SetBits(GPIO_LED2_PORT,GPIO_LED2_PIN);
	        break;
	    case LED3:
	        GPIO_SetBits(GPIO_LED3_PORT,GPIO_LED3_PIN);
	        break;
	    case LED4:
	        GPIO_SetBits(GPIO_LED4_PORT,GPIO_LED4_PIN);
	        break;
	    default:
	        break;
    }
}

void ledOff(uint8_t ledNum)
{
    switch (ledNum)
    {
        case LED1:
            GPIO_ResetBits(GPIO_LED1_PORT,GPIO_LED1_PIN);
            break;
        case LED2:
            GPIO_ResetBits(GPIO_LED2_PORT,GPIO_LED2_PIN);
            break;
        case LED3:
            GPIO_ResetBits(GPIO_LED3_PORT,GPIO_LED3_PIN);
            break;
        case LED4:
            GPIO_ResetBits(GPIO_LED4_PORT,GPIO_LED4_PIN);
            break;
        default:
            break;
    }
}
