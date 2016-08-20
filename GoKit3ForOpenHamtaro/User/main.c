/**
********************************************************
*
* @file      main.c
* @author    Gizwtis
* @version   V03010100
* @date      2016-07-05
*
* @brief     机智云.只为智能硬件而生
*            Gizwits Smart Cloud  for Smart Products
*            链接|增值?|开放|中立|安全|自有|自由|生态
*            www.gizwits.com
*
*********************************************************/
#include <string.h>
#include "gizwits_product.h"
#include "header.h"
#include "Hal_key/Hal_key.h"
#include "Hal_Usart/hal_uart.h"
#include "Hal_led/Hal_led.h"
#include "Hal_motor/Hal_motor.h"
#include "Hal_rgb_led/Hal_rgb_led.h"
#include "Hal_temp_hum/Hal_temp_hum.h"
#include "Hal_infrared/Hal_infrared.h"
#include "Hal_Watchdog/hal_watchdog.h"
#include "zigbee_ha.h"

volatile gizwitsReport_t reportData;
keyTypedef_t singleKey[2];
keysTypedef_t keys;

void userInit(void)
{
    delayInit(72);
    uartxInit();
    rgbLedInit();
    ledGpioInit();
    rgbKeyGpioInit();
    motorInit();
    dht11Init();
    irInit();
    watchdogInit(2);    //5,625看门狗复位时间2s
    
    memset((uint8_t*)&reportData, 0, sizeof(gizwitsReport_t));
    reportData.devStatus.Motor_Speed = protocolExchangeBytes(Y2X(MOTOR_SPEED_RATIO,MOTOR_SPEED_ADDITION,MOTOR_SPEED_DEFAULT));
    motorStatus(MOTOR_SPEED_DEFAULT);
}

#if EN_HAM == 0 
void userHandle(void)
{
    uint8_t ret  = 0;
    int8_t curTem = 0;
    int8_t curHum = 0;
    uint8_t curInfrared = 0;
    static int8_t lastTem = 0;
    static int8_t lastHum = 0;
    static uint32_t irLastTimer = 0;
    static uint32_t thLastTimer = 0;
    
    curInfrared = irHandle();
    if(curInfrared != reportData.devStatus.Infrared)
    {
        if((gizwitsGetTimerCount() - irLastTimer) > REPORT_TIME_MAX/4) //timeout = 2S
        {
            reportData.devStatus.Infrared = curInfrared;

            irLastTimer = gizwitsGetTimerCount();
            #if EN_DEBUG > 0
            printf("Infrared %d \r\n", irLastTimer);
            #endif
        }
    }
    
    if((gizwitsGetTimerCount()-thLastTimer) > REPORT_TIME_MAX) //timeout = 2S
    {
        ret = dht11Read(&curTem, &curHum);
        
        if(0 == ret)
        {
            if((curTem != lastTem)||(curHum != lastHum))
            {            
                reportData.devStatus.Temperature = Y2X(TEMPERATURE_RATIO,TEMPERATURE_ADDITION,curTem);
                reportData.devStatus.Humidity = Y2X(HUMIDITY_RATIO,HUMIDITY_ADDITION,curHum);
                
                lastTem = curTem;
                lastHum = curHum;
                #if EN_DEBUG > 0
                printf("Temperature&Humidity  [%d-%d] %d %d\r\n", gizwitsGetTimerCount(), thLastTimer, curTem, curHum);
                #endif
            }
        }
        else
        {
            #if EN_DEBUG > 0
            printf("Failed to read DHT11\r\n");
            #endif
        }
        
        thLastTimer = gizwitsGetTimerCount();
    }
}
#endif

void key1ShortPress(void)
{
    #if EN_DEBUG > 0
    printf("KEY1 PRESS\r\n");
    #endif
}

void key1LongPress(void)
{
    #if EN_DEBUG > 0
    printf("KEY1 PRESS LONG ,Wifi Reset\r\n");
    #endif
    gizwitsSetDefault();

}
void key2ShortPress(void)
{
    #if EN_DEBUG > 0
    printf("KEY2 PRESS ,Soft AP mode\r\n");
    #endif

    //Soft AP mode, RGB red
    ledRgbControl(255, 0, 0);
    gizwitsSetMode(SoftAp_Mode);
}

void key2LongPress(void)
{
    #if EN_DEBUG > 0
    //AirLink mode, RGB Green
    printf("KEY2 PRESS LONG ,AirLink mode\r\n");
    #endif
    ledRgbControl(0, 128, 0);
    gizwitsSetMode(AirLink_Mode);
}
int main(void)
{
    SystemInit();
    
    userInit();

    singleKey[0] = keyInitOne(0,RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_10, key1ShortPress, key1LongPress);
    keys.keyNum++;
    singleKey[1] = keyInitOne(1,RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_8, key2ShortPress, key2LongPress);
    keys.keyNum++;
    keys.singleKey = (keyTypedef_t *)&singleKey;
    keyParaInit(&keys); 

    gizwitsInit();
    ZB_HA_Init();
    #if EN_DEBUG > 0
    printf("Gokit Init Success \r\n");
    #endif
    while(1)
    {
        watchdogFeed();
        #if EN_HAM == 0 
        userHandle();
        #endif
        
        gizwitsHandle((gizwitsReport_t *)&reportData);
        ZB_HA_Handle();
    }
}
