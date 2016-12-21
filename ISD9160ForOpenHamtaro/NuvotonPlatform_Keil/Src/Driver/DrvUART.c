/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"

#include "DrvUART.h"
#include "DrvSYS.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_DRVUART_CALLBACK *g_pfnUART0callback = NULL;


/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler                                                                                 	   */
/*---------------------------------------------------------------------------------------------------------*/
void UART02_IRQHandler(void)
{
    uint32_t u32uart0IntStatus;

    u32uart0IntStatus = inpw(&UART0->ISR) ;

    if(g_pfnUART0callback != NULL)
	{
        g_pfnUART0callback(u32uart0IntStatus);
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_BaudRateCalculator                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */
/*	             i32clk          	-[in] Uart Source Clock; unit: Hz                                      */
/*	             i32baudRate     	-[in] User seting BaudRate; unit: Bits per second                      */
/*                                        computer: 110; 300; 1200; 2400; 4800; 9600; 19200;               */ 
/*                                                  38400; 57600; 115200; 230400; 460800; 921600           */
/*	             UART_BAUD_T *baud  -[in] Get User Settings                                                */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to get  compute Baud Setting Value                                   */
/*               	mode: 			0x4005_0024[29:28]  (baud->DIVX_EN; baud->DIVX_ONE)					   */	
/*                  divider(div):   0x4005_0024[27:24]  (baud->DIVX)									   */
/*					BRD(tmp):		0x4005_0024[15:0]	(baud->BRD)       								   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_BaudRateCalculator(uint32_t i32clk, uint32_t i32baudRate, UART_BAUD_T *baud)
{
  	int32_t i32tmp;
	int32_t i32div;

	if(((i32clk / i32baudRate)%16)<3)	      /* Source Clock mod 16 <3 => Using Divider X =16 (MODE#0) */
	{
		baud->DIVX_EN = 0;
	    baud->DIVX_ONE   = 0;
		i32tmp = i32clk / i32baudRate/16  -2;
	}
	else							  /* Source Clock mod 16 >3 => Up 5% Error BaudRate */
	{
	    baud->DIVX_EN = 1;			  /* Try to Set Divider X = 1 (MODE#2)*/
	    baud->DIVX_ONE   = 1;
		i32tmp = i32clk / i32baudRate  -2;

		if(i32tmp > 0xFFFF)			  /* If Divider > Range  */
		{
			baud->DIVX_ONE = 0;		  /* Try to Set Divider X up 10 (MODE#1) */

			for(i32div = 8; i32div <16;i32div++)
			{
				if(((i32clk / i32baudRate)%(i32div+1))<3)
				{
					baud->DIVX   = i32div;
					i32tmp = i32clk / i32baudRate / (i32div+1) -2;
					break;
				}
			}
		}
	}

	baud->BRD = i32tmp;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetUartCLk                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */
/*	             None										                                               */
/* Returns:                                                                                                */
/*               Current Uart Clock                                                                        */
/* Description:                                                                                            */
/*               The function is used to get Uart clock                                                    */
/*---------------------------------------------------------------------------------------------------------*/
//static uint32_t DrvUART_GetUartCLk(void)
uint32_t DrvUART_GetUartCLk(void)
{
	uint32_t u32clk =0;
													/* Check UART Clock Source Setting */
//	u32clk = 49152000;								    /* Clock 49Mhz  */
	u32clk = DrvSYS_GetHCLK() * 1000 / (SYSCLK->CLKDIV.UART_N + 1) ;					
	return u32clk;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetRTS				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */
/*				 u16Port 			-[in]   UART Channel:  UART_PORT0				                       */
/*               uint8_t	   	-[in]   RTS Value 					          	         				   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to set RTS information	                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetRTS(UART_PORT u16Port,uint8_t u8Value)
{

	if(u16Port == UART_PORT0)
		UART0->MCR.RTS_SET = u8Value;
	else
		return;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Open                                                                        	   */
/*                                                                                                         */
/* Parameter:        																					   */
/*				 u16Port 	 -[in] UART Channel:  UART_PORT0						               		   */
/*               sParam  -[in] the struct parameter to configure UART                                      */
/*                         include of                                                                      */
/*                           * u32BaudRate - Baud rate 													   */
/*                           * u8cParity   - DRVUART_PARITY_NONE / DRVUART_PARITY_EVEN / DRVUART_PARITY_ODD*/
/*                           * u8cDataBits - DRVUART_DATA_BITS_5 / DRVUART_DATA_BITS_6 					   */
/*									  	 	 DRVUART_DATA_BITS_7 / DRVUART_DATA_BITS_8        			   */
/*                           * u8cStopBits - DRVUART_STOPBITS_1 / STOPBITS_1_5 / STOPBITS_2   			   */
/*                           * u8cRxTriggerLevel   - LEVEL_1_BYTE to LEVEL_62_BYTES                        */
/*                           * u8TimeOut - Time out value	                                     		   */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_DRVUART_ERR_PORT_INVALID         												       */
/*               E_DRVUART_ERR_PARITY_INVALID                    										   */
/*               E_DRVUART_ERR_DATA_BITS_INVALID               											   */
/*               E_DRVUART_ERR_STOP_BITS_INVALID                										   */
/*               E_DRVUART_ERR_TRIGGERLEVEL_INVALID      												   */
/*               E_DRVUART_ERR_ALLOC_MEMORY_FAIL                 										   */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to initialize UART                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Open(UART_PORT u16Port, STR_UART_T *sParam)
{

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check UART port                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if (u16Port != UART_PORT0) 
    {

		return E_DRVUART_ERR_PORT_INVALID;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied parity                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((sParam->u8cParity != DRVUART_PARITY_NONE) &&
        (sParam->u8cParity != DRVUART_PARITY_EVEN) &&
        (sParam->u8cParity != DRVUART_PARITY_ODD)  &&
        (sParam->u8cParity != DRVUART_PARITY_MARK) &&
        (sParam->u8cParity != DRVUART_PARITY_SPACE))
    {
 		return E_DRVUART_ERR_PARITY_INVALID;
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied number of data bits                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cDataBits != DRVUART_DATABITS_5) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_6) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_7) &&
             (sParam->u8cDataBits != DRVUART_DATABITS_8))
    {
 		return E_DRVUART_ERR_DATA_BITS_INVALID;
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied number of stop bits                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cStopBits != DRVUART_STOPBITS_1) &&
             (sParam->u8cStopBits != DRVUART_STOPBITS_2) &&
             (sParam->u8cStopBits != DRVUART_STOPBITS_1_5)
             )
    {
      	return E_DRVUART_ERR_STOP_BITS_INVALID;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Check the supplied nember of trigger level bytes                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    else if ((sParam->u8cRxTriggerLevel != DRVUART_FIFO_1BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_4BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_8BYTES) &&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_14BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_30BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_46BYTES)&&
             (sParam->u8cRxTriggerLevel != DRVUART_FIFO_62BYTES))
    {
		return E_DRVUART_ERR_TRIGGERLEVEL_INVALID;
    }

	if(u16Port == UART_PORT0)
	{
		/* Reset IP */
		SYS->IPRSTC2.UART0_RST = 1;
		SYS->IPRSTC2.UART0_RST = 0;

		/* Enable UART clock */
	    SYSCLK->APBCLK.UART0_EN = 1;


	}

	/* Tx FIFO Reset & Rx FIFO Reset & FIFO Mode Enable */
	if(u16Port == UART_PORT0)
	{
	  	UART0->FCR.TFR =1;
	  	UART0->FCR.RFR =1;
	}

	if (u16Port == UART_PORT0)
	{
		/* Set Rx Trigger Level */
		UART0->FCR.RFITL = sParam->u8cRxTriggerLevel;

		/* Set Parity & Data bits & Stop bits */
		UART0->LCR.SPE	=((sParam->u8cParity)&0x4)?1:0;
		UART0->LCR.EPE	=((sParam->u8cParity)&0x2)?1:0;
		UART0->LCR.PBE	=((sParam->u8cParity)&0x1)?1:0;

		UART0->LCR.WLS	=sParam->u8cDataBits;
		UART0->LCR.NSB	=sParam->u8cStopBits;

		/* Set Time-Out */
		UART0->TOR.TOIC 		=sParam->u8TimeOut;

		/* Set BaudRate */
		DrvUART_BaudRateCalculator(DrvUART_GetUartCLk(), sParam->u32BaudRate, &UART0->BAUD);
	}

	return E_SUCCESS;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DrvUART_Close                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*				 	u16Port 	 -[in] UART Channel:  UART_PORT0 			                         		   */
/* RETURN                                                                                                  */
/*                  None                                                                                   */
/* Description:                                                                                            */
/*                  The function is used disable UART clock, disable ISR and close UART function.  		   */
/*---------------------------------------------------------------------------------------------------------*/

void DrvUART_Close(UART_PORT u16Port)
{
	if(u16Port == UART_PORT0)
	{
		while(!UART0->FSR.TX_EMPTY);
		SYSCLK->APBCLK.UART0_EN = 0;
		g_pfnUART0callback = NULL;
        NVIC_DisableIRQ(UART0_IRQn);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_EnableInt                                                                     	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port 			  -[in] UART Channel:  UART_PORT0			                           */
/*               u32InterruptFlag -[in]	DRVUART_LININT/DRVUART_WAKEUPINT/DRVUART_BUFERRINT/DRVUART_RLSNT   */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/*               pfncallback      -[in] A function pointer for callback function                           */
/* Returns:                                                                                                */
/*               E_DRVUART_ARGUMENT                                                                        */
/*				 E_SUCCESS																									   */	
/*																										   */	
/* Side effects:                                                                                           */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to enable UART Interrupt and Install the call back function          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_EnableInt(
	UART_PORT   u16Port,
	uint32_t  u32InterruptFlag,
	PFN_DRVUART_CALLBACK pfncallback
)
{
    if((u16Port != UART_PORT0) )
    {
        return E_DRVUART_ARGUMENT;
    }

 	if(u16Port == UART_PORT0)											   	/* Set Specified Interrupt */
	{
		UART0->IER.RDA_IEN		=(u32InterruptFlag & DRVUART_RDAINT)?1:0 ;
		UART0->IER.THRE_IEN		=(u32InterruptFlag & DRVUART_THREINT)?1:0;
		UART0->IER.RLS_IEN		=(u32InterruptFlag & DRVUART_RLSNT)?1:0;
		UART0->IER.MS_IEN		=(u32InterruptFlag & DRVUART_MOSINT)?1:0;

		UART0->IER.TOC_EN		=(u32InterruptFlag & DRVUART_TOUTINT)?1:0;   	/* Time-out count enable */
		UART0->IER.RTO_IEN		=(u32InterruptFlag & DRVUART_TOUTINT)?1:0;		/* Time-out INT enable */

		UART0->IER.BUF_ERR_IEN	=(u32InterruptFlag & DRVUART_BUFERRINT)?1:0;
		UART0->IER.LIN_RX_BRK_IEN=(u32InterruptFlag & DRVUART_LININT)?1:0;

	}
    if(u16Port == UART_PORT0)											   	/* Install Callback function */
    {
		g_pfnUART0callback = pfncallback;
		NVIC_EnableIRQ(UART0_IRQn);
    }

	return E_SUCCESS;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_IsIntEnabled                                                                 	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Por		   	  -[in] UART Channel:  UART_PORT0 / UART_PORT0                         	   */
/*               u32InterruptFlag -[in]	DRVUART_LININT/DRVUART_WAKEUPINT/DRVUART_BUFERRINT/DRVUART_RLSNT   */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/* Returns:                                                                                                */
/*               Specified Interrupt Flag Set or clear                                                     */
/* Description:                                                                                            */
/*               The function is used to get the interrupt enable status								   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvUART_IsIntEnabled(UART_PORT	u16Port,uint32_t u32InterruptFlag)
{
		/* Read IER Register and check specified flag is enable */
		return ((inpw(&UART0->IER) &
		 (u32InterruptFlag & (DRVUART_LININT | DRVUART_WAKEUPINT | DRVUART_BUFERRINT |
		 		DRVUART_TOUTINT | DRVUART_MOSINT | DRVUART_RLSNT | DRVUART_THREINT | DRVUART_RDAINT))))?1:0;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_DisableInt                                                                   	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Por		   	  -[in] UART Channel:  UART_PORT0							               */
/*               u32InterruptFlag -[in]	DRVUART_LININT/DRVUART_WAKEUPINT/DRVUART_BUFERRINT/DRVUART_RLSNT   */
/*										DRVUART_MOSINT/DRVUART_THREINT/DRVUART_RDAINT/DRVUART_TOUTINT      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to disable UART Interrupt and uninstall the call back function       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_DisableInt(UART_PORT u16Port,uint32_t u32InterruptFlag)
{
	if(u16Port == UART_PORT0)
	{
		outpw(&UART0->IER + u16Port,inpw(&UART0->IER + u16Port) &~u32InterruptFlag);	   /* Disable INT  */
	}

	if(u32InterruptFlag & DRVUART_TOUTINT)			  							 /* Disable Counter Enable */
	{
		if(u16Port == DRVUART_PORT0)
			UART0->IER.RTO_IEN = 0;
	}

	switch (u16Port)									   			 /* Disable Callback function and NVIC */
    {
        case UART_PORT0:
			g_pfnUART0callback = NULL;
            NVIC_DisableIRQ(UART0_IRQn);
            break;
        default:
            break;
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_ClearInt                                                                     	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   		  -[in]   UART Channel:  UART_PORT0							           */
/*               u32InterruptFlag     -[in]   DRVUART_MOSINT/DRVUART_RLSNT/DRVUART_THREINT 		  		   */
/*											  DRVUART_RDAINT/DRVUART_TOUTINT                               */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to clear UART Interrupt								        	   */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvUART_ClearInt(UART_PORT	u16Port,uint32_t u32InterruptFlag)
{
	if((u32InterruptFlag & DRVUART_RDAINT) == DRVUART_RDAINT)    			/* clear Rx read Interrupt */
		inpw(&UART0->DATA + u16Port);

	if((u32InterruptFlag & DRVUART_RLSNT) == DRVUART_RLSNT)					/* clear Receive Line Status Interrupt */
		outpw(&UART0->FSR + u16Port , inpw(&UART0->FSR + u16Port) | 0x70);

	if((u32InterruptFlag & DRVUART_MOSINT) == DRVUART_MOSINT)				/* clear Modem Interrupt */
		outpw(&UART0->MSR + u16Port,inpw(&UART0->MSR + u16Port) | 0x1);

	if((u32InterruptFlag & DRVUART_TOUTINT) == DRVUART_TOUTINT)				/* clear Time-out Interrupt */
	{
		inpw(&UART0->DATA + u16Port);
	}

	return E_SUCCESS;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetIntStatus                                                                 	   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 			                       */
/*               u32InterruptFlag     -[in]   DRVUART_MOSINT / DRVUART_RLSNT / DRVUART_THREINT 		   	   */
/*											  DRVUART_RDAINT/DRVUART_TOUTINT                               */
/* Returns:                                                                                                */
/*               0: None                                                                                   */
/*               1: Interrupt occurs                                                                       */
/* Description:                                                                                            */
/*               The function is used to get the interrupt  status										   */
/*---------------------------------------------------------------------------------------------------------*/
int8_t DrvUART_GetIntStatus(UART_PORT u16Port,uint32_t u32InterruptFlag)
{

	switch(u32InterruptFlag)
	{
		case DRVUART_MOSINT:				  	/* MODEM Status Interrupt */
				return (inpw(&UART0->ISR + u16Port) & DRVUART_MOSINT)?TRUE:FALSE;

		case DRVUART_RLSNT:						/* Receive Line Status Interrupt */
				return (inpw(&UART0->ISR + u16Port) & DRVUART_RLSNT)?TRUE:FALSE;

		case DRVUART_THREINT:					/* Transmit Holding Register Empty Interrupt */
				return (inpw(&UART0->ISR + u16Port) & DRVUART_THREINT)?TRUE:FALSE;

		case DRVUART_RDAINT:					/* Receive Data Available Interrupt */
				return (inpw(&UART0->ISR + u16Port) & DRVUART_RDAINT)?TRUE:FALSE;

		case DRVUART_TOUTINT:					/* Time-out Interrupt */
				return (inpw(&UART0->ISR + u16Port) & DRVUART_TOUTINT)?TRUE:FALSE;

		default:
			return FALSE;
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetFIFOTriggerLevel                                                               */
/*                                                                                                         */
/* Parameter:        																					   */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 						           */
/*               u16TriggerLevel     - [in]   FIFO Trigger Level :LEVEL_1_BYTE to LEVEL_62_BYTES    	   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to set Rx FIFO Trigger Level                                         */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetFIFOTriggerLevel(UART_PORT	u16Port,uint16_t u16TriggerLevel)
{
	if(u16Port == UART_PORT0)
		UART0->FCR.RFITL = u16TriggerLevel;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetCTS				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */
/*				 UART_PORT	   	  	  -[in]   UART Channel:  UART_PORT0 			                       */
/*               pu8CTSValue   		  -[in]   Buffer to store the CTS Value           	         		   */
/*				 pu8CTSChangeState	  -[in]   Buffer to store the CTS Change State                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to get CTS value and change state                                    */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_GetCTS(UART_PORT u16Port,uint8_t *pu8CTSValue,	uint8_t	*pu8CTSChangeState)
{
	if(u16Port == UART_PORT0)
	{
		*pu8CTSValue 		= UART0->MSR.CTS;
		*pu8CTSChangeState 	= UART0->MSR.DCTS;
	}
	else
		return;   /* CTS */
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetRxTimeOut                                                                      */
/*                                                                                                         */
/* Parameter:        																					   */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 							       */
/*               u8TimeOut   		  -[in]   Time out value	           	         		               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to set Rx Time Out Value                                             */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetRxTimeOut(UART_PORT	u16Port, uint8_t u8TimeOut)
{
	if(u16Port == UART_PORT0)
	{
		UART0->TOR.TOIC = u8TimeOut;					/* Set Time out value */
		UART0->IER.TOC_EN =1;							/* Enable Time-out count  */
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Read                                                                              */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 							       */
/*               pu8RxBuf   		  -[in]   Specify the buffer to receive the data of receive FIFO       */
/*               u32ReadBytes  		  -[in]   Specify the bytes number of data.     		               */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/* Description:                                                                                            */
/*               The function is used to read Rx data from RX buffer   	                                   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Read(UART_PORT	u16Port,uint8_t	*pu8RxBuf, uint32_t	u32ReadBytes)
{
    uint32_t  u32Count, u32delayno;

	if(u16Port == UART_PORT0)
	{
	    for (u32Count=0; u32Count < u32ReadBytes; u32Count++)
	    {
	    	 u32delayno = 0;
	         while (UART0->FSR.RX_EMPTY ==1)					  /* Check RX empty => failed */
	         {
	             u32delayno++;
	             if ( u32delayno >= 0x40000000 )
	                return E_DRVUART_ERR_TIMEOUT;

	         }
	         pu8RxBuf[u32Count] = UART0->DATA;					  /* Get Data from UART RX  */
	    }
	}

    return E_SUCCESS;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Write	                                                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 						           */
/*               pu8RxBuf   		  -[in]   Specify the buffer to send the data to transmission FIFO.    */
/*               u32ReadBytes  		  -[in]   Specify the byte number of data.         		               */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is to write data to TX buffer to transmit data by uart                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_Write(UART_PORT	u16Port,uint8_t	*pu8TxBuf, 	uint32_t u32WriteBytes)
{
    uint32_t  u32Count, u32delayno;

	if(u16Port == UART_PORT0)
	{
	    for (u32Count=0; u32Count<u32WriteBytes; u32Count++)
	    {
	       u32delayno = 0;
		   while (UART0->FSR.TX_EMPTY !=1)							/* Wait Tx empty and Time-out manner */
	       {
	       	    u32delayno++;
	       	    if ( u32delayno >= 0x40000000 )
	       	       return E_DRVUART_ERR_TIMEOUT;

	       }
		   UART0->DATA = pu8TxBuf[u32Count];						/* Send UART Data from buffer */
	    }
	}

    return E_SUCCESS;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_OpenIRCR                                                                          */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0							           */
/*               STR_IRCR_T   		  -[in]   Ther stucture of IRCR  	  	         		               */
/*                                            It includes of                                               */
/*                                               u8cTXSelect: Tx select TRUE or FALSE                      */
/*                                               u8cRXSelect: Rx select TRUE or FALSE                      */
/*                                               u8cInvTX: Inverse TX signal                               */
/*                                               u8cInvRX: Inverse RX signal                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is to Set IRCR Control Register                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_OpenIRCR(UART_PORT u16Port,STR_IRCR_T str_IRCR )
{
	if(u16Port == UART_PORT0)
	{
		UART0->FUNSEL.LIN_EN 	= 0;
		UART0->FUNSEL.IrDA_EN 	= 1;
		//UART0->IRCR.RX_EN 		= str_IRCR.u8cRXSelect ;
		UART0->IRCR.TX_SELECT	= str_IRCR.u8cTXSelect ;
		UART0->IRCR.TX_INV_EN	= str_IRCR.u8cInvTX ;
		UART0->IRCR.RX_INV_EN	= str_IRCR.u8cInvRX ;

	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_OpenLIN	                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 							       */
/*               u16DIRECTION         -[in]   MODE_TX or MODE_RX                                           */
/*               u16BCNT       		  -[in]	  Break Count                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to set LIN relative setting                                          */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_OpenLIN(UART_PORT u16Port,uint16_t u16DIRECTION,uint16_t U16BCNT)
{

	if(u16Port == UART_PORT0)
	{
		UART0->FUNSEL.LIN_EN 	= 1;
		UART0->FUNSEL.IrDA_EN 	= 0;
		if(u16DIRECTION ==MODE_TX )
		{
			UART0->LINCON.LINTX_EN	= 1;
			UART0->LINCON.LINRX_EN	= 0;
		}else
		{
			UART0->LINCON.LINTX_EN	= 0;
			UART0->LINCON.LINRX_EN	= 1;
		}
	    UART0->LINCON.LINBCNT = U16BCNT;
	}

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvUART_kbhit                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None                                                                                      */
/*                                                                                                         */
/* Returns:      None                                                                                      */
/* Description:                                                                                            */
/*               This function returns TRUE  when UART get any character.Default use UART0                 */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvUART_kbhit(void)
{

    if(UART0->ISR.RDA_IF ==1)
    {
        return TRUE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_SetPDMA   	                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*				 u16Port	   	  	  -[in]   UART Channel:  UART_PORT0 / UART_PORT0                       */
/*               u16IsEnable   		  -[in]	  Enable TX/RX PDMA TRUE or FASLE                              */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                          	   */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is to Enable/Disable PDMA Channel                                            */
/*---------------------------------------------------------------------------------------------------------*/
void DrvUART_SetPDMA(UART_PORT u16Port,uint16_t u16IsEnable)
{

	if(u16Port == UART_PORT0)
	{
		UART0->IER.DMA_TX_EN 	= u16IsEnable ?1:0;
		UART0->IER.DMA_RX_EN	= u16IsEnable ?1:0;
	}
	else
		return;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_GetVersion                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */
/*	             None                                                                                 	   */
/* Returns:                                                                                                */
/*               Version Number                                                                            */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to get  DrvUART Version Number                                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvUART_GetVersion(void)
{
	return DRVUART_VERSION_NUM;

}

//---------------------------------------------------------------
//
// DrvUART_Init()
//---------------------------------------------------------------
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvUART_Init()  											                               */
/*                                                                                                         */
/* Parameter:        																					   */
/*	             baudrate     		-[in] User seting baudrate; unit: Bits per second                      */
/*                                        computer: 110; 300; 1200; 2400; 4800; 9600; 19200;               */ 
/*                                                  38400; 57600; 115200; 230400; 460800; 921600           */
/*										  			8bits; no-parity; 1 stop bit						   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to initialize the UART settings                                      */
/*---------------------------------------------------------------------------------------------------------*/

void DrvUART_Init(int baudrate)
{
    /* Multi-Function Pin: Enable UART0:Tx Rx */
	SYS->GPA_ALT.GPA8 = 1;
	SYS->GPA_ALT.GPA9 = 1;
    /* Configure GCR to reset UART0 */
    SYS->IPRSTC2.UART0_RST = 1;
    SYS->IPRSTC2.UART0_RST = 0;

    /* Enable UART clock */
	SYSCLK->APBCLK.UART0_EN = 1;

    /* Select UART clock source */
//    SYSCLK->CLKSEL1.UART_S = 0;

    /* Data format */
    UART0->LCR.WLS = 3;

    /* Configure the baud rate */
	DrvUART_BaudRateCalculator(DrvUART_GetUartCLk(), baudrate, &UART0->BAUD);
}








