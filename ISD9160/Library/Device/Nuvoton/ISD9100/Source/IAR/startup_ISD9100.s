/**************************************************
 *
 * Part one of the system initialization code, contains low-level
 * initialization, plain thumb variant.
 *
 * Copyright 2010 IAR Systems. All rights reserved.
 *
 * $Revision: 34539 $
 *
 **************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
;        EXTERN  SystemInit
        PUBLIC  __vector_table

        DATA
__vector_table
                DCD     sfe(CSTACK)               ; Top of Stack
                DCD     Reset_Handler       	  ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                                                  ; maximum of 32 External Interrupts are possible
                DCD     BOD_IRQHandler            ; Brownout low voltage detector interrupt
                DCD     WDT_IRQHandler            ; Watch Dog Timer interrupt
                DCD     EINT0_IRQHandler          ; External signal interrupt from PB.0 pin
                DCD     EINT1_IRQHandler          ; External signal interrupt from PB.1 pin
                DCD     GPAB_IRQHandler           ; External signal interrupt from PA[15:0] / PB[7:2]
                DCD     ALC_IRQHandler            ; Automatic Level Control Interrupt
                DCD     PWMA_IRQHandler           ; PWM0 , PWM1 interrupt
                DCD     0                         ; Reserved
                DCD     TMR0_IRQHandler           ; Timer 0 interrupt
                DCD     TMR1_IRQHandler           ; Timer 1 interrupt
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     UART02_IRQHandler         ; UART0 interrupt
                DCD     0                         ; Reserved
                DCD     SPI0_IRQHandler           ; SPI0 interrupt
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     I2C0_IRQHandler           ; I2C0 interrupt
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     TALARM_IRQHandler         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     ACMP_IRQHandler           ; Analog Comparator-0 or Comaprator-1 interrupt
                DCD     PDMA_IRQHandler           ; PDMA interrupt
                DCD     I2S_IRQHandler            ; I2S interrupt
                DCD     CAPS_IRQHandler           ; Capacitive Touch Sensing Relaxation Oscillator Interrupt
                DCD     SDADC_IRQHandler          ; ADC interrupt
                DCD     Default_Handler           ; Reserved
                DCD     RTC_IRQHandler            ; Real time clock interrupt


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
      PUBWEAK Reset_Handler
      PUBWEAK NMI_Handler
      PUBWEAK HardFault_Handler
      PUBWEAK SVC_Handler
      PUBWEAK PendSV_Handler
      PUBWEAK SysTick_Handler
      PUBWEAK BOD_IRQHandler
      PUBWEAK WDT_IRQHandler
      PUBWEAK EINT0_IRQHandler
      PUBWEAK EINT1_IRQHandler
      PUBWEAK GPAB_IRQHandler
      PUBWEAK ALC_IRQHandler
      PUBWEAK PWMA_IRQHandler
      PUBWEAK TMR0_IRQHandler
      PUBWEAK TMR1_IRQHandler
      PUBWEAK UART02_IRQHandler
      PUBWEAK SPI0_IRQHandler
      PUBWEAK I2C0_IRQHandler
      PUBWEAK TALARM_IRQHandler
      PUBWEAK ACMP_IRQHandler
      PUBWEAK PDMA_IRQHandler
      PUBWEAK I2S_IRQHandler
      PUBWEAK CAPS_IRQHandler
      PUBWEAK SDADC_IRQHandler
      PUBWEAK RTC_IRQHandler


        THUMB
        SECTION .text:CODE:REORDER(2)
Reset_Handler
;                LDR     R0, =SystemInit
;                BLX     R0
;*************Add by Nuvoton***************

;******************************************
               LDR     R0, =__iar_program_start
               BX      R0
              SECTION .text:CODE:REORDER(2)
NMI_Handler
HardFault_Handler
        LDR    R0, [R13, #24]        ; Get previous PC
        LDRH   R1, [R0]              ; Get instruction
        LDR    R2, =0xBEAB           ; The sepcial BKPT instruction
        CMP    R1, R2                ; Test if the instruction at previous PC is BKPT
        BNE    HardFault_Handler_Ret ; Not BKPT

        ADDS   R0, #4                ; Skip BKPT and next line
        STR    R0, [R13, #24]        ; Save previous PC

        BX     LR
HardFault_Handler_Ret
        B      .

SVC_Handler
PendSV_Handler
SysTick_Handler
BOD_IRQHandler
WDT_IRQHandler
EINT0_IRQHandler
EINT1_IRQHandler
GPAB_IRQHandler
ALC_IRQHandler
PWMA_IRQHandler
TMR0_IRQHandler
TMR1_IRQHandler
UART02_IRQHandler
SPI0_IRQHandler
I2C0_IRQHandler
TALARM_IRQHandler
ACMP_IRQHandler
PDMA_IRQHandler
I2S_IRQHandler
CAPS_IRQHandler
SDADC_IRQHandler
RTC_IRQHandler
Default_Handler
        B Default_Handler

        END
