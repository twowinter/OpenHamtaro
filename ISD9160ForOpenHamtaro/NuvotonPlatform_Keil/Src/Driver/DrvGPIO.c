/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "DrvGPIO.h"
//#include "SysInfra.h" 
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define PORT_OFFSET   0x40

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static void (*_pfGPABCallback)(uint32_t u32GpaStatus, uint32_t u32GpbStatus);
static void (*_pfEINT0Callback)(void);
static void (*_pfEINT1Callback)(void);              		 

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO A and GPIO B  ISR                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     GPAB_IRQHandler                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle GPAB interrupt event           		                                       */
/*---------------------------------------------------------------------------------------------------------*/
//#ifdef USE_DRIVER_GPIO_IRQ
void GPAB_IRQHandler(void)
{
	uint32_t u32GpaStatus, u32GpbStatus;
	
    /* Keep the interrupt source */
	u32GpaStatus = GPIOA->ISRC;// (0x5000_4000 + 8 << 2)[31:0] = 0x5000_4020[31:0]
	u32GpbStatus = GPIOB->ISRC;// (0x5000_4040 + 8 << 2)[31:0] = 0x5000_4060[31:0]

    /* Clear the interrupt */
    GPIOA->ISRC = u32GpaStatus;
    GPIOB->ISRC = u32GpbStatus;

    /* Call the callback function of GPIOAB interrupt */
    if(_pfGPABCallback)
        _pfGPABCallback(u32GpaStatus, u32GpbStatus);    

}
//	  #endif


/*---------------------------------------------------------------------------------------------------------*/
/* External INT0 & INT1  ISR                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     EINT0_IRQHandler                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle EINT0 interrupt event           		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void EINT0_IRQHandler(void)
{
    /* EINT0 = GPB0. Clear the interrupt */
  	GPIOB->ISRC  = 1UL << 0;
	if(_pfEINT0Callback)
        _pfEINT0Callback();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     EINT1_IRQHandler                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle EINT1 interrupt event           		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void EINT1_IRQHandler(void)
{
    /* EINT1 = GPB1. Clear the interrupt */
  	GPIOB->ISRC  = 1UL << 1;
	if(_pfEINT1Callback)
        _pfEINT1Callback();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetCallbackFunctions                                                              */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             pfGPABCallback  - [in] The callback function of GPA and GPB interrupts.                   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               To install GPIO interrrupt callback functions                                             */
/*---------------------------------------------------------------------------------------------------------*/
void DrvGPIO_SetIntCallback(
    GPIO_GPAB_CALLBACK pfGPABCallback
)
{
    _pfGPABCallback  = (void (*)(uint32_t, uint32_t))pfGPABCallback;
      
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_InitFunction                                                                      */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             function   -[in] Specified GPIO port. 													   */	
/*	             				  Select the different pin to output the specified function                */
/*																										   */
/*   GPIO 	    Power |	Alternate1 	Alt1_IO |	Alternate2 	Alt2_IO |	Alternate3 |Alt3_IO                */
/*  ------------------|---------------------|-----------------------|--------------|---------              */
/*   GPIOA0 	VCCF  |	SPI_MOSI0 	O 		|	MCLK_out 	O		|              |                       */
/*   GPIOA1 	VCCF  |	SPI_SCLK 	O 		|	I2C_SCL 	IO		|              |                       */
/*   GPIOA2 	VCCF  |	SPI_SSB0 	O 		|				        |              |                       */
/*   GPIOA3 	VCCF  |	SPI_MISO0 	I 		|	I2C_SDA 	IO		|              |                       */
/*   GPIOA4 	VCCF  |	I2S_FS 		IO 		|				        |              |                       */
/*   GPIOA5 	VCCF  |	I2S_BCLK 	IO 		|				        |              |                       */
/*   GPIOA6 	VCCF  |	I2S_SDI 	I 		|				        |              |                       */
/*   GPIOA7 	VCCF  |	I2S_SDO 	O 		|				        |              |                       */
/*   GPIOA8 	VCCD  |	UART_TX 	O 		|	I2S_FS		IO		|              |                       */
/*   GPIOA9 	VCCD  |	UART_RX 	I 		|	I2S_BCLK	IO		|              |                       */
/*   GPIOA10 	VCCD  |	I2C_SDA 	O 		|	I2S_SDI		I		|	UART_RTSn  |	O                  */
/*   GPIOA11 	VCCD  |	I2C_SCL 	O 		|	I2S_SDO		O		|	UART_CTSn  |	O                  */
/*   GPIOA12 	VCCD  |	PWM0 		O 		|	SPKP 		O 		|	I2S_FS 	   |	IO                 */
/*   GPIOA13 	VCCD  |	PWM1 		O 		|	SPKM 		O 		|	I2S_BCLK   |	IO                 */
/*   GPIOA14 	VCCD  |	TM0 		I 		|	SDCLK 		O 		|	SDCLK 	   |	O                  */
/*   GPIOA15 	VCCD  |	TM1 		I 		|	SDIN 		I 		|		       |	                   */
/*   GPIOB0 	VCCD  |	SPI_SSB1 	O		|	CMPIN0 		AIO		|	SPI_SSB0   |	O                  */
/*   GPIOB1 	VCCD  |	MCLK_out 	O 		|	CMPIN1 		AIO		|	SPI_SSB1   |	O                  */
/*   GPIOB2 	VCCD  |	I2C_SCL 	IO		|	CMPIN2 		AIO		|	SPI_SCLK   |	O                  */
/*   GPIOB3 	VCCD  |	I2C_SDA 	IO		|	CMPIN3 		AIO		|	SPI_MISO0  |	I                  */
/*   GPIOB4 	VCCD  |	PWM0B 		IO 		|	CMPIN4 		AIO		|	SPI_MOSI0  |	O                  */
/*   GPIOB5 	VCCD  |	PWM1B 		IO 		|	CMPIN5 		AIO		|	SPI_MISO1  |	I                  */
/*   GPIOB6 	VCCD  |	I2S_SDI 	I 		|	CMPIN6 		AIO		|	SPI_MOSI1  |	O                  */
/*   GPIOB7 	VCCD  |	I2S_SDO 	O 		|	CMPIN7 		AIO		|              |                       */
/*																										   */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                                 */
/*               E_DRVGPIO_ARGUMENT  Argument error                                                        */
/* Description:                                                                                            */
/*               To initialize the multi-function pin's of the specified function.                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_InitFunction(DRVGPIO_FUNC function)
{
  
	switch(function)
	{
		/*---------------------------------------------------------------------------------------------------------*/
		/* GPIO                                                                                                    */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_GPIO:
		{
		  	outpw(&SYS->GPA_ALT,0); //(0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[31:0]
			outpw(&SYS->GPB_ALT,0); //(0x5000_0000 + 15<<2)[31:0]= 0x5000_003C[31:0]
	
		  	
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* PWM                                                                                                     */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_PWM01:
		{
		  	SYS->GPA_ALT.GPA12 		=1;	 // PWM0
		  	SYS->GPA_ALT.GPA13 		=1;	 // PWM1

		}break;

		case FUNC_PWM01B:
		{
		  	SYS->GPB_ALT.GPB4 		=1;	 // PWM0B
		  	SYS->GPB_ALT.GPB5 		=1;	 // PWM1B

		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* I2C                                                                                                     */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_I2C0:
		{
		  	SYS->GPA_ALT.GPA10 	=1;	// SDA // SYS->GPAALT.GPA10: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[21:20]
			SYS->GPA_ALT.GPA11 	=1;	// SCL // SYS->GPAALT.GPA11: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[23:22]
		}break;		

		case FUNC_I2C1:
		{
		  	SYS->GPA_ALT.GPA1 	=2;	// SCL
			SYS->GPA_ALT.GPA3 	=2;	// SDA
		}break;		

		case FUNC_I2C2:
		{
		  	SYS->GPB_ALT.GPB2 	=1;	// SCL
			SYS->GPB_ALT.GPB3 	=1;	// SDA
		}break;		
	
		/*---------------------------------------------------------------------------------------------------------*/
		/* I2S                                                                                                     */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_I2S0:	
		{
			SYS->GPA_ALT.GPA4 	=1; // I2S_FS
			SYS->GPA_ALT.GPA5	=1;	// I2S_BCLK	
			SYS->GPA_ALT.GPA6 	=1; // I2S_SDI		 
			SYS->GPA_ALT.GPA7	=1;	// I2S_SDO	
		}break;

		case FUNC_I2S1:	
		{
			SYS->GPA_ALT.GPA8 	=2; // I2S_FS
			SYS->GPA_ALT.GPA9	=2;	// I2S_BCLK	
			SYS->GPA_ALT.GPA10 	=2; // I2S_SDI		 
			SYS->GPA_ALT.GPA11	=2;	// I2S_SDO	
		}break;	
	
		/*---------------------------------------------------------------------------------------------------------*/
		/* SPI                                                                                                     */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_SPI0:	
		{
			SYS->GPA_ALT.GPA0 	=1; // SPI_MOSI0
			SYS->GPA_ALT.GPA1	=1;	// SPI_SCLK	
			SYS->GPA_ALT.GPA2 	=1; // SPI_SSB0		 
			SYS->GPA_ALT.GPA3	=1;	// SPI_MISO0	
		}break;

		case FUNC_SPI1:	
		{
			SYS->GPB_ALT.GPB0 	=3; // SPI_SSB0		 
			SYS->GPB_ALT.GPB2	=3;	// SPI_SCLK	
			SYS->GPB_ALT.GPB3	=3;	// SPI_MISO0	
			SYS->GPB_ALT.GPB4 	=3; // SPI_MOSI0
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* ACMP0   ACMP1                                                                                           */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_ACMP0:	
		{
			SYS->GPB_ALT.GPB0 	=2;
			SYS->GPB_ALT.GPB1 	=2;
			SYS->GPB_ALT.GPB2 	=2;
			SYS->GPB_ALT.GPB3 	=2;
			SYS->GPB_ALT.GPB4 	=2;
			SYS->GPB_ALT.GPB5 	=2;
			SYS->GPB_ALT.GPB6 	=2;
			SYS->GPB_ALT.GPB7 	=2;
		}break;

		case FUNC_ACMP1:	
		{
			SYS->GPB_ALT.GPB6 	=2;
			SYS->GPB_ALT.GPB7 	=2;
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* UART                                                                                                    */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_UART0:
		{
			SYS->GPA_ALT.GPA8	=1;		// UART_TX: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[17:16] 
			SYS->GPA_ALT.GPA9 	=1;		// UART_RX: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[19:18]
		}break;
		
		case FUNC_UART0_FLOW:
		{
			SYS->GPA_ALT.GPA8	=1;		// UART_TX: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[17:16] 
			SYS->GPA_ALT.GPA9 	=1;		// UART_RX: (0x5000_0000 + 14<<2)[31:0]= 0x5000_0038[19:18]
			SYS->GPA_ALT.GPA10 	=3;		// UART_RTSn
			SYS->GPA_ALT.GPA11	=3;		// UART_CTSn
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* TIMER                                                                                                   */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_TMR0:
		{
			SYS->GPA_ALT.GPA14 		=1;	   // TM0
		}break;
	
		case FUNC_TMR1:
		{
			SYS->GPA_ALT.GPA15 		=1;	   // TM1
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* MCLK                                                                                                    */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_MCLK0:
		{
			SYS->GPA_ALT.GPA0 		=2;	   
		}break;

		case FUNC_MCLK1:
		{
			SYS->GPB_ALT.GPB1 		=1;	   
		}break;

  		/*---------------------------------------------------------------------------------------------------------*/
		/* DMIC                                                 				                                   */
		/*---------------------------------------------------------------------------------------------------------*/
	  	case FUNC_DMIC0:
		{
			SYS->GPA_ALT.GPA14 		=2;		//SDCLK
			SYS->GPA_ALT.GPA15 		=2;	    //SDIN
		}break;

	  	case FUNC_DMIC1:
		{
			SYS->GPA_ALT.GPA14 		=3;		//!SDCLK
			SYS->GPA_ALT.GPA15 		=2;	    //SDIN
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* SPK   									                                                               */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_SPK:
		{
			SYS->GPA_ALT.GPA12 		=2;		//SPKP
			SYS->GPA_ALT.GPA13		=2;		//SPKM
		}break;

		/*---------------------------------------------------------------------------------------------------------*/
		/* NONE: user will configure GPIO manually	                                                               */
		/*---------------------------------------------------------------------------------------------------------*/
		case FUNC_NONE:
		{
		}break;

		default:
			return E_DRVGPIO_ARGUMENT;
	}

	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* FUNCTION                                                                                                */
/*      DrvGPIO_Close		 		                                                                       */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*      To close the opened IO and reset its configurations                                                */
/*                                                                                                         */
/* INPUTS                                                                                                  */
/*      port                                                                                               */
/*          Specified GPIO port. It could be GPA, GPB					                                   */
/*      i32Bit                                                                                             */
/*          Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.                          */
/*                                                                                                         */
/* RETURN                                                                                                  */
/*      E_SUCCESS   		                                                                               */
/*      E_DRVGPIO_ARGUMENT  Wrong arguments                                                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_Close(DRVGPIO_PORT port,int32_t i32Bit)
{
    if((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }
    // &GPIOA->PMD: 0x5000_4000[31:0]
    outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, (inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) & (~(3 << (i32Bit*2)))));
	
	GPIO_DBNCECON->ICLK_ON = 0; // 0x5000_4180[5]

    return E_SUCCESS;
}	

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_Open                                                                              */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB                                */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/*	             mode    	-[in] Set the IO to be IO_INPUT , IO_OUTPUT ,IO_OPENDRAIN or IO_QUASI          */
/* Returns:                                                                                                */
/*               E_SUCCESS           Success                                                               */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               To configure the specified GPIO to use it.                                                */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_Open(DRVGPIO_PORT port,int32_t i32Bit,DRVGPIO_IO mode)
{
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    if((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    
    if(mode == IO_INPUT)
    {
        outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, (inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) & (~(3 << (i32Bit * 2)))));
    }
    else if(mode == IO_OUTPUT)
    {
		outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, (inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) & (~(3 << (i32Bit * 2)))));
        outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) | (1 << (i32Bit * 2)));
    }
    else if(mode == IO_OPENDRAIN)
    {
		outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, (inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) & (~(3 << (i32Bit * 2)))));
        outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) | (2 << (i32Bit * 2)));
    }
	else if(mode == IO_QUASI)
    {
		outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, (inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) & (~(3 << (i32Bit * 2)))));
        outpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET, inpw((uint32_t)&GPIOA->PMD + port * PORT_OFFSET) | (3 << (i32Bit * 2)));
    }
	else
    {
        return E_DRVGPIO_ARGUMENT;
    }
        
	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetPortBits                                                                       */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB                                */
/*	             i32Data  	-[in] The data to write to the specified IO port.                              */
/* Returns:                                                                                                */
/*               E_SUCCESS        			Success                                                        */
/*               E_DRVGPIO_ARGUMENT        	Wrong arguments                                                */
/* Description:                                                                                            */
/*               The function is used to set the output port value to the specified GPIO port.                       */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_SetPortBits(DRVGPIO_PORT port,int32_t i32Data)
{
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    if(port == GPA)
		GPIOA->DOUT =	i32Data; //(0x5000_0000 + 2<<2)[31:0]= 0x5000_0004[31:0]
	else if(port == GPB)
		GPIOB->DOUT =	i32Data; //(0x5000_0040 + 2<<2)[31:0]= 0x5000_0044[31:0]
	else
		return E_DRVGPIO_ARGUMENT;	

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetPortBits                                                                       */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB                                */
/* Returns:                                                                                                */
/*               The IO pin value of the specified IO port                                                 */
/*               E_DRVGPIO_ARGUMENT        	Wrong arguments                                                */
/* Description:                                                                                            */
/*               The function is used to get the data of the specified IO port.                            */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_GetPortBits(DRVGPIO_PORT port)
{
 
    uint32_t u32PortAddr;
    
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    u32PortAddr = (uint32_t)&GPIOA->PIN + port * PORT_OFFSET;
   	
    return inpw(u32PortAddr);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetBit                                                                            */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               The bit value of the specified IO bit                                                     */
/*               E_DRVGPIO_ARGUMENT        	Wrong arguments                                                */
/* Description:                                                                                            */
/*               The function is used to Get the value of the specified IO bit .                           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_GetBit(DRVGPIO_PORT port, int32_t i32Bit)
{
    uint32_t u32PortAddr;
    
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    if((i32Bit < 0) || (i32Bit > 16))
    {
        return 0;
    }

    u32PortAddr = (uint32_t)&GPIOA->PIN + port * PORT_OFFSET;
   	   

    return ((inpw(u32PortAddr) >> i32Bit) & 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetBit                                                                            */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB                                */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS           Success                                                               */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               The function is used to Set the specified IO bit to 1  .                                  */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_SetBit(DRVGPIO_PORT port,int32_t i32Bit)
{
  if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }

    if((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }

    if(port == GPA)
		GPIOA->DOUT |=	1 << i32Bit;
	else if(port == GPB)
		GPIOB->DOUT |=	1 << i32Bit;
	else
		return E_DRVGPIO_ARGUMENT;	

	return E_SUCCESS;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_ClrBit                                                                            */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS           Success                                                               */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               The function is used to  Clear the specified IO bit to 0                                  */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_ClrBit(DRVGPIO_PORT port,int32_t i32Bit)
{
    uint32_t u32PortAddr;
    
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    if((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }

	u32PortAddr = (uint32_t)&GPIOA->DOUT  + port * PORT_OFFSET;
   	
	outpw(u32PortAddr, inpw(u32PortAddr) & (~(1 << i32Bit)));
   
    return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetDoutBit                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB                                */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               The bit value of the specified IO bit                                                     */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               The function is used to get the value of the specified IO bit from GPIO Dout register.    */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_GetDoutBit(DRVGPIO_PORT port, int32_t i32Bit)
{
	if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    if((i32Bit < 0) || (i32Bit >= 15))
    {
        return E_DRVGPIO_ARGUMENT;
    }
 
    return ((inpw((uint32_t)&GPIOA->DOUT + port * PORT_OFFSET) >> i32Bit) & 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetPortDoutBits                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB						           */
/* Returns:                                                                                                */
/*               The value of the GPIO DOUT register value.                                                */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               The function is used to get the Dout register value of the specified IO port.    		   */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_GetPortDoutBits(DRVGPIO_PORT port)
{
    if(port > GPB)
    {
        return E_DRVGPIO_ARGUMENT;
    }

    return inpw((uint32_t)&GPIOA->DOUT + port *PORT_OFFSET);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetDebounceTime                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             u32DebounceClk    	-[in] The debounce timing is 2^(u32DebounceClk) * clockSource          */
/*	             clockSource    	-[in] The debounce clock source can be DBCLKSRC_HCLK or DBCLKSRC_10K   */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                                 */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               The function is used to To set the debounce timing and select source.     		           */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_SetDebounceTime(uint32_t u32DebounceClk,DRVGPIO_DBCLKSRC clockSource)
{

    /* Maximum debounce time is 2^(15) * clockSource */
    if(u32DebounceClk > 15)
    {
        u32DebounceClk = 15;
    }
    
	GPIO_DBNCECON->DBCLKSEL = u32DebounceClk ; // 0x5000_4180[3:0]

	GPIO_DBNCECON->DBCLKSRC = clockSource ; // 0x5000_4180[4]

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetDebounceTime                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               The debounce time setting.                                                                */
/* Description:                                                                                            */
/*               The function is used to To Get the debounce timing setting.            		           */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_GetDebounceTime(void)
{
    volatile int32_t i32Reg;

    i32Reg = 	GPIO_DBNCECON->DBCLKSEL;//0x5000_4180[3:0]
   
    return i32Reg;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_EnableDebounce                                                                    */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB						           */
/*	             u32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS   		 Success .                                                             */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments .                                                     */
/* Description:                                                                                            */
/*               To enable the debounce function of the specified GPIO bit.            	     	           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_EnableDebounce(DRVGPIO_PORT port,uint32_t u32Bit)
{
    if(u32Bit > 15)
        return E_DRVGPIO_ARGUMENT;

	// GPIOA->DBEN: (0x5000_4000 + 5 << 2)[31:0] = 0x5000_4014[31:0]
    outpw((uint32_t)&GPIOA->DBEN + port * PORT_OFFSET, inpw((uint32_t)&GPIOA->DBEN + port * PORT_OFFSET) | (1 << u32Bit));

	//GPIO_DBNCECON->ICLK_ON = 1;

    return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_DisableDebounce                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             i32Bit  	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS   		 Success .                                                             */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments .                                                     */
/* Description:                                                                                            */
/*               To disable the debounce function of the specified GPIO bit.            	     	       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_DisableDebounce(DRVGPIO_PORT port, uint32_t u32Bit)
{
    if(port > GPB)
        return E_DRVGPIO_ARGUMENT;

	// GPIOA->DBEN: (0x5000_4000 + 5 << 2)[31:0] = 0x5000_4014[31:0]
    outpw((uint32_t)&GPIOA->DBEN + port * PORT_OFFSET , inpw((uint32_t)&GPIOA->DBEN + port * PORT_OFFSET) &(~(1<<u32Bit)));

    
	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetPortMask                                                                       */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             u32Mask   	-[in] The mask data for the specified IO port.                                 */
/* Returns:                                                                                                */
/*               E_SUCCESS   		 Success .                                                             */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments .                                                     */
/* Description:                                                                                            */
/*               To set GPIO port mask register.                                        	     	       */
/*				 When set the port = '0xFFFF', the corresponding register of GPIOx_DOUT is protected and   */
/*                   the write data to the protect port is ignored.                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_SetPortMask(DRVGPIO_PORT port, uint32_t u32Mask)
{
    if(port > GPB)
		return E_DRVGPIO_ARGUMENT;	

	//&GPIOA->DMASK: (0x5000_4000 + 3<< 4)[31:0] = 0x5000_400C[31:0]
    outpw((uint32_t)&GPIOA->DMASK + port * PORT_OFFSET, u32Mask);

    return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_SetBitMask                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             i32Bit    	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS   		 Success .                                                             */
/* Description:                                                                                            */
/*               To set bits of GPIO port mask                                           	     	       */
/*				 When set the bit = '1', the corresponding registre of GPIOx_DOUT bit[n] is protected and  */
/*                   the write data to the protect bit is ignored.                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_SetBitMask(DRVGPIO_PORT port, int32_t i32Bit)
{

	outpw((uint32_t)&GPIOA->DMASK+ port*PORT_OFFSET , inpw((uint32_t)&GPIOA->DMASK+ port*PORT_OFFSET) | (1<<i32Bit));

	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_ClrBitMask                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/*	             i32Bit    	-[in] Specified bit of the IO port. It could be 0~15 in GPA and 0~7 in GPB.    */
/* Returns:                                                                                                */
/*               E_SUCCESS   		 Success .                                                             */
/* Description:                                                                                            */
/*               To clear bits of GPIO port mask	                                       	     	       */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_ClrBitMask(DRVGPIO_PORT port, uint32_t u32Bit)
{

	outpw((uint32_t)&GPIOA->DMASK+ port*PORT_OFFSET,inpw((uint32_t)&GPIOA->DMASK+ port*PORT_OFFSET) &(~(1<<u32Bit)));
	
	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetPortMask                                                                      */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    	-[in] Specified GPIO port. It could be GPA, GPB					               */
/* Returns:                                                                                                */
/*               the value of Specified GPIO port mask register .                                          */
/* Description:                                                                                            */
/*               To get current GPIO port mask setting	                                  	     	       */
/*---------------------------------------------------------------------------------------------------------*/

int32_t DrvGPIO_GetPortMask(DRVGPIO_PORT port)
{
	return inpw((uint32_t)&GPIOA->DMASK + port * PORT_OFFSET); 
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_EnableInt                                                                         */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    		   -[in] Specified GPIO port. It could be GPA, GPB					       */
/*	             u32Bit    	       -[in] Specified bit of the IO port. It could be 0~15 in GPA and 2~7 in GPB.*/
/*	             tiggerType		   -[in] Specified bit of the IO port. Specified INT type.                 */
/*                                       It could be IO_RISING, IO_FALLING or IO_BOTH_EDGE                 */
/*	             mode			   -[in] Specified INT mode.                                               */
/*                                       It could be MODE_EDGE, or MODE_LEVEL                              */
/* Returns:                                                                                                */
/*               E_SUCCESS           Success                                                               */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               To set one bit of GPIO port to Interrupt type	                                  	       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_EnableInt(DRVGPIO_PORT port, uint32_t u32Bit, DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode)
{
    if((port == GPB) && ((u32Bit == 0) || (u32Bit == 1)))
    {
        // GPB0 is EINT0, GPB1 is EINT1. Both of them can't be assigned to GPAB interrupt
        return E_DRVGPIO_ARGUMENT;    
    }

	if((port == GPA && u32Bit > 16 	) || 
	   (port == GPB && u32Bit > 8  	) ||
       (port  > GPB                 ) 
	  )
    {
        return E_DRVGPIO_ARGUMENT;
    }
    
    /* Configure the interrupt to be rising/falling when edge trigger or high/low level when level trigger */
	if(tiggerType ==IO_RISING)//'1': rising edge or high level
		outpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET , inpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET) | (1<<(u32Bit+16)));	 

	else if(tiggerType == IO_FALLING)//'1': falling edge or low level
		outpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET , inpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET) | (1<<u32Bit));	 

	else if(tiggerType == IO_BOTH_EDGE) //'1': (rising edge + falling edge) or [(high level + low level) = no action] 
		outpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET , inpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET) | (1<<u32Bit)| (1<<(u32Bit+16)));	 
	else
		return E_DRVGPIO_ARGUMENT;

    /* Configure to be level trigger or edge trigger: '1': level trigger; '0': edge trigger */
	if(mode ==MODE_EDGE)
		outpw((uint32_t)&GPIOA->IMD + port*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + port*PORT_OFFSET)&(~(1<<u32Bit)));

	else if(mode ==MODE_LEVEL)
		outpw((uint32_t)&GPIOA->IMD + port*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + port*PORT_OFFSET)|(1<<u32Bit));

	else
		return E_DRVGPIO_ARGUMENT;
    
    /* Enable the relative interrupt of M0 */
    if(port == GPA || port == GPB)
    {
        //NVIC_SetPriority (GPAB_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
        NVIC_EnableIRQ(GPAB_IRQn);
    }

   return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_DisableInt                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    		   -[in] Specified GPIO port. It could be GPA, GPB					       */
/*               u32Bit    	       -[in] Specified bit of the IO port. It could be 0~15 in GPA and 2~7 in GPB.*/
/* Returns:                                                                                                */
/*               E_SUCCESS           Success                                                               */
/*               E_DRVGPIO_ARGUMENT  Wrong arguments                                                       */
/* Description:                                                                                            */
/*               To disable the interrupt of specified GPIO bit.                                 	       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_DisableInt(DRVGPIO_PORT port,uint32_t u32Bit)
{
    uint32_t u32Reg;

    if((port == GPA && u32Bit > 15) || (port == GPB && u32Bit > 7))
        return E_DRVGPIO_ARGUMENT;
    
    /* Disable both rising/falling or both high level/low level interrupts */
    u32Reg = (1 << u32Bit) | (1 << (u32Bit + 16));
    outpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET , inpw((uint32_t)&GPIOA->IEN + port * PORT_OFFSET) & (~u32Reg));	 

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_EnableEINT0                                                                       */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             tiggerType		-[in] Specified trigger type. It could be IO_RISING, IO_FALLING, IO_BOTH_EDGE*/
/*               mode    	    -[in] Specified trigger mode. It could be MODE_EDGE, MODE_LEVEL.           */
/*				 pfEINT0Callback-[in] The callback function of EINT0 interrupts.    					   */
/* Returns:                                                                                                */
/*               None           			                                                               */
/*               									                                                       */
/* Description:                                                                                            */
/*               To enable interrupt 0; for GPIOB[0]						                                 	      	   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvGPIO_EnableEINT0(DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode, GPIO_EINT0_CALLBACK pfEINT0Callback)
{
    uint32_t u32Reg;

    /* Configure the interrupt to be rising/falling when edge trigger or high/low level when level trigger */
    u32Reg = inpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET);
	if(tiggerType ==IO_RISING)
    {
        u32Reg = u32Reg | (1 << (0 + 16)); 
	}
    else if(tiggerType == IO_FALLING)
	{
        u32Reg = u32Reg | (1 << 0);  
	}
    else if(tiggerType == IO_BOTH_EDGE)
    {  
        u32Reg = u32Reg | (1 << 0) | (1 << (0 + 16));    
	}
		outpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET , u32Reg);	 

    /* Configure to be level trigger or edge trigger: '1': level trigger; '0': edge trigger */
	if(mode ==MODE_EDGE)
		outpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET)&(~(1<<0)));

	else if(mode ==MODE_LEVEL)
		outpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET)|(1<<0));

    _pfEINT0Callback = pfEINT0Callback;
    NVIC_EnableIRQ(EINT0_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_DisableEINT0                                                                      */
/*                                                                                                         */
/* Parameter: 																							   */
/*               None																					   */
/* Returns:                                                                                                */
/*               None           			                                                               */
/*               									                                                       */
/* Description:                                                                                            */
/*               To disable interrupt 0; for GPIOB[0]					                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvGPIO_DisableEINT0(void)
{
    uint32_t u32Reg;
    u32Reg = inpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET);
    u32Reg = u32Reg & (~((1UL << 0) | (1UL << (0 + 16))));
    outpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET , u32Reg);	 
    NVIC_DisableIRQ(EINT0_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_EnableEINT1                                                                       */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             tiggerType		-[in] Specified trigger type. It could be IO_RISING, IO_FALLING, IO_BOTH_EDGE*/
/*               mode    	    -[in] Specified trigger mode. It could be MODE_EDGE, MODE_LEVEL.           */
/*				 pfEINT0Callback-[in] The callback function of EINT1 interrupts.    					   */
/* Returns:                                                                                                */
/*               None           			                                                               */
/*               									                                                       */
/* Description:                                                                                            */
/*               To enable interrupt 1; for GPIOB[1]						                                 	      	   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvGPIO_EnableEINT1(DRVGPIO_INT_TYPE tiggerType, DRVGPIO_INT_MODE mode, GPIO_EINT1_CALLBACK pfEINT1Callback)
{
    uint32_t u32Reg;

    u32Reg = inpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET);
	if(tiggerType ==IO_RISING)
    {
        u32Reg = u32Reg | (1UL << (1 + 16));    
	}
    else if(tiggerType == IO_FALLING)
	{
        u32Reg = u32Reg | (1UL << 1);  
	}
    else if(tiggerType == IO_BOTH_EDGE)
    {  
        u32Reg = u32Reg | (1UL << 1) | (1UL << (1 + 16)); 
	}
    outpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET , u32Reg);	 


    /* Configure to be level trigger or edge trigger: '1': level trigger; '0': edge trigger */
	if(mode ==MODE_EDGE)
		outpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET)&(~(1<<1)));

	else if(mode ==MODE_LEVEL)
		outpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET,inpw((uint32_t)&GPIOA->IMD + GPB*PORT_OFFSET)|(1<<1));

    
    _pfEINT1Callback = pfEINT1Callback;
    NVIC_EnableIRQ(EINT1_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_DisableEINT1                                                                      */
/*                                                                                                         */
/* Parameter: 																							   */
/*               None																					   */
/* Returns:                                                                                                */
/*               None           			                                                               */
/*               									                                                       */
/* Description:                                                                                            */
/*               To disable interrupt 1; for GPIOB[1]					                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvGPIO_DisableEINT1(void)
{
    uint32_t u32Reg;
    u32Reg = inpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET);
    u32Reg = u32Reg & (~((1UL << 1) | (1UL << (1 + 16))));
    outpw((uint32_t)&GPIOA->IEN + GPB * PORT_OFFSET , u32Reg);	 
    NVIC_DisableIRQ(EINT1_IRQn);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetIntStatus                                                                      */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             port    		   -[in] Specified GPIO port. It could be GPA, GPB                         */
/* Returns:                                                                                                */
/*               A value of the GPIO interrupt status register                                             */
/* Description:                                                                                            */
/*               This function is used to return a value to the GPIO interrupt status register.    	       */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvGPIO_GetIntStatus(DRVGPIO_PORT port)
{
    uint32_t u32Reg;
	
	if(port == GPA)
		u32Reg = GPIOA->ISRC;
	else if(port == GPB)
		u32Reg = GPIOB->ISRC;
	else
		return E_DRVGPIO_ARGUMENT;
	return u32Reg;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvGPIO_GetVersion                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             None                                                                                      */
/* Returns:                                                                                                */
/*               The DrvGPIO version number                                                                */
/* Description:                                                                                            */
/*               This function is used to return the version number of GPIO driver.    	                   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvGPIO_GetVersion (void)
{
   return DRVGPIO_VERSION_NUM;
}


