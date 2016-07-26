/**
********************************************************
*
* @file      hal_key.c
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
#include "Hal_key/hal_key.h"
#include <stm32f10x.h>
#include <math.h>

extern uint8_t			SN;
uint8_t 				keyCountTime;


/*******************************************************************************
* Function Name  : keyGpioInit
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void keyGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(GPIO_KEY1_CLK | GPIO_KEY2_CLK | GPIO_KEY3_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY1_PIN;
    GPIO_Init(GPIO_KEY1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY2_PIN;
    GPIO_Init(GPIO_KEY2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_KEY3_PIN;
    GPIO_Init(GPIO_KEY3_PORT, &GPIO_InitStructure);
}

/**
  * TIM2_IRQHandler
  */
extern keyTypedef_t singleKey[2];
extern keysTypedef_t keys;
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        keyHandle((keysTypedef_t *)&keys);
    }
}

/*******************************************************************************
* Function Name  : timer2Init
* Description    : timer initialization function
* Input          : arr 重装初值   psc 预分频
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void timer2Init(void)
{
    u16 arr = 7199;
    u16 psc = 9;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位0

    TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //优占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
    TIM_Cmd(TIM2, ENABLE);  //使能TIMx
}

keyTypedef_t keyInitOne(uint8_t keyNo ,uint32_t keyRccPeriph, GPIO_TypeDef * keyPort, uint32_t keyGpio, gokitKeyFunction shortPress, gokitKeyFunction longPress)
{
    keyTypedef_t singleKey;
    singleKey.keyRccPeriph = keyRccPeriph;
    singleKey.keyPort = keyPort;
    singleKey.keyGpio = keyGpio;
    singleKey.keyNum = keyNo;
    singleKey.longPress = longPress;
    singleKey.shortPress = shortPress;
    return singleKey;
}

void keyParaInit(keysTypedef_t *keys)
{
    uint8_t temI = 0; 
    for(temI = 0; temI < keys->keyNum; temI++) 
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphClockCmd(keys->singleKey[temI].keyRccPeriph, ENABLE);

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

        GPIO_InitStructure.GPIO_Pin = keys->singleKey[temI].keyGpio;
        GPIO_Init(keys->singleKey[temI].keyPort, &GPIO_InitStructure);
    }
    timer2Init();
}

/*******************************************************************************
* Function Name  : getKey
* Description    : Read the KEY state
* Input          : None
* Output         : None
* Return         : uint8_t KEY state
* Attention		 : None
*******************************************************************************/
uint8_t getKey(keysTypedef_t *keyS)
{
    uint8_t readKey = 0;              //必要,初始化
    if(!GPIO_ReadInputDataBit((GPIO_TypeDef*)keyS->singleKey[0].keyPort,keyS->singleKey[0].keyGpio))
    {
        readKey |= PRESS_KEY1;
    }
    if(!GPIO_ReadInputDataBit((GPIO_TypeDef*)keyS->singleKey[1].keyPort,keyS->singleKey[1].keyGpio))
    {
        readKey |= PRESS_KEY2;
    }
    return readKey;
}


/*******************************************************************************
* Function Name  : readKeyValue
* Description    : Read the KEY value
* Input          : None
* Output         : None
* Return         : uint8_t KEY value
* Attention		 : None
*******************************************************************************/
uint8_t readKeyValue(keysTypedef_t *keyS)
{
    static uint8_t keyCheck;
    static uint8_t keyState;
    static uint16_t keyLongCheck;
    static uint8_t keyPrev = 0;      //上一次按键

    uint8_t keyPress;
    uint8_t keyReturn = 0;
    
    keyCountTime ++;
    
    if(keyCountTime >= 10)          //keyCountTime 1MS+1  按键消抖10MS
    {
        keyCountTime = 0;
        keyCheck = 1;
    }
    if(1 == keyCheck)
    {
        keyCheck = 0;
        keyPress = getKey(keyS);
        switch (keyState)
        {
            case 0:
                if(keyPress != 0)
                {
                    keyPrev = keyPress;
                    keyState = 1;
                }
                break;
                
            case 1:
                if(keyPress == keyPrev)
                {
                    keyState = 2;
                    keyReturn= keyPrev | KEY_DOWN;
                }
                else 				//按键抬起,是抖动,不响应按键
                {
                    keyState = 0;
                }
                break;
                
            case 2:

                if(keyPress != keyPrev)
                {
                    keyState = 0;
                    keyLongCheck = 0;
                    keyReturn = keyPrev | KEY_UP;
                    return keyReturn;
                }
                if(keyPress == keyPrev)
                {
                    keyLongCheck++;
                    if(keyLongCheck >= 100)    //长按2S
                    {
                        keyLongCheck = 0;
                        keyState = 3;
                        keyReturn= keyPress | KEY_LONG;
                        return keyReturn;
                    }
                }
                break;

            case 3:
                if(keyPress != keyPrev)
                {
                    keyState = 0;
                }
                break;
        }
    }
    return  NO_KEY;
}

/*******************************************************************************
* Function Name  : keyHandle
* Description    : Key processing function
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void keyHandle(keysTypedef_t *keyS)
{
    uint8_t keyReturn =0;

    keyReturn = readKeyValue(keyS);

    if(NO_KEY == keyReturn)
    {
        return ;
    }
    if(keyReturn & KEY_UP)
    {
        if(keyReturn & PRESS_KEY1)
        {
            keyS->singleKey[0].shortPress();
        }
        if(keyReturn & PRESS_KEY2)
        {
            keyS->singleKey[1].shortPress();
        }
    }

    if(keyReturn & KEY_LONG)
    {
        if(keyReturn & PRESS_KEY1)
        {
            keyS->singleKey[0].longPress();
        }
        if(keyReturn & PRESS_KEY2)
        {
            keyS->singleKey[1].longPress();
        }
    }
} 

