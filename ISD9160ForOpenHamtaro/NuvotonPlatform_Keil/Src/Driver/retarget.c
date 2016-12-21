/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
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
#define GPIO1		          ((GPIO_TypeDef *) ((uint32_t)0x60000800))
#define GPIO2		          ((GPIO_TypeDef *) ((uint32_t)0x60001000))


#define PRINT_BYTE 1
#define PRINT_STROBE (1 << 7)
#define PRINT_CHAR 0x7F

#ifdef VERILOG_VALIDATION
void Enable_GPIO_PRINT(void)
{
	GPIO0->DIR.BYTE[PRINT_BYTE] = 0xff;	
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Routine to write a char                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SendChar(int ch)
{
#ifdef VERILOG_VALIDATION
	GPIO0->DATA[PRINT_CHAR].BYTE[PRINT_BYTE] = ch; // Write whole char to 2nd byte
	GPIO0->DATA[PRINT_STROBE].BYTE[PRINT_BYTE] = PRINT_STROBE; // Write strobe
	GPIO0->DATA[PRINT_STROBE].BYTE[PRINT_BYTE] = 0; // Clear Strobe
#else
	while(UART0->FSR.TX_FULL == 1);
	UART0->DATA = ch;
    if(ch == '\n'){
        while(UART0->FSR.TX_FULL == 1);
        UART0->DATA = '\r';
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
  	while (UART0->FSR.RX_EMPTY == 1);
	return (UART0->DATA);
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

