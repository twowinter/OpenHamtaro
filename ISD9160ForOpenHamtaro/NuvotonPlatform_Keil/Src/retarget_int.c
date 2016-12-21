/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <coocox.h>
#include "CoOS.h"
#include "Driver\DrvUART.h"
#include "Driver\DrvGPIO.h"
#include "ISD9xx.h"

#if defined ( __CC_ARM   )
#if (__ARMCC_VERSION < 400000)
#else
/* Insist on keeping widthprec, to avoid X propagation by benign code in C-lib */
#pragma import _printf_widthprec
#endif
#endif									

/*---------------------------------------------------------------------------------------------------------*/
/* Macro Definition                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
/* Using UART0 or UART1 */  
#define DEBUG_PORT   0 		    /*0:UART0  1:UART1 2:UART2 */

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef SEMIHOST
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

typedef union
{
    __IO uint32_t WORD;  
    __IO uint16_t HALFWORD[2];
    __IO uint8_t  BYTE[4];
} GPIO_Data_TypeDef;

typedef struct
{
    GPIO_Data_TypeDef DATA [256];
    GPIO_Data_TypeDef DIR;
    uint32_t RESERVED[3];
    GPIO_Data_TypeDef IE;
} GPIO_TypeDef;


#define GPIO0		          ((GPIO_TypeDef *) ((uint32_t)0x60000000))


#define PRINT_BYTE 1
#define PRINT_STROBE (1 << 7)
#define PRINT_CHAR 0x7F

#define RXBUFSIZE 32
#define TXBUFSIZE 32

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t comRxbuf[RXBUFSIZE];
volatile uint8_t comRxbytes = 0;		/* Available receiving bytes */
volatile uint8_t comRxhead 	= 0;
volatile uint8_t comRxtail 	= 0;

volatile uint8_t comTxbuf[TXBUFSIZE];
volatile uint8_t comTxbytes = 0;		/* Available receiving bytes */
volatile uint8_t comTxhead 	= 0;
volatile uint8_t comTxtail 	= 0;

OS_FlagID UARTRxFlag=0;
OS_FlagID UARTTxFlag=0;

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                           	   */
/*---------------------------------------------------------------------------------------------------------*/
void UART_INT_HANDLE(uint32_t u32IntStatus)
{

 	uint8_t bInChar[1]={0xFF};
	CoEnterISR();
	if(u32IntStatus & RDAIE){
		/* Get all the input characters */
		while(UART0->ISR.RDA_IF==1){
			/* Get the character from UART Buffer */
			DrvUART_Read(UART_PORT0,bInChar,1);
			/* Check if buffer full */
			if(comRxbytes < RXBUFSIZE){
				/* Enqueue the character */
				comRxbuf[comRxtail] = bInChar[0];
				comRxtail = (comRxtail == (RXBUFSIZE-1)) ? 0 : (comRxtail+1);
				Inc8(&comRxbytes);
                isr_SetFlag(UARTRxFlag);
			}			
		}
	}
	else if(u32IntStatus & THREIE)
	{   
        uint16_t tmp;
    txch:
        tmp = comTxbytes;
		if(0x00 != tmp)
		{
			if(UART0->FSR.TX_FULL != 1){
			UART0->DATA = comTxbuf[comTxhead];
			comTxhead = (comTxhead == (RXBUFSIZE-1)) ? 0 : (comTxhead+1);
			Dec8(&comTxbytes);
			goto txch;
			}
            
		}else{
            // no more data to Tx, disable int
            UART0->IER.THRE_IEN	= FALSE;
            isr_SetFlag(UARTTxFlag);
        }  
	}
    CoExitISR();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Routine to write a char                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef VERILOG_VALIDATION
void Enable_GPIO_PRINT(void)
{
	GPIO0->DIR.BYTE[PRINT_BYTE] = 0xff;	
}
#endif

void SendChar(int ch)
{
    uint8_t tmp;
#ifdef VERILOG_VALIDATION
	GPIO0->DATA[PRINT_CHAR].BYTE[PRINT_BYTE] = ch; // Write whole char to 2nd byte
	GPIO0->DATA[PRINT_STROBE].BYTE[PRINT_BYTE] = PRINT_STROBE; // Write strobe
	GPIO0->DATA[PRINT_STROBE].BYTE[PRINT_BYTE] = 0; // Clear Strobe
#else
 txch:
    tmp = comTxbytes;
	/* Check if buffer full */
    if(comTxbytes < TXBUFSIZE){
        /* Enqueue the character */
        comTxbuf[comTxtail] = ch;
        comTxtail = (comTxtail == (TXBUFSIZE-1)) ? 0 : (comTxtail+1);
        // Needs to be atomic
        Inc8 (&comTxbytes);
		// Enable int
        UART0->IER.THRE_IEN	= TRUE;
    }else{
        // Wait for flag
		if(UARTTxFlag)
        	CoWaitForSingleFlag(UARTTxFlag,0);
        goto txch;
    }
    if(ch == '\n'){
        ch = '\r';
        goto txch;
    }
#endif

}

/*---------------------------------------------------------------------------------------------------------*/
/* Routine to get a char                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
char GetChar()
{
#ifdef VERILOG_VALIDATION
	return 0;
#else
    uint8_t tmp, ch;
 rxch:
    tmp = comRxbytes;
    if(0x00 != tmp){
        ch = comRxbuf[comRxhead];
        comRxhead = (comRxhead == (RXBUFSIZE-1)) ? 0 : (comRxhead+1);
        Dec8(&comRxbytes);
    }else{
        // wait for flag
        CoWaitForSingleFlag(UARTRxFlag,0);
        goto rxch;
    }
    return (ch);
#endif

}


/*---------------------------------------------------------------------------------------------------------*/
/* C library retargetting                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

void _ttywrch(int ch)
{
    SendChar(ch);
    return;
}

int fputc(int ch, FILE *f)
{
    SendChar(ch);
    return 0;
}

int fgetc(FILE *f) {
    return (GetChar());
}


int ferror(FILE *f) {
    return EOF;
}

//----------------------------------
// Specific to this test program
//----------------------------------
/* target-specific gotachar():
 * Return 0 if no char is avaialable at UART rcv fifo; else 1.
 * Do NOT pull character out of fifo, just return status. 
 */
int
gotachar(void)
{
	return (UART0->FSR.RX_EMPTY==0);

}
#else

int
gotachar(void)
{
	return(1);
}
#endif
