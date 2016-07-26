/**
********************************************************
*
* @file      Hal_rgb_led.c
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
#include "Hal_rgb_led/Hal_rgb_led.h"

void rgbKeyGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(GPIO_RGB_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_RGB_PIN;
    GPIO_Init(GPIO_RGB_PORT, &GPIO_InitStructure);
}

void ledDelay(unsigned int ms)
{
    volatile unsigned  int i=0;
    for(i=0; i<ms; i++);

}


/************ generation clock *********************/
void clkProduce(void)
{
    SCL_LOW;        // SCL=0
    ledDelay(40);
    SCL_HIGH;       // SCL=1
    ledDelay(40);
}


/**********  send 32 zero ********************/
void send32Zero(void)
{
    unsigned char i;
    SDA_LOW;   // SDA=0
    for (i=0; i<32; i++)
    {
        clkProduce();
    }
}

/********* invert the grey value of the first two bits ***************/
uint8_t takeAntiCode(uint8_t dat)
{
    uint8_t tmp = 0;

    tmp=((~dat) & 0xC0)>>6;
    return tmp;
}


/****** send gray data *********/
void datSend(uint32_t dx)
{
    uint8_t i;

    for (i=0; i<32; i++)
    {
        if ((dx & 0x80000000) != 0)
        {

            SDA_HIGH;     //  SDA=1;
        }
        else
        {
            SDA_LOW;    //  SDA=0;
        }

        dx <<= 1;
        clkProduce();
    }
}

/******* data processing  ********************/
void dataDealWithAndSend(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t dx = 0;

    dx |= (uint32_t)0x03 << 30;             // The front of the two bits 1 is flag bits
    dx |= (uint32_t)takeAntiCode(b) << 28;
    dx |= (uint32_t)takeAntiCode(g) << 26;
    dx |= (uint32_t)takeAntiCode(r) << 24;

    dx |= (uint32_t)b << 16;
    dx |= (uint32_t)g << 8;
    dx |= r;

    datSend(dx);
}

void rgbLedInit(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    send32Zero();
    dataDealWithAndSend(0,0,0);	  // display red
    dataDealWithAndSend(0,0,0);
}

void ledRgbControl(uint8_t r, uint8_t g, uint8_t b)
{
    GPIO_SetBits(GPIO_RGB_PORT,GPIO_RGB_PIN);
    send32Zero();
    dataDealWithAndSend(r, g, b);  // display red
    dataDealWithAndSend(r, g, b);  // display red
}
