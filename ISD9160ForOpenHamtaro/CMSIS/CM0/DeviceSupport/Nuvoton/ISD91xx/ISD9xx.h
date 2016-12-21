/*----------------------------------------------------------------------------------------------*/
/*                                                                                              */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                  */
/*                                                                                              */
/*----------------------------------------------------------------------------------------------*/
/*
 * @file     ISD0xx.h
 * @brief    CMSIS Cortex-M0 Core Peripheral Access Layer Header File
 *           for ISD91XX CM0 Device Series
 * @date     5. April 2011
 *
 ------------------------------------------------------------------------------------------------*/
#ifndef __ISD9xx_H__
#define __ISD9xx_H__

#ifdef __cplusplus
extern "C" {
#endif 
#if 0
} /* Fool Emacs into not indenting things. */
#endif
/** @addtogroup ISD91XX_Definitions ISD91XX Definitions
    This file defines all structures and symbols for ISD91XX:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - Peripheral definitions
    @{
*/
/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup ISD91XX_CMSIS Device CMSIS Definitions
    Configuration of the Cortex-M# Processor and Core Peripherals
    @{
*/
/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */
 
typedef enum IRQn
    {
        /******  Cortex-M0 Processor Exceptions Numbers *************************************************/
        NonMaskableInt_IRQn       = -14,    /*!< 2 Non Maskable Interrupt                               */
        HardFault_IRQn		      = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                       */
        SVCall_IRQn               = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                         */
        PendSV_IRQn               = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                         */
        SysTick_IRQn              = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                     */
        /******  ARMIKMCU Swift specific Interrupt Numbers **********************************************/
        BOD_IRQn                  = 0,        /*!< 16 Brown Out Detector Device Interrupt               */
        WDT_IRQn                  = 1,        /*!< 17 Watchdog Timer Device Interrupt                   */   
        EINT0_IRQn                = 2,        /*!< 18 External Interrupt PB0 Interrupt                    */
        EINT1_IRQn                = 3,        /*!< 19 External Interrupt PB1 Interrupt                    */
        GPAB_IRQn                 = 4,        /*!< 20 GPIO Interrupt                                    */  
        ALC_IRQn                  = 5,        /*!< 21 ALC Interrupt                                     */
        PWMA_IRQn                 = 6,        /*!< 22 PWM Peripheral Device Interrupt                   */  
        IRQ7n                     = 7,
        TMR0_IRQn                 = 8,        /*!< 24 Timer 0 Interrupt                                 */
        TMR1_IRQn                 = 9,        /*!< 25 Timer 1 Interrupt                                 */
        IRQ10n                    = 10,
        IRQ11n                    = 11,
        UART0_IRQn                = 12,        /*!< 28 UART Device Interrupt                            */    
        IRQ13n                    = 13,
        SPI0_IRQn                 = 14,        /*!< 30 SPI Interface Interrupt                          */  
        IRQ15n                    = 15,
        IRQ16n                    = 16,
        IRQ17n                    = 17,
        I2C0_IRQn                 = 18,        /*!< 34 I2C Interface Interrupt                          */    
        IRQ19n                    = 19,
        IRQ20n                    = 20,        
        TALARM_IRQn               = 21,        /*!< 37 Temperature Alaram Interrupt                     */   
        IRQ22n                    = 22,
        IRQ23n                    = 23,
        IRQ24n                    = 24,
        ACMP_IRQn                 = 25,        /*!< 41 Analog Comparator Interrupt                      */    
        PDMA_IRQn                 = 26,        /*!< 42 Peripheral DMA Interrupt                         */
        I2S_IRQn                  = 27,        /*!< 43 I2S Interface Device Interrupt                   */ 
        CAPS_IRQn                 = 28,        /*!< 44 CapSense Device Interrupt                        */
        ADC_IRQn                  = 29,        /*!< 45 Audio ADC Device Interrupt                       */ 
        IRQ30n                    = 30,
        RTC_IRQn                  = 31         /*!< 47 Real Time Clock Interrupt                        */
         
        /*!< maximum of 32 Interrupts are possible                */
    } IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __MPU_PRESENT           0       /*!< armikcmu does not provide a MPU present or not       */
#define __NVIC_PRIO_BITS        2       /*!< armikcmu Supports 2 Bits for the Priority Levels     */
#define __Vendor_SysTickConfig  0       /*!< Set to 1 if different SysTick Config is used         */
/*@}*/ /* end of group ISD91XX_CMSIS */

#include "core_cm0.h"                   /* Cortex-M0 processor and core peripherals               */
#include "system_ISD9xx.h"              /* ISD9xx System include file                             */
#include "System\SysInfra.h"    


     
/**
 * Initialize the system clock
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialize the Oscillator and update the SystemFrequency variable
 */
extern void SystemInit (void);


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/
/** @addtogroup ISD91XX_Peripherals ISD91XX Peripherals
    ISD91XX Device Specific Peripheral registers structures
    @{
*/

/*--------------------- General Purpose Input and Ouptut ---------------------*/
/** @addtogroup ISD91XX_GPIO ISD91XX GPIO Control Registers (GPIO)
    @{
*/
typedef struct
{
    __IO uint32_t PMD0:2;
    __IO uint32_t PMD1:2;
    __IO uint32_t PMD2:2;
    __IO uint32_t PMD3:2;
    __IO uint32_t PMD4:2;
    __IO uint32_t PMD5:2;
    __IO uint32_t PMD6:2;
    __IO uint32_t PMD7:2;
    __IO uint32_t PMD8:2;
    __IO uint32_t PMD9:2;
    __IO uint32_t PMD10:2;
    __IO uint32_t PMD11:2;
    __IO uint32_t PMD12:2;
    __IO uint32_t PMD13:2;
    __IO uint32_t PMD14:2;
    __IO uint32_t PMD15:2;
} GPIO_PMD_T;

typedef __IO uint32_t GPIO_INDIS_T;
typedef __IO uint32_t GPIO_DOUT_T;
typedef __IO uint32_t GPIO_DMASK_T;
typedef __IO uint32_t GPIO_PIN_T;
typedef __IO uint32_t GPIO_DBEN_T;
typedef __IO uint32_t GPIO_IMD_T;
typedef __IO uint32_t GPIO_IEN_T;
typedef __IO uint32_t GPIO_ISRC_T;

typedef struct
{
    __IO uint32_t  DBCLKSEL:4;
    __IO uint32_t  DBCLKSRC:1;
    __IO uint32_t  ICLK_ON:1;
    __I  uint32_t  RESERVE:26;    
} GPIO_DBNCECON_T;

typedef struct
{
    GPIO_PMD_T      PMD;                 /*!< Offset: 0x0000   GPIO Mode Control Register           */  
    GPIO_INDIS_T    INDIS;               /*!< Offset: 0x0004   GPIO Input Disable Register          */  
    GPIO_DOUT_T     DOUT;                /*!< Offset: 0x0008   GPIO Data Output Value Register      */  
    GPIO_DMASK_T    DMASK;               /*!< Offset: 0x000C   GPIO Data Output Write Mask Register */  
    GPIO_PIN_T      PIN;                 /*!< Offset: 0x0010   GPIO Pin Value Register              */  
    GPIO_DBEN_T     DBEN;                /*!< Offset: 0x0014   GPIO De-bounce Enable Register       */  
    GPIO_IMD_T      IMD;                 /*!< Offset: 0x0018   GPIO Interrupt Mode Control          */  
    GPIO_IEN_T      IEN;                 /*!< Offset: 0x001C   GPIO Interrupt Enable Control        */     
    GPIO_ISRC_T     ISRC;                /*!< Offset: 0x0020   GPIO Interrupt Source                */  
    
} GPIO_T;
/*@}*/ /* end of group ISD91XX_GPIO */
     
/*------------------------- UART Interface Controller ------------------------*/

typedef __IO uint8_t UART_DATA_T;


typedef struct
{
    __IO uint32_t  RDA_IEN:1;
    __IO uint32_t  THRE_IEN:1;
    __IO uint32_t  RLS_IEN:1;
    __IO uint32_t  MS_IEN:1;
    __IO uint32_t  RTO_IEN:1;	  
    __IO uint32_t  BUF_ERR_IEN:1;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  LIN_RX_BRK_IEN:1;
    __I  uint32_t  RESERVE1:2;
    __IO uint32_t  TOC_EN:1;      /* Time-out counter enable */
    __IO uint32_t  AUTO_RTS_EN:1;
    __IO uint32_t  AUTO_CTS_EN:1;
    __IO uint32_t  DMA_TX_EN:1;
    __IO uint32_t  DMA_RX_EN:1;
    __I  uint32_t  RESERVE2:15;    
    __IO uint32_t  nDEBUGACK_EN:1;
} UART_IER_T;

typedef struct
{
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  RFR:1;
    __IO uint32_t  TFR:1;
    __I  uint32_t  RESERVE1:1;
    __IO uint32_t  RFITL:4;             /* Rx FIFO Interrupt Trigger Level */
    __I  uint32_t  RESERVE2:8;
    __IO uint32_t  RTS_TRIG_LEVEL:4;
    __I  uint32_t  RESERVE3:12;
} UART_FCR_T;

typedef struct
{
    __IO uint32_t  WLS:2;               /* Word length select */
    __IO uint32_t  NSB:1;               /* Number of STOP bit */
    __IO uint32_t  PBE:1;               /* Parity bit enable  */
    __IO uint32_t  EPE:1;               /* Even parity enable */
    __IO uint32_t  SPE:1;               /* Stick parity enable*/
    __IO uint32_t  BCB:1;               /* Break control bit  */
    __I  uint32_t  RESERVE:25;
} UART_LCR_T;

typedef struct
{
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  RTS_SET:1;
    __I  uint32_t  RESERVE1:2;
    __IO uint32_t  LBME:1;
    __I  uint32_t  RESERVE2:4;
    __IO uint32_t  RTS_ACT:1;
    __I  uint32_t  RESERVE3:3;
    __I  uint32_t  RTS_ST:1;               /* RTS status */
    __I  uint32_t  RESERVE4:18;
} UART_MCR_T;

typedef struct
{
    __IO uint32_t  DCTS:1;
    __I  uint32_t  RESERVE0:3;
    __I  uint32_t  CTS:1;               /* CTS status */
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  CTS_ACT:1;
    __I  uint32_t  RESERVE2:23;
} UART_MSR_T;

typedef struct
{
    __IO uint32_t  RX_OVF_IF:1;
    __I  uint32_t  RESERVE0:3;
    __IO uint32_t  PEF:1;
    __IO uint32_t  FEF:1;
    __IO uint32_t  BIF:1;
    __I  uint32_t  RESERVE1:1;
    __I  uint32_t  RX_POINTER:6;
    __I  uint32_t  RX_EMPTY:1;
    __I  uint32_t  RX_FULL:1;
    __I  uint32_t  TX_POINTER:6;
    __I  uint32_t  TX_EMPTY:1;
    __I  uint32_t  TX_FULL:1;
    __IO uint32_t  TX_OVF_IF:1;
    __I  uint32_t  RESERVE2:3;
    __I  uint32_t  TE:1;                /* Transmitter empty */
    __I  uint32_t  RESERVE3:3;
} UART_FSR_T;

typedef struct
{
    __IO uint32_t  RDA_IF:1;
    __IO uint32_t  THRE_IF:1;
    __IO uint32_t  RLS_IF:1;
    __IO uint32_t  MODEM_IF:1;
    __IO uint32_t  TOUT_IF:1;
    __IO uint32_t  BUF_ERR_IF:1;
    __IO uint32_t  WAKE_IF:1;
    __IO uint32_t  LIN_Rx_Break_IF:1;
    __IO uint32_t  RDA_INT:1;
    __IO uint32_t  THRE_INT:1;
    __IO uint32_t  RLS_INT:1;
    __IO uint32_t  MODEM_INT:1;
    __IO uint32_t  TOUT_INT:1;
    __IO uint32_t  BUF_ERR_INT:1;
    __IO uint32_t  WAKE_INT:1;
    __IO uint32_t  LIN_Rx_Break_INT:1;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  DMA_RLS_IF:1;
    __IO uint32_t  DMA_MODEM_IF:1;
    __IO uint32_t  DMA_TOUT_IF:1;
    __IO uint32_t  DMA_BUF_ERR_IF:1;
    __I  uint32_t  RESERVE1:1;
    __IO uint32_t  DMA_LIN_Rx_Break_IF:1;
    __I  uint32_t  RESERVE2:2;
    __IO uint32_t  DMA_RLS_INT:1;
    __IO uint32_t  DMA_MODEM_INT:1;
    __IO uint32_t  DMA_TOUT_INT:1;
    __IO uint32_t  DMA_BUF_ERR_INT:1;
    __I  uint32_t  RESERVE3:1;
    __IO uint32_t  DMA_LIN_Rx_Break_INT:1;
} UART_ISR_T;

typedef struct
{
    __IO uint32_t  TOIC:7;
    __I  uint32_t  RESERVE0:25;
} UART_TOR_T;

typedef struct
{
    __IO uint32_t  BRD:16;
    __I  uint32_t  RESERVE0:8;
    __IO uint32_t  DIVX:4;
    __IO uint32_t  DIVX_ONE:1;            
    __IO uint32_t  DIVX_EN:1;            
    __I  uint32_t  RESERVE1:2;
} UART_BAUD_T;

typedef struct
{
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  TX_SELECT:1;
    __IO uint32_t  LOOPBACK:1;            
    __IO uint32_t  RX_DEB_EN:1;            
    __I  uint32_t  RESERVE1:1;
    __IO uint32_t  TX_INV_EN:1;            
    __IO uint32_t  RX_INV_EN:1;
    __I  uint32_t  RESERVE2:25;
} UART_IRCR_T;

typedef struct
{
    __IO uint32_t  LINBCNT:4;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  LINRX_EN:1;
    __IO uint32_t  LINTX_EN:1;            
    __I  uint32_t  RESERVE1:24;
} UART_LINCON_T;



typedef struct
{
    __IO uint32_t  LIN_EN:1;
    __IO uint32_t  IrDA_EN:1;
    __I  uint32_t  RESERVE0:30;
} UART_FUNSEL_T;


typedef struct
{
    UART_DATA_T     DATA;
    __I  uint8_t    RESERVE0[3];
    UART_IER_T      IER;
    UART_FCR_T      FCR;
    UART_LCR_T      LCR;
    UART_MCR_T      MCR;
    UART_MSR_T      MSR;
    UART_FSR_T      FSR;
    UART_ISR_T      ISR;
    UART_TOR_T      TOR;
    UART_BAUD_T     BAUD;
    UART_IRCR_T     IRCR;
    UART_LINCON_T   LINCON;    
    UART_FUNSEL_T   FUNSEL;    
} UART_T;

/*----------------------------- Timer Controller -----------------------------*/
typedef struct
{
    __IO uint32_t  PRESCALE:8;
    __I  uint32_t  RESERVE0:8;
    __IO uint32_t  TDR_EN:1;
    __I  uint32_t  RESERVE1:8;
    __IO uint32_t  CACT:1;
    __IO uint32_t  CRST:1;
    __IO uint32_t  MODE:2;
    __IO uint32_t  IE:1;
    __IO uint32_t  CEN:1;
    __IO uint32_t  nDBGACK_EN:1;
} TIMER_TCSR_T;

typedef __IO uint32_t TIMER_TCMPR_T;

typedef __IO uint32_t TIMER_TDR_T;

typedef struct
{
    __IO uint32_t  TIF:1;
    __I  uint32_t  RESERVE:31;
} TIMER_TISR_T;

typedef struct
{
    TIMER_TCSR_T     TCSR;
    TIMER_TCMPR_T    TCMPR;
    TIMER_TISR_T     TISR;    
    TIMER_TDR_T      TDR;
} TIMER_T;


/*----------------------------- WDT Controller -----------------------------*/
typedef struct
{
    __IO uint32_t  WTR:1;
    __IO uint32_t  WTRE:1;
    __IO uint32_t  WTRF:1;
    __IO uint32_t  WTIF:1;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  WTIE:1;
    __IO uint32_t  WTE:1;
    __IO uint32_t  WTIS:3;
    __I  uint32_t  RESERVE1:21;
} WDT_WTCR_T;

typedef struct
{
    WDT_WTCR_T     WTCR;
    
} WDT_T;

/*------------------------- SPI Interface Controller -------------------------*/
typedef struct
{
    __IO uint32_t  GO_BUSY:1;
    __IO uint32_t  RX_NEG:1;
    __IO uint32_t  TX_NEG:1;
    __IO uint32_t  TX_BIT_LEN:5;
    __IO uint32_t  TX_NUM:2;
    __IO uint32_t  LSB:1;
    __IO uint32_t  CLKP:1;
    __IO uint32_t  SLEEP:4;
    __IO uint32_t  IF:1;
    __IO uint32_t  IE:1;
    __IO uint32_t  SLAVE:1;
    __IO uint32_t  BYTE_SLEEP:1;
    __IO uint32_t  BYTE_ENDIAN:1;
    __IO uint32_t  FIFO:1;
    __IO uint32_t  TWOB:1;
    __IO uint32_t  VARCLK_EN:1;
    __IO uint32_t  RX_EMPTY:1;
    __IO uint32_t  RX_FULL:1;
    __IO uint32_t  TX_EMPTY:1;
    __IO uint32_t  TX_FULL:1;
    __IO uint32_t  DMA_ASS_BURST:1;
    __I  uint32_t  RESERVE:3;

} SPI_CNTRL_T;

typedef struct
{
    __IO uint32_t  DIVIDER:16;
    __IO uint32_t  DIVIDER2:16;
} SPI_DIVIDER_T;

typedef struct
{
    __IO uint32_t  SSR:2;
    __IO uint32_t  SS_LVL:1;
    __IO uint32_t  ASS:1;
    __IO uint32_t  SS_LTRIG:1;
    __IO uint32_t  LTRIG_FLAG:1;
    __I  uint32_t  RESERVE:26;
} SPI_SSR_T;


typedef __I  uint32_t   SPI_RX_T;
typedef __O  uint32_t   SPI_TX_T;

typedef struct
{
    __IO uint32_t  JS:1;
    __I  uint32_t  RESERVE0:3;
    __IO uint32_t  JS_RW:1;
    __IO uint32_t  CS_ACT:1;
    __IO uint32_t  DATA_RDY:1;
    __IO uint32_t  CS_DEACT:1;
    __IO uint32_t  READYB:1;
    __I  uint32_t  RESERVE1:23;
} SPI_JS_T;

typedef __IO uint32_t SPI_VARCLK_T;

typedef struct
{
    __IO uint32_t  TX_DMA_GO:1;
    __IO uint32_t  RX_DMA_GO:1;
    __I  uint32_t  RESERVE:30;
} SPI_DMA_T;

typedef struct
{
    SPI_CNTRL_T     CNTRL;
    SPI_DIVIDER_T   DIVIDER;
    SPI_SSR_T       SSR;
    uint32_t        RESERVE0;
    SPI_RX_T        RX[2];
    uint32_t        RESERVE1;
    uint32_t        RESERVE2;
    SPI_TX_T        TX[2];
    uint32_t        RESERVE3;
    uint32_t        RESERVE4;
    SPI_JS_T        JS;
    SPI_VARCLK_T    VARCLK;
    SPI_DMA_T       DMA;
} SPI_T;

/*------------------------------ I2C Controller ------------------------------*/
typedef struct
{
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  AA:1;
    __IO uint32_t  SI:1;
    __IO uint32_t  STO:1;
    __IO uint32_t  STA:1;
    __IO uint32_t  ENSI:1;
    __IO uint32_t  EI:1;
    __I  uint32_t  RESERVE1:24;
} I2C_CON_T;

typedef struct
{
    __IO uint32_t  GC:1;
    __IO uint32_t  ADDR:7;
    __I  uint32_t  RESERVE:24;
} I2C_ADDR_T;

typedef __IO uint32_t I2C_DATA_T;

typedef __I  uint32_t I2C_STATUS_T;

typedef __IO uint32_t I2C_CLK_T;

typedef struct
{
    __IO uint32_t  TIF:1;
    __IO uint32_t  DIV4:1;
    __IO uint32_t  ENTI:1;
    __I  uint32_t  RESERVE:29;
} I2C_TOC_T;

typedef struct
{
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  ADM:7;
    __I  uint32_t  RESERVE1:24;
} I2C_ADRM_T;

typedef struct
{
    I2C_CON_T       CON;
    I2C_ADDR_T      ADDR0;
    I2C_DATA_T      DATA;
    I2C_STATUS_T    STATUS;
    I2C_CLK_T       CLK;
    I2C_TOC_T       TOC;
    I2C_ADDR_T	    ADDR1;
    I2C_ADDR_T	    ADDR2;
    I2C_ADDR_T	    ADDR3;
    I2C_ADRM_T		ADRM0;
    I2C_ADRM_T		ADRM1;
    I2C_ADRM_T		ADRM2;
    I2C_ADRM_T	    ADRM3;  
    __I uint32_t    DATE;
    __I uint32_t    VERSION;
    __IO uint32_t   High_Speed;      
} I2C_T;


/*----------------------------- RTC Controller -------------------------------*/

typedef __IO uint32_t RTC_INIR_T;

typedef struct
{
    __IO uint32_t  AER:16;
    __I  uint32_t  ENF:1;
    __I  uint32_t  RESERVE1:15;
} RTC_AER_T;

typedef struct
{
    __IO uint32_t  FRACTION:6;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  INTEGER:4;
    __I  uint32_t  RESERVE1:20;
} RTC_FCR_T;

typedef struct
{
    __IO uint32_t  SEC1:4;
    __IO uint32_t  SEC10:3;
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  MIN1:4;
    __IO uint32_t  MIN10:3;
    __I  uint32_t  RESERVE1:1;
    __IO uint32_t  HR1:4;
    __IO uint32_t  HR10:2;
    __I  uint32_t  RESERVE2:10;
} RTC_TLR_T;

typedef struct
{
    __IO uint32_t  DAY1:4;
    __IO uint32_t  DAY10:2;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  MON1:4;
    __IO uint32_t  MON10:1;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  YEAR1:4;
    __IO uint32_t  YEAR10:4;
    __I  uint32_t  RESERVE2:8;
} RTC_CLR_T;

typedef struct
{
    __IO uint32_t  HR24:1;
    __I  uint32_t  RESERVE:31;
} RTC_TSSR_T;

typedef struct
{
    __IO uint32_t  DWR:3;
    __I  uint32_t  RESERVE:29;
} RTC_DWR_T;

typedef RTC_TLR_T   RTC_TAR_T;
typedef RTC_CLR_T   RTC_CAR_T;

typedef struct
{
    __IO uint32_t  LIR:1;
    __I  uint32_t  RESERVE:31;
} RTC_LIR_T;

typedef struct
{
    __IO uint32_t  AIER:1;
    __IO uint32_t  TIER:1;
    __I  uint32_t  RESERVE:30;
} RTC_RIER_T;



typedef struct
{
    __IO uint32_t  AI:1;
    __IO uint32_t  TI:1;
    __I  uint32_t  RESERVE:30;
} RTC_RIIR_T;

typedef struct
{
    __IO uint32_t  TTR:3;
    __IO uint32_t  TWKE:1;
    __I  uint32_t  RESERVE:28;
} RTC_TTR_T;

typedef struct
{
    RTC_INIR_T  INIR;
    RTC_AER_T   AER;
    RTC_FCR_T   FCR;
    RTC_TLR_T   TLR;
    RTC_CLR_T   CLR;
    RTC_TSSR_T  TSSR;
    RTC_DWR_T   DWR;
    RTC_TAR_T   TAR;
    RTC_CAR_T   CAR;
    RTC_LIR_T   LIR;
    RTC_RIER_T  RIER;
    RTC_RIIR_T  RIIR;
    RTC_TTR_T   TTR;
} RTC_T;


/*----------------------------- ADC Controller -------------------------------*/

typedef struct
{
    __IO uint32_t FIFO_IE_LEV:3;
    __I  uint32_t RESERVE:28;
    __IO uint32_t IE:1;
} ADC_INT_T;

typedef struct
{
    __IO uint32_t OSR:4;
    __I  uint32_t RESERVE:12;
    __IO uint32_t GAIN:4;
    __I  uint32_t RESERVE1:12;
} ADC_DEC_T;

typedef struct
{
    __IO uint32_t RxDmaEn:1;
    __I  uint32_t RESERVE:31;
} ADC_ADPDMA_T;

typedef struct
{
    __IO uint32_t CMPEN:1;
    __IO uint32_t CMPIE:1;
    __IO uint32_t CMPCOND:1;
    __I  uint32_t RESERVE:4;
    __IO uint32_t CMPF:1;
    __IO uint32_t CMPMATCHCNT:4;
    __I  uint32_t RESERVE1:4;
    __IO uint32_t CMPD:16;
}  ADC_ADCMPR_T;

typedef struct
{
    __I  uint32_t           ADCOUT;
    __IO uint32_t           EN;
    __IO uint32_t           CLK_DIV;
    ADC_DEC_T               DEC;
    ADC_INT_T               INT;
    ADC_ADPDMA_T			ADCPDMA;
    ADC_ADCMPR_T            ADCMPR[2];
} SDADC_T;

/*---------------------- Analog Comparator Controller -------------------------*/
typedef struct
{
    __IO uint32_t  CMPEN:1;
    __IO uint32_t  CMPIE:1;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  CMPCN:1;
    __I  uint32_t  RESERVE1:1;
    __I  uint32_t  RESERVE:26;
} ACMP_CMPCR_T;

typedef struct
{
    __IO uint32_t  CMPF0:1;
    __IO uint32_t  CMPF1:1;
    __I  uint32_t  CO0:1;
    __I  uint32_t  CO1:1;
    __I  uint32_t  RESERVE:28;
} ACMP_CMPSR_T;

typedef struct
{
    ACMP_CMPCR_T CMPCR[2];
    ACMP_CMPSR_T CMPSR;
    __IO uint32_t CMPSEL;
} ACMP_T;

/*---------------------------- Clock Controller ------------------------------*/
typedef struct
{
    __I  uint32_t  RESERVE0:1;	   // 0
    __IO uint32_t  XTL32K_EN:1;	   // 1	  
    __IO uint32_t  OSC49M_EN:1;	   // 2	  
    __IO uint32_t  OSC10K_EN:1;	   // 3	  
    __I  uint32_t  RESERVE4:5;	   // 4-8
    __IO uint32_t  STOP:1;         // 9	  
    __IO uint32_t  STANDBY_PD:1;   // 10  
    __IO uint32_t  DEEP_PD:1;      // 11  
    __I  uint32_t  RESERVE1:4;     // 12-15
    __IO uint32_t  PIN_ENB:1;      // 16	 
    __IO uint32_t  OSC10K_ENB:1;   // 17	 
    __I  uint32_t  RESERVE2:2;     // 18-19
    __IO uint32_t  TIMER_SEL:4;    // 20-23	 
    __I  uint32_t  PIN_WAKE:1;	   // 24
    __I  uint32_t  TIMER_WAKE:1;   // 25
    __I  uint32_t  POI_WAKE:1;	   // 26
    __I  uint32_t  RESERVE3:1;	   // 27
    __I  uint32_t  TIMER_SEL_RD:4; // 28-31
} SYSCLK_PWRCON_T;

typedef struct
{
    __IO uint32_t  CPU_EN:1;
    __IO uint32_t  PDMA_EN:1;
    __IO uint32_t  ISP_EN:1;
    __I  uint32_t  RESERVE1:29;
} SYSCLK_AHBCLK_T;


typedef struct
{
    __IO uint32_t  WDG_EN:1;	//0
    __I  uint32_t  RESERVE0:4;  //1-4
    __IO uint32_t  RTC_EN:1;	//5
    __IO uint32_t  TMR0_EN:1;	//6
    __IO uint32_t  TMR1_EN:1;	//7
    __IO uint32_t  I2C0_EN:1;	//8
    __I  uint32_t  RESERVE1:3;	//9-11
    __IO uint32_t  SPI0_EN:1;	//12
    __IO uint32_t  DPWM_EN:1;	//13
    __I  uint32_t  RESERVE2:2;	//14-15
    __IO uint32_t  UART0_EN:1;	//16
    __I  uint32_t  RESERVE3:1;	//17
    __IO uint32_t  BIQALC_EN:1;	//18
    __IO uint32_t  CRC_EN:1;	//19
    __IO uint32_t  PWM01_EN:1;	//20
    __I  uint32_t  RESERVE4:1;	//21
    __IO uint32_t  ACMP_EN:1;	//22
    __I  uint32_t  RESERVE5:3;	//23-25
    __IO uint32_t  SBRAM_EN:1;	//26
    __I  uint32_t  RESERVE6:1;	//27
    __IO uint32_t  ADC_EN:1;	//28
    __IO uint32_t  I2S_EN:1;	//29
    __IO uint32_t  ANA_EN:1;	//30
    __I  uint32_t  RESERVE7:1;	//31
}  SYSCLK_APBCLK_T;

typedef struct
{
    __IO uint32_t  CPU_EN:1;    //0
    __IO uint32_t  PDMA_EN:1;   //1
    __IO uint32_t  ISP_EN:1;    //2
    __I  uint32_t  RESERVE0:1;  //3
    __IO uint32_t  WDG_EN:1;	//4
    __IO uint32_t  RTC_EN:1;	//5
    __IO uint32_t  TMR0_EN:1;	//6
    __IO uint32_t  TMR1_EN:1;	//7
    __IO uint32_t  I2C0_EN:1;	//8
    __I  uint32_t  RESERVE1:3;	//9-11
    __IO uint32_t  SPI0_EN:1;	//12
    __IO uint32_t  DPWM_EN:1;	//13
    __I  uint32_t  RESERVE2:2;	//14-15
    __IO uint32_t  UART0_EN:1;	//16
    __I  uint32_t  RESERVE3:1;	//17
    __IO uint32_t  BIQALC_EN:1;	//18
    __IO uint32_t  CRC_EN:1;	//19
    __IO uint32_t  PWM01_EN:1;	//20
    __I  uint32_t  RESERVE4:1;	//21
    __IO uint32_t  ACMP_EN:1;	//22
    __I  uint32_t  RESERVE5:3;	//23-25
    __IO uint32_t  SBRAM_EN:1;	//26
    __I  uint32_t  RESERVE6:1;	//27
    __IO uint32_t  ADC_EN:1;	//28
    __IO uint32_t  I2S_EN:1;	//29
    __IO uint32_t  ANA_EN:1;	//30
    __I  uint32_t  RESERVE7:1;	//31
}  SYSCLK_CLKSLEEP_T;

typedef struct
{
    __IO uint32_t  DPD_STATE_WR:8;
    __I  uint32_t  DPD_STATE_RD:8;
    __I  uint32_t  RESERVE:16;
} SYSCLK_DPDSTATE_T;

typedef struct
{
    __IO uint32_t  HCLK_S:3;
    __IO uint32_t  STCLK_S:3;
    __IO uint32_t  OSCFSel:1;
    __I  uint32_t  RESERVE:25;
} SYSCLK_CLKSEL0_T;			  

typedef struct
{
    __IO uint32_t  WDG_S:2;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  DPWM_S:1;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  TMR0_S:3;
    __I  uint32_t  RESERVE2:1;
    __IO uint32_t  TMR1_S:3;
    __I  uint32_t  RESERVE3:13;
    __IO uint32_t  PWM01_S:2;
    __I  uint32_t  RESERVE4:2;
} SYSCLK_CLKSEL1_T;

typedef struct
{
    __IO uint32_t  I2S_S:2;
    __I  uint32_t  RESERVE:30;
} SYSCLK_CLKSEL2_T;

typedef struct
{
    __IO uint32_t  HCLK_N:4;
    __I  uint32_t  RESERVE0:4;
    __IO uint32_t  UART_N:4;
    __I  uint32_t  RESERVE1:4;
    __IO uint32_t  ADC_N:8;
    __I  uint32_t  RESERVE:8;
} SYSCLK_CLKDIV_T;


typedef struct
{    
    __IO uint32_t  FSEL:4;
    __IO uint32_t  FDIV_EN:1;
    __I  uint32_t  RESERVE:27;
} SYSCLK_FRQDIV_T;

typedef struct
{    
    __IO uint32_t  DS_FLAG:1;
    __IO uint32_t  STOP_FLAG:1;
    __IO uint32_t  PD_FLAG:1;
    __I  uint32_t  RESERVE:29;
} SYSCLK_PFLAGCON_T;

typedef struct
{    
    __IO uint32_t  DISABLE_PD:1;
    __I  uint32_t  RESERVE1:5;
    __I  uint32_t  ICE_CLK:1;
    __I  uint32_t  ICE_DAT:1;
    __I  uint32_t  RESERVE2:24;
} SYSCLK_DBGPD_T;

typedef struct
{
    SYSCLK_PWRCON_T    PWRCON;
    SYSCLK_AHBCLK_T    AHBCLK;
    SYSCLK_APBCLK_T    APBCLK;
    SYSCLK_DPDSTATE_T  DPDSTATE;
    SYSCLK_CLKSEL0_T   CLKSEL0;
    SYSCLK_CLKSEL1_T   CLKSEL1;
    SYSCLK_CLKDIV_T    CLKDIV;
    SYSCLK_CLKSEL2_T   CLKSEL2;
    SYSCLK_CLKSLEEP_T  CLKSLEEP;
    SYSCLK_PFLAGCON_T  PFLAGCON;
    SYSCLK_DBGPD_T     DBGPD;    
} SYSCLK_T;


/*---------------------------- Global Controller -----------------------------*/
typedef __I uint32_t GCR_PDID_T;  
//------------------------------------------------------
// This register tells what triggered the last reset
// Bit0 - POR reset
// Bit1 - PAD reset
// Bit2 - WDT reset
//

// Access via SYS->RSTSRC
typedef struct
{
    __IO uint32_t  RSTS_CORE:1;
    __I  uint32_t  RESERVE2:1;
//    __IO uint32_t  RSTS_POR:1;
//    __IO uint32_t  RSTS_PAD:1;
    __IO uint32_t  RSTS_WDG:1;
    __I  uint32_t  RESERVE1:2;
    __IO uint32_t  RSTS_SYS:1;
    __IO uint32_t  RSTS_PMU:1;
    __IO uint32_t  RSTS_CPU:1;    
    __I  uint32_t  RESERVE:24;
} GCR_RSTSRC_T;
// -----------------------------
// This register, when written 1, triggers reset 
// of the corresponding block
// Bit 0 - chip reset
// BIt 1 - core reset (resets the M0 core only)
// Bit 2 - resets all the PDMA
//

typedef struct
{
    __IO uint32_t  CHIP_RST:1;
    __IO uint32_t  CPU_RST:1;
    __IO uint32_t  PDMA_RST:1;
    __I  uint32_t  RESERVE:29;
} GCR_IPRSTC1_T;

typedef struct
{
    __I  uint32_t  RESERVE0:6;
    __IO uint32_t  TMR0_RST:1; //6
    __IO uint32_t  TMR1_RST:1; //7
    __IO uint32_t  I2C0_RST:1; //8
    __I  uint32_t  RESERVE1:3; //9-11
    __IO uint32_t  SPI0_RST:1; //12
    __IO uint32_t  DPWM_RST:1; //13
    __I  uint32_t  RESERVE2:2; //14-15
    __IO uint32_t  UART0_RST:1;//16
    __I  uint32_t  RESERVE3:1; //17
    __IO uint32_t  BIQ_RST:1;  //18
    __IO uint32_t  CRC_RST:1;  //19
    __IO uint32_t  PWM10_RST:1;//20
    __I  uint32_t  RESERVE4:1; //21
    __IO uint32_t  ACMP_RST:1; //22
    __I  uint32_t  RESERVE5:5; //23-27
    __IO uint32_t  ADC_RST:1;  //28
    __IO uint32_t  I2S_RST:1;  //29
    __IO uint32_t  ANA_RST:1;  //30
    __I  uint32_t  RESERVE6:1; //31
} GCR_IPRSTC2_T;


typedef struct
{
    __I  uint32_t RESERVE:16;
    __IO uint32_t SCHMITT:16;    
} GCR_GPA_INP_T;

typedef struct
{
    __IO uint32_t GPA0:2;
    __IO uint32_t GPA1:2;
    __IO uint32_t GPA2:2;
    __IO uint32_t GPA3:2;
    __IO uint32_t GPA4:2;
    __IO uint32_t GPA5:2;
    __IO uint32_t GPA6:2;
    __IO uint32_t GPA7:2;
    __IO uint32_t GPA8:2;
    __IO uint32_t GPA9:2;
    __IO uint32_t GPA10:2;
    __IO uint32_t GPA11:2;
    __IO uint32_t GPA12:2;
    __IO uint32_t GPA13:2;
    __IO uint32_t GPA14:2;
    __IO uint32_t GPA15:2;
} GCR_GPA_ALT_T;

typedef struct
{
    __IO uint32_t GPB0:2;
    __IO uint32_t GPB1:2;
    __IO uint32_t GPB2:2;
    __IO uint32_t GPB3:2;
    __IO uint32_t GPB4:2;
    __IO uint32_t GPB5:2;
    __IO uint32_t GPB6:2;
    __IO uint32_t GPB7:2;
    __I  uint32_t RESERVE:8;
} GCR_GPB_ALT_T;



typedef struct
{
    __I  uint32_t RESERVE:16;
    __IO uint32_t SCHMITT:16;    
} GCR_GPB_INP_T;


typedef struct
{
    __IO uint32_t  WAKE_DOUT:1;
    __IO uint32_t  WAKE_OENB:1;
    __IO uint32_t  WAKE_TRI:1;
    __I  uint32_t  WAKE_DIN:1;
    __I  uint32_t  RESERVE:28;
} GCR_WAKECR_T;

typedef __IO uint32_t GCR_REGLOCK_T;

typedef struct
{
    __IO uint16_t  TRIM:8;
    __IO uint16_t  RANGE:1;
    __I  uint16_t  RESERVE:7;
}GCR_OSCTRIM_T;

typedef struct
{
    __IO uint32_t  TRIM:9;
    __I  uint32_t  RESERVE1:7;
    __IO uint32_t  TM_REG:5;
    __I  uint32_t  RESERVE2:1;
    __IO uint32_t  IBGEN:2;
    __I  uint32_t  RESERVE3:7;
    __IO uint32_t  TRIM_CLK:1;
}GCR_OSC10K_T;


typedef struct
{
    __IO uint32_t  INTSRC:3;
    __I  uint32_t  RESERVE:29;
} GCR_INTSRC_T;

typedef struct
{
    __IO uint32_t  NMISEL:5;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  INT_TEST:1;
    __I  uint32_t  RESERVE1:24;
} GCR_NMISEL_T;


typedef __IO uint32_t GCR_MCUIRQ_T;

typedef struct
{
    GCR_PDID_T      PDID;   
    GCR_RSTSRC_T    RSTSRC;
    GCR_IPRSTC1_T   IPRSTC1;
    GCR_IPRSTC2_T   IPRSTC2;
    uint32_t        RESERVE1[8];
    GCR_GPA_INP_T   GPA_INP;
    GCR_GPB_INP_T   GPB_INP;
    GCR_GPA_ALT_T   GPA_ALT;
    GCR_GPB_ALT_T   GPB_ALT;
    uint32_t        RESERVE3[5];
    GCR_WAKECR_T    WAKECR;
    uint32_t        RESERVE4[42];
    GCR_REGLOCK_T   REGLOCK;
    uint32_t        RESERVE5[3];
    GCR_OSCTRIM_T   OSCTRIM[2];
    GCR_OSC10K_T    OSC10K;
}  GCR_T;


typedef struct
{
    GCR_INTSRC_T    INTSRC[32];
    GCR_NMISEL_T    NMISEL;
    GCR_MCUIRQ_T    MCUIRQ;   
} GCR_INT_T;

/*-------------------------- FLASH Memory Controller -------------------------*/
typedef struct
{
    __IO uint32_t  ISPEN:1;
    __IO uint32_t  BS:1;
    __I  uint32_t  RESERVE0:2;
    __IO uint32_t  CFGUEN:1;
    __IO uint32_t  LDUEN:1;
    __IO uint32_t  ISPFF:1;
    __IO uint32_t  SWRST:1;
    __IO uint32_t  PT:3;
    __I  uint32_t  RESERVE1:1;
    __IO uint32_t  ET:3;
    __I  uint32_t  RESERVE2:17;

} FMC_ISPCON_T;

typedef __IO uint32_t FMC_ISPADR_T;
typedef __IO uint32_t FMC_ISPDAT_T;
typedef __IO uint32_t FMC_ISPCMD_T;
typedef __IO uint32_t FMC_ISPTRG_T;
typedef __I uint32_t  FMC_DFBADR_T;

	
	  


typedef struct
{
    FMC_ISPCON_T    ISPCON;
    FMC_ISPADR_T    ISPADR;
    FMC_ISPDAT_T    ISPDAT;
    FMC_ISPCMD_T    ISPCMD;
    FMC_ISPTRG_T    ISPTRG;
    FMC_DFBADR_T    DFBADR;
	__I uint32_t	RESERVED[3];
	__IO uint32_t	TRIM_RD;
} FMC_T;




/*------------------------------ PDMA Controller -----------------------------*/
typedef struct
{
    __IO uint32_t  PDMACEN:1;
    __IO uint32_t  SW_RST:1;
    __IO uint32_t  MODE_SEL:2;
    __IO uint32_t  SAD_SEL:2;
    __IO uint32_t  DAD_SEL:2;
    __I  uint32_t  RESERVE0:4;
    __IO uint32_t  WRA_INT_SEL:4;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  APB_TWS:2;
    __I  uint32_t  RESERVE2:2;
    __IO uint32_t  TRIG_EN:1;
    __I  uint32_t  RESERVE3:8;
} PDMA_CSR_T;

typedef struct
{
    __IO uint32_t  CBCR:24;
    __I  uint32_t  RESERVE:8;
} PDMA_CBCR_T;

typedef struct
{
    __IO uint32_t  TABORT_IE:1;
    __IO uint32_t  BLKD_IE:1;
    __IO uint32_t  WAR_IE:1;
    __I  uint32_t  RESERVE:29;
} PDMA_IER_T;

typedef struct
{
    __IO uint32_t  TABORT_IF:1;
    __IO uint32_t  BLKD_IF:1;
    __I  uint32_t  RESERVE:6;
    __IO uint32_t  WAR_IF:4;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  BUSY:1;
    __I  uint32_t  RESERVE2:15;
    __IO uint32_t  INTR:1;
} PDMA_ISR_T;

typedef struct
{
    __IO uint32_t  PDMA_RST:1;
    __I  uint32_t  RESERVE0:7;
    __IO uint32_t  HCLK_EN:4;
    __I  uint32_t  RESERVE1:20;
} PDMA_GCRCSR_T;


typedef struct
{
    __IO uint32_t  SPI0_RXSEL:4;
    __IO uint32_t  SPI0_TXSEL:4;
    __IO uint32_t  ADC_RXSEL:4;
    __IO uint32_t  DPWM_TXSEL:4;
    __IO uint32_t  UART0_RXSEL:4;
    __IO uint32_t  UART0_TXSEL:4;
    __IO uint32_t  I2S_RXSEL:4;
    __IO uint32_t  I2S_TXSEL:4;
} PDMA_PDSSR0_T;
                                                       
typedef __IO uint32_t PDMA_GCRISR_T;


typedef struct
{
    PDMA_GCRCSR_T   GCR;
    PDMA_PDSSR0_T   PDSSR;
    __I  uint32_t   RESERVE;  
    PDMA_GCRISR_T   GCRISR;  
} PDMA_GCR_T;

typedef struct 
{
    PDMA_CSR_T      CSR;
    __IO uint32_t   SAR;
    __IO uint32_t   DAR;
    __IO uint32_t   BCR;
    __I  uint32_t   POINT;
    __I  uint32_t   CSAR;
    __I  uint32_t   CDAR;
    PDMA_CBCR_T     CBCR;
    PDMA_IER_T      IER;
    PDMA_ISR_T      ISR;
    __I  uint32_t   RESERVED0[54];
} PDMA_T;

	
typedef struct {
    PDMA_T          channel[4];
    __I  uint32_t   RESERVE[704];
    PDMA_GCRCSR_T   GCR;
    PDMA_PDSSR0_T   PDSSR;
    __I  uint32_t   RESERVED;
    PDMA_GCRISR_T   GCRISR;  
} PDMA_PERIPH_T;

/*----------------------------- PWM Controller -------------------------------*/
typedef struct
{
    __IO uint32_t  CP01:8;
    __I  uint32_t  RESERVE0:8;
    __IO uint32_t  DZI01:8;
    __I  uint32_t  RESERVE1:8;
} PWM_PPR_T;

typedef struct
{
    __IO uint32_t  CSR0:3;
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  CSR1:3;
    __I  uint32_t  RESERVE1:1;
    __I  uint32_t  RESERVE3:24;
} PWM_CSR_T;

typedef struct
{
    __IO uint32_t  CH0EN:1;
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  CH0INV:1;
    __IO uint32_t  CH0MOD:1;
    __IO uint32_t  DZEN01:1;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  CH1EN:1;
    __I  uint32_t  RESERVE2:1;
    __IO uint32_t  CH1INV:1;
    __IO uint32_t  CH1MOD:1;
    __I  uint32_t  RESERVE3:4;
    __I  uint32_t  RESERVE4:16;
} PWM_PCR_T;

typedef __IO uint32_t PWM_CNR_T;

typedef __IO uint32_t PWM_CMR_T;

typedef __I  uint32_t PWM_PDR_T;

typedef struct
{
    __IO uint32_t  PWMIE0:1;
    __IO uint32_t  PWMIE1:1;
    __I  uint32_t  RESERVE:30;
} PWM_PIER_T;

typedef struct
{
    __IO uint32_t  PWMIF0:1;
    __IO uint32_t  PWMIF1:1;
    __I  uint32_t  RESERVE:30;
} PWM_PIFR_T;

typedef struct
{
    __IO uint32_t  INV0:1;
    __IO uint32_t  CRL_IE0:1;
    __IO uint32_t  CFL_IE0:1;
    __IO uint32_t  CAPCH0EN:1;
    __IO uint32_t  CAPIF0:1;
    __I  uint32_t  RESERVE0:1;
    __IO uint32_t  CRLRI0:1;
    __IO uint32_t  CFLRI0:1;
    __I  uint32_t  RESERVE1:8;
    __IO uint32_t  INV1:1;
    __IO uint32_t  CRL_IE1:1;
    __IO uint32_t  CFL_IE1:1;
    __IO uint32_t  CAPCH1EN:1;
    __IO uint32_t  CAPIF1:1;
    __I  uint32_t  RESERVE2:1;
    __IO uint32_t  CRLRI1:1;
    __IO uint32_t  CFLRI1:1;
    __I  uint32_t  RESERVE3:8;
} PWM_CCR0_T;

typedef __IO uint32_t PWM_CRLR_T;

typedef __IO uint32_t PWM_CFLR_T;

typedef __IO uint32_t PWM_CAPENR_T;

typedef struct
{
    __IO uint32_t  PWM0:1;
    __IO uint32_t  PWM1:1;
    __I  uint32_t  RESERVE:30;
} PWM_POE_T;


typedef struct
{
    PWM_PPR_T       PPR;
    PWM_CSR_T       CSR;
    PWM_PCR_T       PCR;
    PWM_CNR_T       CNR0;
    PWM_CMR_T       CMR0;
    PWM_PDR_T       PDR0;
    PWM_CNR_T       CNR1;
    PWM_CMR_T       CMR1;
    PWM_PDR_T       PDR1;
    __I uint32_t    RESERVE0[7];
    PWM_PIER_T      PIER;
    PWM_PIFR_T      PIFR;
    __I uint32_t    RESERVE1[2];
    PWM_CCR0_T      CCR0;
    __I uint32_t    RESERVE2;
    PWM_CRLR_T      CRLR0;
    PWM_CFLR_T      CFLR0;
    PWM_CRLR_T      CRLR1;
    PWM_CFLR_T      CFLR1;
    __I uint32_t    RESERVE3[4];
    PWM_CAPENR_T    CAPENR;
    PWM_POE_T       POE;    
        
    
} PWM_T;

//---------------------------- Audio Digital PWM (DPWM) ------------------------
typedef struct
{
    __IO uint32_t Freq:3;
    __IO uint32_t Deadtime:1;
    __IO uint32_t Dither:2;
    __IO uint32_t Enable:1;
	__IO uint32_t EnGPIO:1;
    __I  uint32_t RESERVE:24;
} DPWM_CTRL_T;

typedef struct
{
    __I uint32_t Full:1;
    __I uint32_t Empty:1;
    __I uint32_t RESERVE:30;
}DPWM_STAT_T;
typedef struct
{
    __IO uint32_t EnablePDMA:1;
    __I  uint32_t RESERVE:31;
}DPWM_PDMA_T;
//---------------------------------------
//	DPWM->FIFO is writtable	only
//     It is a 16-deep fifo each 16bit wide
//  ZOH_DIV is the upsample ratio of the zeroth order hold
//     The consumption rate of the PWM_fifo is calculated by
//     Freq(HCLK) / (ZOH_DIV * 64), where 64 is the upsampling ratio of the CIC filter
// 
//----------------------------------------

typedef struct
{
    DPWM_CTRL_T CTRL;
    DPWM_STAT_T STAT;
    DPWM_PDMA_T DMA;
    __O uint32_t FIFO;
    __IO uint32_t ZOH_DIV; 
} DPWM_T;

/*----------------------------- I2S Controller -------------------------------*/

typedef struct
{
    __IO uint32_t  I2SEN:1;
    __IO uint32_t  TXEN:1;
    __IO uint32_t  RXEN:1;
    __IO uint32_t  MUTE:1;
    __IO uint32_t  WORDWIDTH:2;
    __IO uint32_t  MONO:1;
    __IO uint32_t  FORMAT:1;
    __IO uint32_t  SLAVE:1;
    __IO uint32_t  TXTH:3;
    __IO uint32_t  RXTH:3;
    __IO uint32_t  MCLKEN:1;
    __IO uint32_t  RCHZCEN:1;
    __IO uint32_t  LCHZCEN:1;
    __IO uint32_t  CLR_TXFIFO:1;
    __IO uint32_t  CLR_RXFIFO:1;
    __IO uint32_t  TXDMA:1;
    __IO uint32_t  RXDMA:1;	
    __I  uint32_t  RESERVE:10;
} I2S_CON_T;

typedef struct
{
    __IO uint32_t  MCLK_DIV:3;
    __I  uint32_t  RESERVE0:5;
    __IO uint32_t  BCLK_DIV:8;
    __I  uint32_t  RESERVE1:16;
} I2S_CLKDIV_T;

typedef struct
{
    __IO uint32_t  RXUDFIE:1;
    __IO uint32_t  RXOVFIE:1;
    __IO uint32_t  RXTHIE:1;
    __I  uint32_t  RESERVE0:5;
    __IO uint32_t  TXUDFIE:1;
    __IO uint32_t  TXOVFIE:1;
    __IO uint32_t  TXTHIE:1;
    __IO uint32_t  RZCIE:1;
    __IO uint32_t  LZCIE:1;
    __I  uint32_t  RESERVE1:19;
} I2S_IE_T;

typedef struct
{
    __I  uint32_t  I2SINT:1;
    __I  uint32_t  I2SRXINT:1;
    __I  uint32_t  I2STXINT:1;
    __I  uint32_t  RIGHT:1;
    __I  uint32_t  RESERVE0:4;
    __IO uint32_t  RXUDF:1;
    __IO uint32_t  RXOVF:1;
    __I  uint32_t  RXTHF:1;
    __I  uint32_t  RXFULL:1;
    __I  uint32_t  RXEMPTY:1;
    __I  uint32_t  RESERVE1:3;
    __IO uint32_t  TXUDF:1;
    __IO uint32_t  TXOVF:1;
    __I  uint32_t  TXTHF:1;
    __I  uint32_t  TXFULL:1;
    __I  uint32_t  TXEMPTY:1;
    __I  uint32_t  TXBUSY:1;
    __IO uint32_t  RZCF:1;
    __IO uint32_t  LZCF:1;	
    __I  uint32_t  RX_LEVEL:4;
    __I  uint32_t  TX_LEVEL:4;
} I2S_STATUS_T;

typedef __O uint32_t I2S_TXFIFO_T;
typedef __I uint32_t I2S_RXFIFO_T;

typedef struct
{
    I2S_CON_T        CON;
    I2S_CLKDIV_T     CLKDIV;
    I2S_IE_T      	 IE;
    I2S_STATUS_T     STATUS;
    I2S_TXFIFO_T     TXFIFO;
    I2S_RXFIFO_T     RXFIFO;
} I2S_T;

/*----------------------------- Analog Controller -------------------------------*/
typedef struct
{
    __IO uint32_t  PULLDOWN:1;
    __IO uint32_t  PDLORES:1;
    __IO uint32_t  PDHIRES:1;
    __I  uint32_t  RESERVE:29;
	
} ANA_VMID_T; 

typedef struct
{
    __IO uint32_t  FM_SEL:2;
    __I  uint32_t  FM_DONE:1;
    __I  uint32_t  RESERVE:13;
    __IO uint32_t  FM_CYCLE:8;
    __I  uint32_t  RESERVE1:7;
    __IO uint32_t  FM_GO:1;
} FREQ_MEAS_T; 

typedef struct
{
    __IO uint32_t  LOW_TIME:2;	     // [1:0] 00=1, 01=2, 10=8, 11=16
    __IO uint32_t  CYCLE_CNT:3;		 // [4:2] =2^CYCLE_CNT
    __IO uint32_t  REF_CLK_MODE:1;	 // [5]
    __I  uint32_t  RESERVE0:2;		 // [7:6]
    __IO uint32_t  CLK_DIV:8;		 // [15:8]
    __I  uint32_t  RESERVE:13;		 // [28:16]
    __IO uint32_t  RST_CNT:1;		 // [29]
    __IO uint32_t  INT_EN:1;		 // [30]
    __IO uint32_t  CNT_EN:1;		 // [31]
} CAPS_CTRL_T;

typedef struct
{
    __IO uint32_t  EN:8;
    __IO uint32_t  VAL:2;
    __I  uint32_t  RESERVE:22;

} ISRC_CTRL_T;

typedef struct
{
    __IO uint32_t  PD:1;
    __IO uint32_t  DISCH:1;
    __I  uint32_t  RESERVE:30;

} LDO_PD_T;

typedef struct
{
    __IO uint32_t  MUXN_SEL:8;
    __IO uint32_t  MUXP_SEL:4;
    __IO uint32_t  TEMP_SEL:1;
    __IO uint32_t  MIC_SEL:1;
    __IO uint32_t  EN:1;
    __I  uint32_t  RESERVE:17;
} ANA_AMUX_T;




typedef struct
{
    __I  uint8_t   OSCTRIM;
    __I  uint8_t   COARSE;
    __IO int8_t    SUPERFINE;
    __I  uint8_t   RESERVE;
} ANA_OSCTRIM_T;


typedef struct
{
    __IO uint32_t  REF_SEL:1;
    __IO uint32_t  PU_PGA:1;
    __IO uint32_t  PU_IPBOOST:1;
    __IO uint32_t  BOOSTGAIN:1;
    __I  uint32_t  RESERVE:28;
} ANA_PGAEN_T;

typedef struct
{
    __IO uint32_t  PU_ZCD:1;
    __IO uint32_t  PU_BUFPGA:1;
    __IO uint32_t  PU_BUFADC:1;
    __IO uint32_t  PU_IBGEN:1;
    __IO uint32_t  PU_MOD:1;
    __IO uint32_t  MUTE_PGA:1;
    __IO uint32_t  MUTE_IPBOOST:1;
    __I  uint32_t  RESERVE:25;
} ANA_SIGCTRL_T;

typedef struct
{
    __IO uint32_t  GAIN:6;
    __I  uint32_t  RESERVE0:2;
    __I  uint32_t  GAIN_READ:6;
    __I  uint32_t  RESERVE1:18;

} ANA_PGAGAIN_T;

typedef struct
{
    ANA_VMID_T      VMID;           // 00
    __I  uint32_t	RESERVE0;       //
    ISRC_CTRL_T		ISRC;           // 08             
    __IO uint32_t	RESERVE1[5];
    __IO uint32_t	LDOSET;         
    LDO_PD_T    	LDOPD;
    __IO uint32_t	MICBSEL;
    __IO uint32_t	MICBEN;
    __I  uint32_t	RESERVE2[2];
    __IO uint32_t	VSET;
    __IO uint32_t	TMPOWER;
    __IO uint32_t	TMANALOG;
    __I  uint32_t	RESERVE3[3];
    ANA_AMUX_T		AMUX;
	__IO uint32_t	AMUX_B1;
    __I  uint32_t	RESERVE4[2];
    ANA_PGAEN_T 	PGAEN;
    ANA_SIGCTRL_T	SIGCTRL;
    ANA_PGAGAIN_T	PGA_GAIN;
    __I  uint32_t	RESERVE5[5];
    __IO uint32_t	TMOSC;
    ANA_OSCTRIM_T	TRIM;
    __I  uint32_t	FINETRIM;
    CAPS_CTRL_T 	CAPS_CTRL;
    __I  uint32_t	CAPS_CNT;
    FREQ_MEAS_T   	FREQ_CTRL;
    __I  uint32_t	FREQ_CNT;
} ANA_T;

/*------------------------------- BOD Controller -------------------------------*/
typedef struct
{
    __IO uint32_t  BOD_LVL:3;
    __IO uint32_t  BOD_HYS:1;
    __I  uint32_t  RESERVE:28;
} BOD_SEL_T;

typedef struct
{
    __IO uint32_t  EN:2;
    __IO uint32_t  IE:1;
    __IO uint32_t  INT:1;
    __I  uint32_t  BOD_OUT:1;
    __I  uint32_t  RESERVE:27;
} BOD_EN_T;

typedef struct
{
    __IO uint32_t  LVL:4;
    __I  uint32_t  RESERVE:28;
} TALARM_SEL_T;

typedef struct
{
    __IO uint32_t  EN:1;
    __I  uint32_t  TALARM:1;
    __IO uint32_t  IE:1;
    __IO uint32_t  INT:1;
    __I  uint32_t  RESERVE:28;
} TALARM_EN_T;

typedef struct
{
    __IO uint32_t  OFF_DUR:16;
    __IO uint32_t  ON_DUR:4;
    __I  uint32_t  RESERVE:12;
} DET_TIMER_T;

typedef struct
{
    BOD_SEL_T     BOD_SEL;
    BOD_EN_T      BOD_EN;
    TALARM_SEL_T  TALARM_SEL;
    TALARM_EN_T   TALARM_EN;
    DET_TIMER_T   DET_TIMER;
} BOD_T;

/*----------------------------- BIQ Controller -------------------------------*/
typedef struct
{
    __IO uint32_t  EN:1;       //0
    __IO uint32_t  SELPWM:1;   //1
    __IO uint32_t  PRGCOEF:1;  //2
    __IO uint32_t  RSTn:1;     //3
    __IO uint32_t  UPSR:3;     //4:6
    __I  uint32_t  RESERVE2:9; //7:15
    __IO uint32_t  SR_DIV:13;  //16:28
    __I  uint32_t  RESERVE3:2; //29:30
	__IO uint32_t  AGC_BIQ_SELn:1; // 31
} BIQ_CTRL_T;

typedef struct
{
    __IO uint32_t  BIST:1;
    __I uint32_t   FAIL:1;
    __I uint32_t   DONE:1;
    __I  uint32_t  RESERVE1:28;
} BIQ_TST_T;

typedef struct
{
    __IO uint32_t  BIQ_COEFF[16]; //BIQ_BA + 0x00
    BIQ_CTRL_T     BIQ_CTRL;      //BIQ_BA + 0x40
    BIQ_TST_T      BIQ_TEST;      //BIQ_BA + 0x44 
} BIQ_T;

/*------------------------------- ALC (Automatic Level Control ----------------*/
typedef struct
{
    __IO uint32_t  NGTH:3;        // 2:0
    __IO uint32_t  NGEN:1;        // 3
    __IO uint32_t  ALCATK:4;      // 7:4
    __IO uint32_t  ALCDCY:4;      // 11:8
    __IO uint32_t  ALCMODE:1;     // 12
    __IO uint32_t  ALCLVL:4;      // 16:13
    __IO uint32_t  ALCHLD:4;      // 20:17
    __IO uint32_t  ALCZC:1;       // 21
    __IO uint32_t  ALCMIN:3;      // 24:22
    __IO uint32_t  ALCMAX:3;      // 27:25
    __IO uint32_t  ALCSEL:1;      // 28
    __IO uint32_t  ALCNGSEL:1;    // 29
    __IO uint32_t  ALCPKSEL:1;    // 30
    __IO uint32_t  ALCPKLIM:1;    // 31
} ALC_REG_T;

typedef struct
{
    __I uint32_t   FAST_DEC:1;    // 0
    __I uint32_t   NOISE:1;       // 1
    __I uint32_t   P2P:9;         // 11:3
    __I uint32_t   PEAK:9;        // 
    __I uint32_t   RESERVE:12;    // 
	
} ALC_STATUS_T;
typedef struct
{
    __IO uint32_t   INT:1;    // 0
    __I uint32_t   RESERVE:31; // 
	
} ALC_INT_T;
typedef struct
{
    __IO uint32_t   EN;    // 0
    __I uint32_t   RESERVE:31; // 
	
} ALC_INTEN_T;

typedef struct
{
    ALC_REG_T      ALC_CTRL;       //ALC_BA + 0x0
    ALC_STATUS_T   ALC_STATUS;    //ALC_BA + 0x4
    ALC_INT_T      ALC_INT;       //ALC_BA + 0x8
    ALC_INTEN_T    ALC_INTEN;     //ALC_BA + 0xC
} ALC_T;

/*----------------------------- CRC Controller -------------------------------*/
typedef struct
{
    __IO uint32_t  LEN:9;
    __I  uint32_t  RESERVE1:7;
    __IO uint32_t  LSB:1;
    __I  uint32_t  RESERVE2:15;
} CRC_EN_T;

typedef struct
{
    CRC_EN_T       CRC_EN;
    __IO uint32_t  CRC_IN;
    __I  uint32_t  CRC_OUT;
} CRC_T;

/*----------------------------- SBRAM Controller -------------------------------*/
typedef struct
{
    __IO uint32_t	D[64];
} SBRAM_T;


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup ISD91XX_MemoryMap ISD91XX Memory Mapping
    @{
*/
/* Peripheral and SRAM base address */
#define FLASH_BA          ((     uint32_t)0x00000000)
#define SRAM_BA           ((     uint32_t)0x20000000)
#define AHB_BA            ((     uint32_t)0x50000000)
#define APB1_BA           ((     uint32_t)0x40000000)

/* Peripheral memory map */

#define WDT_BA       	   (APB1_BA      + 0x04000)  
#define RTC_BA             (APB1_BA      + 0x08000)
#define TIMER0_BA          (APB1_BA      + 0x10000)  
#define TIMER1_BA          (APB1_BA      + 0x10020)  
#define I2C0_BA            (APB1_BA      + 0x20000)
#define SPI0_BA            (APB1_BA      + 0x30000)
#define PWMA_BA            (APB1_BA      + 0x40000)
#define UART0_BA           (APB1_BA      + 0x50000)
#define DPWM_BA            (APB1_BA      + 0x70000)
#define ANA_BA             (APB1_BA      + 0x80000)
#define BOD_BA             (APB1_BA      + 0x84000)
#define CRC_BA             (APB1_BA      + 0x90000)
#define I2S_BA             (APB1_BA      + 0xA0000)
#define BIQ_BA             (APB1_BA      + 0xB0000)
#define ALC_BA             (APB1_BA      + 0xB0048)
#define ACMP_BA            (APB1_BA      + 0xD0000)
#define ADC_BA             (APB1_BA      + 0xE0000)
#define SBRAM_BA           (APB1_BA      + 0xF0000)   

#define GCR_BA             (AHB_BA       + 0x00000)
#define CLK_BA             (AHB_BA       + 0x00200)
#define INT_BA             (AHB_BA       + 0x00300)
#define GPIO_MAP_BA        (AHB_BA       + 0x00400)
#define GPIO_BA            (AHB_BA       + 0x4000)
#define GPIOA_BA           (GPIO_BA               )
#define GPIOB_BA           (GPIO_BA      + 0x0040)
#define GPIO_DBNCECON_BA   (GPIO_BA      + 0x0180)

#define PDMA_BA            (AHB_BA       + 0x08000)
#define PDMA0_BA           (AHB_BA       + 0x08000)
#define PDMA1_BA           (AHB_BA       + 0x08100)
#define PDMA2_BA           (AHB_BA       + 0x08200)
#define PDMA3_BA           (AHB_BA       + 0x08300)
#define PDMA_GCR_BA        (AHB_BA       + 0x08F00)

#define FMC_BA             (AHB_BA       + 0x0C000)
/*@}*/ /* end of group ISD91XX_MemoryMap */

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
/** @addtogroup ISD91XX_PeripheralDecl ISD91XX Peripheral Declaration
    @{
*/
#define GPIOA               ((GPIO_T *) GPIOA_BA)
#define GPIOB               ((GPIO_T *) GPIOB_BA)
#define GPIO_DBNCECON       ((GPIO_DBNCECON_T *) GPIO_DBNCECON_BA)

#define UART0               ((UART_T *) UART0_BA)
#define DPWM                ((DPWM_T *) DPWM_BA)

#define TIMER0              ((TIMER_T *) TIMER0_BA)
#define TIMER1              ((TIMER_T *) TIMER1_BA)

#define WDT                 ((WDT_T *) WDT_BA)

#define SPI0                ((SPI_T *) SPI0_BA)

#define I2C0                ((I2C_T *) I2C0_BA)

#define I2S                 ((I2S_T *) I2S_BA)

#define RTC                 ((RTC_T *) RTC_BA)

#define SDADC               ((SDADC_T*) ADC_BA)

#define ACMP                ((ACMP_T *) ACMP_BA)
#define ANA					((ANA_T *) ANA_BA)
#define BOD                 ((BOD_T *) BOD_BA)
#define CRC                 ((CRC_T *) CRC_BA)
#define BIQ                 ((BIQ_T *) BIQ_BA)
#define ALC                 ((ALC_T *) ALC_BA)
#define SBRAM				((SBRAM_T *) SBRAM_BA)

#define SYSCLK              ((SYSCLK_T *)CLK_BA)
#define SYS                 ((GCR_T *) GCR_BA)

#define SYSINT              ((GCR_INT_T *) INT_BA)

#define FMC                 ((FMC_T *) FMC_BA)

#define PDMA0               ((PDMA_T *) PDMA0_BA)
#define PDMA1               ((PDMA_T *) PDMA1_BA)
#define PDMA2               ((PDMA_T *) PDMA2_BA)
#define PDMA3               ((PDMA_T *) PDMA3_BA)
#define PDMA_GCR            ((PDMA_GCR_T *) PDMA_GCR_BA)
#define PDMA                ((PDMA_PERIPH_T *) PDMA_BA)

#define PWMA                ((PWM_T *) PWMA_BA)
/*@}*/ /* end of group ISD91XX_PeripheralDecl */

/*@}*/ /* end of group ISD91XX_Definitions */


/******************************************************************************/
/*                         Useful Macros                                      */
/******************************************************************************/

/*  Send the Protected Register Unlock Sequence                               */
#define UNLOCKREG(x)        SYS->REGLOCK=0x59;SYS->REGLOCK=0x16;SYS->REGLOCK=0x88

/*  Lock Protected Registers                                                  */
#define LOCKREG(x)          SYS->REGLOCK=0x00;     

/*  Copy a Register                                                           */
#define REGCOPY(dest, src)  *((uint32_t *)&(dest)) = *((uint32_t *)&(src))
/*  Clear a Register                                                          */
#define CLEAR(dest) 		*((uint32_t *)&(dest)) = 0

//=============================================================================
typedef volatile unsigned char  vu8;
typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;
/******************************************************************************/
/*  Macros to access 8/16/32 bit memory locations                             */
/******************************************************************************/
#define M8(adr)  (*((vu8  *) (adr)))
#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))
#define outpw(port,value)	*((vu32 *)(port))=value
#define inpw(port)			(*((vu32 *)(port)))
#define outpb(port,value)	*((vu8 *)(port))=value
#define inpb(port)			(*((vu8 *)(port)))
#define outps(port,value)	*((vu16 *)(port))=value
#define inps(port)			(*((vu16 *)(port)))
/******************************************************************************/

#define E_SUCCESS 	0
#define NULL         0

#ifndef TRUE
#define TRUE	   	1
#endif
#ifndef FALSE
#define FALSE   	0
#endif

#define ENABLE     1
#define DISABLE    0

// Define one bit mask
#define BIT0	0x00000001
#define BIT1	0x00000002
#define BIT2	0x00000004
#define BIT3	0x00000008
#define BIT4	0x00000010
#define BIT5	0x00000020
#define BIT6	0x00000040
#define BIT7	0x00000080
#define BIT8	0x00000100
#define BIT9	0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000

#ifdef __cplusplus
}
#endif
#endif  // __ISD91XX_H__
