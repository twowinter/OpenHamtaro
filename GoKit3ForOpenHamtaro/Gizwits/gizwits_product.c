#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>
#include "gizwits_product.h"
#include "Hal_key/Hal_key.h"
#include "Hal_Usart/hal_uart.h"
#include "Hal_led/Hal_led.h"
#include "Hal_motor/Hal_motor.h"
#include "Hal_rgb_led/Hal_rgb_led.h"
#include "Hal_temp_hum/Hal_temp_hum.h"
#include "Hal_infrared/Hal_infrared.h"

extern volatile gizwitsReport_t reportData;

//custom 下达数据分析并处理函数
int8_t eventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
    uint8_t i = 0;
    int16_t motorValue = 0;
    gizwitsIssued_t *issuedData = (gizwitsIssued_t *)data;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;

    if((NULL == info) || (NULL == issuedData))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {
            case SetLED_OnOff:
                if(LED_OnOn == issuedData->attrVals.LED_OnOff)
                {
                    reportData.devStatus.LED_OnOff = LED_OnOn;
                    ledRgbControl(254,0,0);       
                    ZB_HA_LightControl(LED_OnOn);
                }
                else
                {
                    reportData.devStatus.LED_OnOff = LED_OnOff;
                    ledRgbControl(0,0,0);
                    ZB_HA_LightControl(LED_OnOff);
                }
                
                GIZWITS_LOG("Evt: SetLED_OnOff %d\r\n",reportData.devStatus.LED_OnOff);
                
                break;
            case SetLED_Color:
                switch(issuedData->attrVals.LED_Color)
                {
                    case LED_Costom:
                        reportData.devStatus.LED_Color = LED_Costom;
                        ledRgbControl(reportData.devStatus.LED_R, reportData.devStatus.LED_G, reportData.devStatus.LED_B);
                        break;
                    case LED_Yellow:
                        reportData.devStatus.LED_Color = LED_Yellow;
                        ledRgbControl(254, 254, 0);                 
                        break;
                    case LED_Purple:
                        reportData.devStatus.LED_Color = LED_Purple;
                        ledRgbControl(254, 0, 70);                    
                        break;
                    case LED_Pink:
                        reportData.devStatus.LED_Color = LED_Pink;
                        ledRgbControl(238 ,30 ,30);                     
                        break;
                    default:
                        break;
                }
                GIZWITS_LOG("Evt: SetLED_Color %d\r\n",issuedData->attrVals.LED_Color);
                break;
            case SetLED_R:
                reportData.devStatus.LED_R = issuedData->attrVals.LED_R;
                issuedData->attrVals.LED_R = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_R);
                ledRgbControl(issuedData->attrVals.LED_R,X2Y(LED_B_RATIO,LED_B_ADDITION,\
            reportData.devStatus.LED_G),X2Y(LED_B_RATIO,  LED_B_ADDITION, reportData.devStatus.LED_G));
                 
                GIZWITS_LOG("Evt:color %d SetLED_R %d\r\n",issuedData->attrVals.LED_Color,issuedData->attrVals.LED_R);
                break;
            case SetLED_G:
                reportData.devStatus.LED_G = issuedData->attrVals.LED_G;
                issuedData->attrVals.LED_G = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_G);
                ledRgbControl(X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),\
            issuedData->attrVals.LED_G,X2Y(LED_B_RATIO, LED_B_ADDITION,reportData.devStatus.LED_B));
            
                GIZWITS_LOG("Evt: color %d SetLED_G %d\r\n", issuedData->attrVals.LED_Color, issuedData->attrVals.LED_G);
                break;
            case SetLED_B:
                reportData.devStatus.LED_B = issuedData->attrVals.LED_B;
                issuedData->attrVals.LED_B = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_B);
                ledRgbControl(X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),\
            X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),issuedData->attrVals.LED_B );
            
                GIZWITS_LOG("Evt: color %dSetLED_B %d\r\n",issuedData->attrVals.LED_Color,issuedData->attrVals.LED_B);
                break;
            case SetMotor:
                reportData.devStatus.Motor_Speed = issuedData->attrVals.Motor_Speed;  
                motorValue = X2Y(MOTOR_SPEED_RATIO,MOTOR_SPEED_ADDITION,protocolExchangeBytes(issuedData->attrVals.Motor_Speed));
                motorStatus(motorValue);
            
                GIZWITS_LOG("Evt: SetMotor %d\r\n", motorValue);
                break;
            
            case WIFI_SOFTAP:
                break;
            case WIFI_AIRLINK:
                break;
            case WIFI_STATION:
                break;
            case WIFI_CON_ROUTER:
                ledRgbControl(0, 0, 0);
                break;
            case WIFI_DISCON_ROUTER:
                break;
            case WIFI_CON_M2M:
                break;
            case WIFI_DISCON_M2M:
                break;
            case WIFI_RSSI:
                printf("RSSI %d\r\n", wifiData->rssi);
                break;
            default:
                break;
        }
    }
    
    return 0;
}

/**
  * TIM3_IRQHandler
  */
extern keyTypedef_t singleKey[2];
extern keysTypedef_t keys;
void TIMER_IRQ_FUN(void)
{
    if (TIM_GetITStatus(TIMER, TIM_IT_Update) != RESET)  
    {
        TIM_ClearITPendingBit(TIMER, TIM_IT_Update  );
        gizwitsTimerMs();
    }
}

/*******************************************************************************
* Function Name  : timerInit
* Description    : timer initialization function
* Input          : arr 重装初值   psc 预分频
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void timerInit(void)
{
    u16 arr = 7199;
    u16 psc = 9;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(TIMER_RCC, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIMER, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位0

    TIM_ITConfig(TIMER, TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //优占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
    TIM_Cmd(TIMER, ENABLE);  //使能TIMx
}

/**
  * USART2_IRQHandler
  */
void UART_IRQ_FUN(void)
{
    uint8_t value = 0;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        value = USART_ReceiveData(USART2);

        gizwitsPutData(&value, 1);
    }
}


//custom 串口写操作，发送数据到WiFi模组
int8_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }

#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizwitsGetTimerCount(), len);
#endif
    
    for(i=0; i<len; i++)
    {
        USART_SendData(UART, buf[i]);
        //Loop until the end of transmission
        while (USART_GetFlagStatus(UART, USART_FLAG_TXE) == RESET);
        
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("%02x ", buf[i]);
#endif
    }
    
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("\r\n");
#endif
    
    return len;
}

void uartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    UART_GPIO_Cmd(UART_GPIO_CLK, ENABLE);
    UART_CLK_Cmd(UART_CLK, ENABLE);
    UART_AFIO_Cmd(UART_AFIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = UART_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = UART_RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = UART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART, &USART_InitStructure);

    USART_ITConfig(UART,USART_IT_RXNE,ENABLE);
    USART_Cmd(UART, ENABLE);
    USART_ClearFlag(UART, USART_FLAG_TC); /*清空发送完成标志,Transmission Complete flag */

    /*使能串口中断,并设置优先级*/
    NVIC_InitStructure.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
