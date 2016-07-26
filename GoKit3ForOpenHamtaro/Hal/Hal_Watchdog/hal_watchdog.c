/**
********************************************************
*
* @file      hal_watchdog.c
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
/*
void IWDG_Init(u8 prer，u16 rlr)是独立看门狗初始化函数，就是按照
上面介绍的步骤来初始化独立看门狗的。该函数有2 个参数，分别用来设置与预分频数与重装
寄存器的值的。通过这两个参数，就可以大概知道看门狗复位的时间周期为多少了。其计算方
式上面有详细的介绍，这里不再多说了。
void IWDG_Feed(void)函数，该函数用来喂狗，因为STM32 的喂狗只需要向键值寄存器写
入0XAAAA 即可
*/

#include "hal_watchdog.h"
#define WATCHDOG
/*
Tout=((4×2^prer) ×rlr) /40 = ((4 * 2^4) * 625) / 40 = 1000ms = 1s 
prer 为看门狗时钟预分频值（IWDG_PR 值），范围为0~7 ; 
rlr 为看门狗的重装载值（IWDG_RLR 的值）
1s内喂狗,看门狗就可以不复位 , 看门狗时钟不标准的40kHz
*/
void watchdogInit(uint8_t timeoutS)//宏
{
#ifdef  WATCHDOG
    /* 输入 参数修改 */
    uint8_t prer = 4;
    uint16_t rlr = timeoutS * 625;
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* 使能对寄存器IWDG_PR和IWDG_RLR的写操作*/
    IWDG_SetPrescaler(prer);    /*设置IWDG预分频值:设置IWDG预分频值*/
    IWDG_SetReload(rlr);     /*设置IWDG重装载值*/
    IWDG_ReloadCounter();    /*按照IWDG重装载寄存器的值重装载IWDG计数器*/
    IWDG_Enable();        /*使能IWDG*/
#endif
}

// 喂狗
void watchdogFeed(void)
{
#ifdef  WATCHDOG
    IWDG->KR=0xaaaa;
#endif
}

