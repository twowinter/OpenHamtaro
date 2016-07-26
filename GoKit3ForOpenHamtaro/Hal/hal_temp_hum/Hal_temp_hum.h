#ifndef _HAL_HEMP_HUM_H
#define _HAL_HEMP_HUM_H

#include <stdio.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "delay.h"
#include "Hal_temp_hum/sys.h"  

//Set GPIO Direction
#define DHT11_IO_IN()  GPIO_InitTypeDef GPIO_InitStruct;\
	                     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;\
	                     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;\
	                     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;\
	                     GPIO_Init(GPIOB, &GPIO_InitStruct);
											 
#define DHT11_IO_OUT() GPIO_InitTypeDef GPIO_InitStruct;\
	                     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;\
	                     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;\
	                     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;\
	                     GPIO_Init(GPIOB, &GPIO_InitStruct);
	
#define	DHT11_DQ_OUT PBout(3) 
#define	DHT11_DQ_IN  PBin(3)   

#define MEAN_NUM            10

typedef struct
{
    uint8_t curI;
    uint8_t thAmount; 
    uint8_t thBufs[10][2];
}thTypedef_t; 

/* Function declaration */
uint8_t dht11Init(void); //Init DHT11
uint8_t dht11Read(uint8_t *temperature, uint8_t *humidity); //Read DHT11 Value
static uint8_t dht11ReadData(uint8_t *temperature, uint8_t *humidity);
static uint8_t dht11ReadByte(void);//Read One Byte
static uint8_t dht11ReadBit(void);//Read One Bit
static uint8_t dht11Check(void);//Chack DHT11
static void dht11Rst(void);//Reset DHT11    
void dht11SensorTest(void);

#endif /*_HAL_HEMP_HUM_H*/

