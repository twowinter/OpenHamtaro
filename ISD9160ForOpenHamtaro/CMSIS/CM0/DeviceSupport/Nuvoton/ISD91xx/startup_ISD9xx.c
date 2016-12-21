/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 *            (C) COPYRIGHT 2009 ARM Limited.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 *
 *      SVN Information
 *
 *      Checked In          : $Date: Thu Jul  2 18:34:07 2009 $
 *
 *      Revision            : $Revision: 1.1 $
 *
 *      Release Information : Cortex-M0-AT510-r0p0-01rel0
 *-----------------------------------------------------------------------------
 */

//
// Boot (vectors) file for Cortex-M0 Integration Kit
//

//#include <stdio.h>
#include <rt_misc.h>
#include "ISD9xx.h"
//#include <stdint.h>


//
// Build a stand-alone image
//

//#pragma import(__use_no_semihosting)
#pragma import(__use_no_heap)

#define _TOP_OF_SP_ 0x20003000


//
// Dummy Interrupt Handlers
//
// The following functions are defined weakly to allow the user
// to override them at link time simply by declaring their own
// function of the same name.
//
// If no user function is provided, the weak function is used.
//

__weak void NMI_Handler(void)
{
  while(1);
}

__weak void HardFault_Handler(void)
   {
  while(1);
}

__weak void SVC_Handler(void)
{
  while(1);
}

__weak void PendSV_Handler(void)
{
  while(1);
}

__weak void SysTick_Handler(void)
{
  while(1);
}

__weak void GPIO_IRQHandler(void) 
{
  while(1);
}

__weak void Default_IRQHandler(void)
{
  while(1);
}

 __weak void BOD_IRQHandler(void)
{
  while(1);
}
__weak void WDT_IRQHandler(void)
{
  while(1);
}
__weak void EINT0_IRQHandler(void)
{
  while(1);
}
__weak void EINT1_IRQHandler(void)
{
  while(1);
}
__weak void GPAB_IRQHandler(void)
{
  while(1);
}
__weak void PWMA_IRQHandler(void)
{
  while(1);
}
__weak void TMR0_IRQHandler(void)
{
  while(1);
}
__weak void TMR1_IRQHandler(void)
{
  while(1);
}
__weak void UART02_IRQHandler(void)
{
  while(1);
}
__weak void SPI0_IRQHandler(void)
{
  while(1);
}
__weak void DPWM_IRQHandler(void)
{
  while(1);
}
__weak void I2C0_IRQHandler(void)
{
  while(1);
}
__weak void ACMP_IRQHandler(void)
{
  while(1);
}
__weak void PDMA_IRQHandler(void)
{
  while(1);
}
__weak void I2S_IRQHandler(void)
{
  while(1);
}
__weak void ADC_IRQHandler(void)
{
  while(1);
}
__weak void CAPS_IRQHandler(void)
{
  while(1);
}
__weak void RTC_IRQHandler(void)
{
  while(1);
}
__weak void ALC_IRQHandler(void)
{
  while(1);
}
__weak void TALARM_IRQHandler(void)
{
  while(1);
}




//
// Reset Handler
//

extern void __main(void);


void Reset_Handler(void)
{
/*  check ICE_CLK and ICE_DAT to disable power down to the chip */
	if (SYSCLK->DBGPD.ICE_CLK == 0 && SYSCLK->DBGPD.ICE_DAT == 0)
		SYSCLK->DBGPD.DISABLE_PD = 1;
	__main();
}


//
// Set up Vector Table
//

typedef void (*const vect_t)(void) __irq;

vect_t __Vectors[]
__attribute__ ((section("vectors"))) = {
  (vect_t)(_TOP_OF_SP_),    // Top of Stack 
  (vect_t)Reset_Handler,    // Reset Handler
  (vect_t)NMI_Handler,      // NMI Handler
  (vect_t)HardFault_Handler,// Hard Fault Handler
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)SVC_Handler,      // SVCall Handler
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)PendSV_Handler,   // PendSV Handler
  (vect_t)SysTick_Handler,  // SysTick Handler

  // External Interrupts
   //0-3
  (vect_t)BOD_IRQHandler,  
  (vect_t)WDT_IRQHandler,
  (vect_t)EINT0_IRQHandler,
  (vect_t)EINT1_IRQHandler,
  //4-7
  (vect_t)GPAB_IRQHandler,
  (vect_t)ALC_IRQHandler,
  (vect_t)PWMA_IRQHandler,
  (vect_t)Default_IRQHandler,
  //8-11
  (vect_t)TMR0_IRQHandler,
  (vect_t)TMR1_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  //12-15
  (vect_t)UART02_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)SPI0_IRQHandler,
  (vect_t)DPWM_IRQHandler,
  //16-19
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)I2C0_IRQHandler,
  (vect_t)Default_IRQHandler,
  //20-23
  (vect_t)GPIO_IRQHandler   ,		 
  (vect_t)TALARM_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  //24-27
  (vect_t)Default_IRQHandler,
  (vect_t)ACMP_IRQHandler,
  (vect_t)PDMA_IRQHandler,
  (vect_t)I2S_IRQHandler,
  //28-31
  (vect_t)CAPS_IRQHandler,
  (vect_t)ADC_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)RTC_IRQHandler
};


//
// Set up initial stack and heap
//

__value_in_regs struct __initial_stackheap
__user_initial_stackheap(unsigned hb, unsigned sb, unsigned hl, unsigned sl)
{	
  struct __initial_stackheap s;
	
  s.heap_base   = hb;
  s.stack_base  = sb;
  s.heap_limit  = s.stack_base;
  s.stack_limit = s.heap_base;
  return s;
}


__asm void define_the_sp(void)
{
	EXPORT __initial_sp
__initial_sp EQU _TOP_OF_SP_ ; top of the stack
}

//
// Set test status bits in testbench when main() exits
//

void _sys_exit(int return_code)
{
      while(1);
}

