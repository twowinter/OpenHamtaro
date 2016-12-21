/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "DrvPDMA.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#define MAX_CHANNEL_NUM   			4
#define CHANNEL_OFFSET    			0x100
#define MAX_TRANSFER_BYTE_COUNT     0xFFFF

#define DESTINATION_DIRECTION_BIT   6
#define SOURCE_DIRECTION_BIT        4
#define WAR_BCR_BIT			        12
#define TRANSFER_WIDTH_BIT    	   	19

#define PDMATABORT_IF   			0x1
#define PDMABLKD_IF     			0x2

#define PDMATABORT_IE   			0x1 
#define BLKD_IE         			0x2 
#define WAR_IE          			0x4

#define PDMA_POINT      			0x3

static PFN_DRVPDMA_CALLBACK  g_pfnPDMACallback[4][3]  = {
								{0,0,0},
								{0,0,0},
								{0,0,0},
								{0,0,0}};								

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_Init				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to initialize PDMA      	                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPDMA_Init(void)
{
    UNLOCKREG();
	/* Enable PDMA Clock */
	SYSCLK->AHBCLK.PDMA_EN  =1;		 	
	LOCKREG();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_Close				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function disables all PDMA channel clock and AHB PDMA clock                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPDMA_Close(void)
{
   	/* Disable All PDMA clock */
	PDMA_GCR->GCR.HCLK_EN = 0x00;
	/* Disable PDMA clock */
	UNLOCKREG();
	SYSCLK->AHBCLK.PDMA_EN  =0;
	LOCKREG();					
	
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_Open				                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel	-[in]       PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                                */
/*               sParam	   	-[in]	    - the struct parameter to configure PDMA   	         			   */
/*                         It include of                                                                   */
/*                            sSrcAddr 		- Source Address 											   */									        
/*                            sDestAddr   	- Destination Address										   */
/*                            u8TransWidth 	- Transfer Width                        					   */
/*                            u8Mode 		- Operation Mode   			                                   */
/*                            i32ByteCnt   	- Byte Count                                                   */
/* Returns:                                                                                                */
/*               E_SUCCESS   					Success                                                    */
/*               E_DRVPDMA_ERR_PORT_INVALID     Wrong Port parameter                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function configure PDMA setting       	                                               */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_Open(E_DRVPDMA_CHANNEL_INDEX eChannel, STR_PDMA_T *sParam)
{						  
	volatile int32_t i = 10;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check PDMA channel                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    if (eChannel > eDRVPDMA_CHANNEL_3)
    {
		return E_DRVPDMA_ERR_PORT_INVALID;
    }

	/*-----------------------------------------------------------------------------------------------------*/
    /* Set PDMA settings                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    PDMA_GCR->GCR.HCLK_EN |= (1<<(eChannel));	              /* Enable Channel Clock */
 	PDMA->channel[eChannel].CSR.PDMACEN = 1; 			  	  /* Enable PDMA Channel */
	PDMA->channel[eChannel].CSR.SW_RST=1;					  /* Reset PDMA Channel */
    while(i--);	  /* Need a delay to allow reset */
    
    PDMA->channel[eChannel].SAR = sParam->sSrcAddr.u32Addr;   /* Set Source Address */
    PDMA->channel[eChannel].DAR = sParam->sDestAddr.u32Addr;  /* Set Destination Address */
    /* Set Control Register */
    PDMA->channel[eChannel].CSR.SAD_SEL = sParam->sSrcAddr.eAddrDirection;
    PDMA->channel[eChannel].CSR.DAD_SEL = sParam->sDestAddr.eAddrDirection;
    PDMA->channel[eChannel].CSR.APB_TWS = sParam->u8TransWidth;
    PDMA->channel[eChannel].CSR.MODE_SEL = sParam->u8Mode;
    PDMA->channel[eChannel].CSR.WRA_INT_SEL = sParam->u8WrapBcr;

    PDMA->channel[eChannel].BCR = sParam->i32ByteCnt;	    /* Set Byte Count Register */
    
	return E_SUCCESS;    
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_Clear_CSR   		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                                 */
/* Description:                                                                                            */
/*               The function is used to clear channel control register.                                   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_Clear_CSR(E_DRVPDMA_CHANNEL_INDEX eChannel)
{						  	
	outpw( &PDMA->channel[eChannel].CSR, 0 ) ;   /* clear Control Register */
	
	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_EnableCH   		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/*               eOP                    -[in]     PDMA enable status. It includes						   */
/*                                                eDRVPDMA_DISABLE										   */
/*                                                eDRVPDMA_ENABLE										   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to enable channelx.                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPDMA_EnableCH(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_OPERATION eOP)
{
	PDMA->channel[eChannel].CSR.PDMACEN = eOP; 			  	  /* Enable PDMA Channel */
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_IsEnableCH   		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               Enable status of eChannel                                                                 */
/* Description:                                                                                            */
/*               The function is used to enable channelx.                                                  */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_IsEnabledCH(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
	return 	PDMA->channel[eChannel].CSR.PDMACEN;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_IsCHBusy   		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               TRUE:  The Channel is busy.                                                               */
/*               FALSE: The Channel is usable.                                                             */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to Get Channel Enable/Disable status                                 */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_IsCHBusy(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
    
    if (eChannel > eDRVPDMA_CHANNEL_3)	    								/* Check Channel is valid */
        return E_DRVPDMA_FALSE_INPUT;    
       
    if (PDMA->channel[eChannel].CSR.TRIG_EN)								/* Check Channel is triggering or not */
        return TRUE;
    else
        return FALSE;    
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetTransferLength	                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/*               pu32TransferLength     -[out]    The data pointer to save transfer length                 */
/* Returns:                                                                                                */
/*                                                                                                         */
/* Side effects:                                                                                           */
/*               E_SUCCESS     Success                                                                     */
/*                                                                                                         */
/* Description:                                                                                            */
/*               The function is used to  get channel transfer length setting                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetTransferLength(E_DRVPDMA_CHANNEL_INDEX eChannel, uint32_t* pu32TransferLength)
{
    
    if (eChannel > eDRVPDMA_CHANNEL_3)										/* Check Channel is valid */
        return E_DRVPDMA_FALSE_INPUT; 
            
	*pu32TransferLength = PDMA->channel[eChannel].BCR;					   	/* Get Transfer Length */
    
    return E_SUCCESS;       
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_SetAPBTransferWidth				                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/*               eTransferWidth	   	-[in]   Transfer width. It could be                                     */
/*                                              eDRVPDMA_WIDTH_32BITS                                      */
/*                                              eDRVPDMA_WIDTH_8BITS                                       */
/*                                              eDRVPDMA_WIDTH_16BITS                                      */
/* Returns:                                                                                                */
/*               E_SUCCESS                  Success                                                        */
/*               E_DRVPDMA_FALSE_INPUT      invalid argument                                               */
/* Description:                                                                                            */
/*               The function is used to set APB transfer width for channelx                               */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvPDMA_SetAPBTransferWidth(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_TRANSFER_WIDTH eTransferWidth)
{
   							
    if (eChannel > eDRVPDMA_CHANNEL_3)					   	/* Check Channel is valid */
        return E_DRVPDMA_FALSE_INPUT; 
            
    PDMA->channel[eChannel].CSR.APB_TWS = eTransferWidth;	/* Set PDMA Transfer width */
        
    return E_SUCCESS;     													
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetAPBTransferWidth                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/* Returns:                                                                                                */
/*               Transfer width from channel                                                               */
/* Description:                                                                                            */
/*               The function is used to get transfer width from channelx                                  */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetAPBTransferWidth(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
	volatile int32_t u32Width;
    																		/* Check Channel is valid */
    if (eChannel > eDRVPDMA_CHANNEL_3)
        return E_DRVPDMA_FALSE_INPUT; 
      
	u32Width = PDMA->channel[eChannel].CSR.APB_TWS;    					    /* Get Transfer width */
    
    return u32Width;    
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_SetCHForAPBDevice                                                                 */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/*               eDevice	   		-[in]   APB device   					          	         		   */
/*                                          It includes eDRVPDMA_SPI0,eDRVPDMA_UART0,                      */
/*                                                eDRVPDMA_DPWM,eDRVPDMA_I2S,eDRVPDMA_ADC                  */
/*               eRWAPB             -[in]   Access Direction                                               */
/*                                          It includes eDRVPDMA_WRITE_APB/eDRVPDMA_READ_APB               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_SUCCESS            	Success                       									   */
/*               E_DRVPDMA_FALSE_INPUT  invalid Argument                								   */
/* Description:                                                                                            */
/*               The function is used to select PDMA channel for APB device                                */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvPDMA_SetCHForAPBDevice(
    E_DRVPDMA_CHANNEL_INDEX eChannel, 
    E_DRVPDMA_APB_DEVICE 	eDevice,
    E_DRVPDMA_APB_RW 		eRWAPB    
)
{
    if (eChannel > eDRVPDMA_CHANNEL_3) 						 				/* Check Channel is valid */
        return E_DRVPDMA_FALSE_INPUT; 

	/* check if eChannel is occupied by other APB then disconnect it. */	 														   
	if (PDMA_GCR->PDSSR.SPI0_TXSEL == eChannel)
		PDMA_GCR->PDSSR.SPI0_TXSEL = 0xf;
	if (PDMA_GCR->PDSSR.UART0_TXSEL == eChannel)
		PDMA_GCR->PDSSR.UART0_TXSEL = 0xf;
	if (PDMA_GCR->PDSSR.DPWM_TXSEL == eChannel)
		PDMA_GCR->PDSSR.DPWM_TXSEL = 0xf;
	if (PDMA_GCR->PDSSR.I2S_TXSEL == eChannel)
		PDMA_GCR->PDSSR.I2S_TXSEL = 0xf;
	if (PDMA_GCR->PDSSR.SPI0_RXSEL == eChannel)
		PDMA_GCR->PDSSR.SPI0_RXSEL = 0xf;
	if (PDMA_GCR->PDSSR.UART0_RXSEL == eChannel)
		PDMA_GCR->PDSSR.UART0_RXSEL = 0xf;
	if (PDMA_GCR->PDSSR.ADC_RXSEL == eChannel)
		PDMA_GCR->PDSSR.ADC_RXSEL = 0xf;
	if (PDMA_GCR->PDSSR.I2S_RXSEL == eChannel)
		PDMA_GCR->PDSSR.I2S_RXSEL = 0xf;
   
	switch(eDevice)
	{
		case eDRVPDMA_SPI0:												    /* Set SPI0 PDMA Channel */
			if(eRWAPB)	  
				PDMA_GCR->PDSSR.SPI0_TXSEL	=	eChannel;
			else
				PDMA_GCR->PDSSR.SPI0_RXSEL	=	eChannel;
	   		break;
		case eDRVPDMA_UART0:											   	/* Set UART0 PDMA Channel */
			if(eRWAPB)	
				PDMA_GCR->PDSSR.UART0_TXSEL	=	eChannel;
			else
				PDMA_GCR->PDSSR.UART0_RXSEL	=	eChannel;
	   		break;
		case eDRVPDMA_DPWM:												    /* Set DPWM PDMA Channel */
			if(eRWAPB)	
				PDMA_GCR->PDSSR.DPWM_TXSEL	=	eChannel;
			else
				return 	E_DRVPDMA_FALSE_INPUT;
	   		break;
		case eDRVPDMA_I2S:													/* Set I2S PDMA Channel */
			if(eRWAPB)	
				PDMA_GCR->PDSSR.I2S_TXSEL	=	eChannel;
			else
				PDMA_GCR->PDSSR.I2S_RXSEL	=	eChannel;
	   		break;
		case eDRVPDMA_ADC:													/* Set ADC PDMA Channel */
			if(eRWAPB)	
				return 	E_DRVPDMA_FALSE_INPUT;
			else
				PDMA_GCR->PDSSR.ADC_RXSEL	=	eChannel;
	   		break;
		default:
			return 	E_DRVPDMA_FALSE_INPUT;
	}
 
    return E_SUCCESS;  
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetCHForAPBDevice	                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/*               eDevice	   		-[in]   CH for APB device 			          	         			   */
/*                                          It includes eDRVPDMA_SPI0,eDRVPDMA_UART0,                      */
/*                                                eDRVPDMA_DPWM,eDRVPDMA_I2S,eDRVPDMA_ADC                  */
/* Returns:                                                                                                */
/*               PDMA Channel for APB device                                                               */
/* Description:                                                                                            */
/*               The function is used to  get PDMA channel for APB device                                  */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetCHForAPBDevice(E_DRVPDMA_APB_DEVICE eDevice,E_DRVPDMA_APB_RW eRWAPB)
{
  
	switch(eDevice)
	{
		case eDRVPDMA_SPI0:
			if(eRWAPB)	  
				return PDMA_GCR->PDSSR.SPI0_TXSEL;						 	/* Get SPI0 PDMA Channel */
			else
				return PDMA_GCR->PDSSR.SPI0_RXSEL;
	   		
	   	case eDRVPDMA_DPWM:												 	/* Get DPWM PDMA Channel */
			if(eRWAPB)	  
				return PDMA_GCR->PDSSR.DPWM_TXSEL;
			else
				return E_DRVPDMA_FALSE_INPUT;
	   		   		
		case eDRVPDMA_UART0:											 	/* Get UART0 PDMA Channel */
			if(eRWAPB)	
				return PDMA_GCR->PDSSR.UART0_TXSEL;
			else
				return PDMA_GCR->PDSSR.UART0_RXSEL;
	   		
	   		
		case eDRVPDMA_I2S:													/* Get I2S PDMA Channel */
			if(eRWAPB)	
				return PDMA_GCR->PDSSR.I2S_TXSEL;
			else
				return PDMA_GCR->PDSSR.I2S_RXSEL;
	   		
		case eDRVPDMA_ADC:													/* Get ADC PDMA Channel */
			if(eRWAPB)	
				return E_DRVPDMA_FALSE_INPUT;
			else
				return PDMA_GCR->PDSSR.ADC_RXSEL;
	   		
		default:
			return 	E_DRVPDMA_FALSE_INPUT;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetWrapIntType         				                                           */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               x1xx: a wrap interrupt can be generated when half each PDMA transfer is completed.        */
/*               xxx1: a wrap interrupt can be generated when each PDMA transfer is wrapped.			   */
/*               x1x1: both half and wrap interrupts generated.											   */
/* Description:                                                                                            */
/*               The function is used to get wrap int type of channelx                                     */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetWrapIntType(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
	return PDMA->channel[eChannel].CSR.WRA_INT_SEL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_CHSoftwareReset                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/* Returns:                                                                                                */
/*               E_SUCCESS               Success                                                           */
/*               E_DRVPDMA_FALSE_INPUT   invalid Argument                                                  */
/* Description:                                                                                            */
/*               The function is used to software reset Channelx                                           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_CHSoftwareReset(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
    if (eChannel > eDRVPDMA_CHANNEL_3)		  /* Check PDMA Channel is Valid */
        return E_DRVPDMA_FALSE_INPUT;
  			
     PDMA->channel[eChannel].CSR.SW_RST = 1;  /* Set S/W Reset Bit */
    return E_SUCCESS;     
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_CHEnablelTransfer                                                                 */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel			-[in]   PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                            */
/* Returns:                                                                                                */
/*               E_SUCCESS                Success                                                          */
/*               E_DRVPDMA_FALSE_INPUT    invalid Argument                                                 */
/* Description:                                                                                            */
/*               The function is used to enable PDMA data read or write transfer                           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvPDMA_CHEnablelTransfer(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
 	/* Check PDMA Channel is Valid */
    if (eChannel > eDRVPDMA_CHANNEL_3)
        return E_DRVPDMA_FALSE_INPUT;
            
  
   	/* PDMA Channel Enable & Trigger Enable */
    PDMA->channel[eChannel].CSR.PDMACEN = 1;
    PDMA->channel[eChannel].CSR.TRIG_EN = 1;
    return E_SUCCESS;      
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_EnableInt			                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               eIntSource	   			-[in] Interrupt source: eDRVPDMA_TABORT/eDRVPDMA_BLKD/eDRVPDMA_WAR */
/* Returns:                                                                                                */
/*               E_SUCCESS                Success                                                          */
/*               E_DRVPDMA_FALSE_INPUT    invalid Argument                                                 */
/* Description:                                                                                            */
/*               The function is used to enable Interrupt for channelx                                     */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_EnableInt(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_ENABLE eIntSource)
{
    if (eChannel > eDRVPDMA_CHANNEL_3)							/* Check PDMA Channel is Valid */
        return E_DRVPDMA_FALSE_INPUT;

	outpw(&PDMA->channel[eChannel].IER, inpw(&PDMA->channel[eChannel].IER) | eIntSource); 
  																		
	NVIC_EnableIRQ(PDMA_IRQn);  							   	/* Enable Specified Interrupt */
   
    return E_SUCCESS;      
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_DisableInt  		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               eIntSource	   			-[in] Interrupt source: eDRVPDMA_TABORT/eDRVPDMA_BLKD/eDRVPDMA_WAR */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to disable Interrupt for channelx                                    */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPDMA_DisableInt(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_ENABLE eIntSource)
{ 						   
	/* Disable Specified Interrupt */
	outpw(&PDMA->channel[eChannel].IER, inpw(&PDMA->channel[eChannel].IER) & ~eIntSource); 
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_IsIntEnabled   	                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               eIntSource	   			-[in] Interrupt source: eDRVPDMA_TABORT/eDRVPDMA_BLKD/eDRVPDMA_WAR */
/* Returns:                                                                                                */
/*               TRUE:     The Specified Source Interrupt is enable                                        */
/*               FASLE:    The Specified Source Interrupt is disable                                       */
/* Description:                                                                                            */
/*               The function is used to check if the specified interrupt source is enabled in Channelx    */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvPDMA_IsIntEnabled(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_ENABLE eIntSource )
{
   
    switch(eIntSource)					/* Read ISR Register and check Specified Interrupt Enable or not */
    {														
    	case eDRVPDMA_TABORT: 			
			return ((PDMA->channel[eChannel].ISR.TABORT_IF) ? TRUE : FALSE);	/* Target Abort Interrupt */
    		
    	case eDRVPDMA_BLKD:    					
			return ((PDMA->channel[eChannel].ISR.BLKD_IF) ? TRUE : FALSE);		/* Transfer Done Interrupt */
    															
    	case eDRVPDMA_WAR:   					
			return ((PDMA->channel[eChannel].ISR.WAR_IF) ? TRUE : FALSE);	    /* Wrap Around Interrupt */
    		
		default :
	    	return E_DRVPDMA_FALSE_INPUT;		    		
	}    
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_ClearInt			                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               eIntFlag	   			-[in] Interrupt Flag, the flag contains                            */
/*	                                             eDRVPDMA_TABORT_FLAG            						   */
/*	                                             eDRVPDMA_BLKD_FLAG              						   */
/*	                                             eDRVPDMA_WRA_EMPTY_FLAG         	          	    	   */
/*                                               eDRVPDMA_WRA_THREE_FOURTHS_FLAG 	  					   */
/*                                               eDRVPDMA_WRA_HALF_FLAG                       			   */
/*                                               eDRVPDMA_WRA_QUARTER_FLAG        						   */
/*																										   */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to clear interrupt status for channelx                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPDMA_ClearInt(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_FLAG eIntFlag)
{
	switch(eIntFlag)
	{
		case eDRVPDMA_TABORT_FLAG:
			PDMA->channel[eChannel].ISR.TABORT_IF = 1;
			break;

		case eDRVPDMA_BLKD_FLAG:
			PDMA->channel[eChannel].ISR.BLKD_IF = 1;
			break;

		case eDRVPDMA_WRA_EMPTY_FLAG:
			PDMA->channel[eChannel].ISR.WAR_IF = 1;
			break;

		case eDRVPDMA_WRA_THREE_FOURTHS_FLAG:
			PDMA->channel[eChannel].ISR.WAR_IF = 2;
			break;

		case eDRVPDMA_WRA_HALF_FLAG:
			PDMA->channel[eChannel].ISR.WAR_IF = 4;
			break;

		case eDRVPDMA_WRA_QUARTER_FLAG:
			PDMA->channel[eChannel].ISR.WAR_IF = 8;
			break;
		default:
			break;
	}
}
						  

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_PollInt    		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               eIntFlag	   			-[in] Interrupt source: the flag contains                          */
/*	                                             eDRVPDMA_TABORT_FLAG            						   */
/*	                                             eDRVPDMA_BLKD_FLAG              						   */
/*	                                             eDRVPDMA_WRA_EMPTY_FLAG         	          	    	   */
/*                                               eDRVPDMA_WRA_THREE_FOURTHS_FLAG 	  					   */
/*                                               eDRVPDMA_WRA_HALF_FLAG                       			   */
/*                                               eDRVPDMA_WRA_QUARTER_FLAG        						   */
/* Returns:                                                                                                */
/*               TRUE:     The Source Interrupt Flag is set                                                */
/*               FASLE:    The Source Interrupt Flag is clear                                              */
/* Description:                                                                                            */
/*               The function is used to polling channel interrupt status                                  */
/*---------------------------------------------------------------------------------------------------------*/
int32_t	DrvPDMA_PollInt(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_FLAG eIntFlag)
{
	switch(eIntFlag)
	{
		case eDRVPDMA_TABORT_FLAG:
			return ((PDMA->channel[eChannel].ISR.TABORT_IF) ? TRUE : FALSE);

		case eDRVPDMA_BLKD_FLAG:
			return ((PDMA->channel[eChannel].ISR.BLKD_IF) ? TRUE : FALSE);

		case eDRVPDMA_WRA_EMPTY_FLAG:
			return ((PDMA->channel[eChannel].ISR.WAR_IF & 0x01) ? TRUE : FALSE);

		case eDRVPDMA_WRA_THREE_FOURTHS_FLAG:
			return ((PDMA->channel[eChannel].ISR.WAR_IF & 0x02) ? TRUE : FALSE);

		case eDRVPDMA_WRA_HALF_FLAG:
			return ((PDMA->channel[eChannel].ISR.WAR_IF & 0x04) ? TRUE : FALSE);

		case eDRVPDMA_WRA_QUARTER_FLAG:
			return ((PDMA->channel[eChannel].ISR.WAR_IF & 0x08) ? TRUE : FALSE);

		default:
			return FALSE;
	}
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetInternalBufPointer				                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               Internal Buffer Pointer                                                                   */
/* Description:                                                                                            */
/*               The function is used to  Get Internal Buffer Pointer                                      */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPDMA_GetInternalBufPointer(E_DRVPDMA_CHANNEL_INDEX eChannel)
{    
    if (eChannel > eDRVPDMA_CHANNEL_3)
        return E_DRVPDMA_FALSE_INPUT;    

	return PDMA->channel[eChannel].POINT;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     PDMA_IRQHandler     		                                                               */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 None 												                                       */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Description:                                                                                            */
/*               The function is used to  PDMA ISR                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void PDMA_IRQHandler(void)
{
    volatile uint32_t u32WraparoundStatus;
    volatile int32_t  i32Channel= 0 ;
  
	for(i32Channel=0;i32Channel<MAX_CHANNEL_NUM;i32Channel++)  
	{ 										 			
		if(PDMA_GCR->GCRISR & (1<<i32Channel))										 /* Check INT Channel */
		{			 							
			if(PDMA->channel[i32Channel].ISR.TABORT_IF)								/* Check Target Abort INT and clear */
			{
			   PDMA->channel[i32Channel].ISR.TABORT_IF = 1;

			   if (g_pfnPDMACallback[i32Channel][0] != 0)    		
		    		(*g_pfnPDMACallback[i32Channel][0])(PDMATABORT_IF);				/* Target Abort callback funtion */
				
			}			 							
			else if(PDMA->channel[i32Channel].ISR.BLKD_IF)							/* Transfer Done INT and clear */
			{
				PDMA->channel[i32Channel].ISR.BLKD_IF = 1;

				if (g_pfnPDMACallback[i32Channel][1] != 0)    			
			    	(*g_pfnPDMACallback[i32Channel][1])(PDMABLKD_IF);				/* Transfer Done INT callback funtion */
			}
			else 										 							/* Wrap Around INT */
			{
				if(PDMA->channel[i32Channel].ISR.WAR_IF)
		    	{																	/* Check Wrap Around Method and clear */
					u32WraparoundStatus = PDMA->channel[i32Channel].ISR.WAR_IF;
					if(PDMA->channel[i32Channel].ISR.WAR_IF & 0x04)
						  PDMA->channel[i32Channel].ISR.WAR_IF = 0x04;
					else
						  PDMA->channel[i32Channel].ISR.WAR_IF = 0x01;

					if (g_pfnPDMACallback[i32Channel][2] != 0)		    						
			  			(*g_pfnPDMACallback[i32Channel][2])(u32WraparoundStatus); 	/* Wrap Around INT callback funtion */
	 		
					
		    	}		
			}

			
		}
	
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_InstallCallBack                                                                   */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in] PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                          */
/*               E_DRVPDMA_INT_ENABLE	-[in] Interrupt source:eDRVPDMA_TABORT/eDRVPDMA_BLKD/eDRVPDMA_WAR  */
/*               pfncallback            -[in] The callback function pointer                                */
/* Returns:                                                                                                */
/*               E_SUCCESS                Success                                                          */
/*               E_DRVPDMA_FALSE_INPUT    invalid Argument                                                 */
/* Description:                                                                                            */
/*               The function is used to install call back function for Channelx & Interrupt source        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_InstallCallBack(
	E_DRVPDMA_CHANNEL_INDEX eChannel, 
    E_DRVPDMA_INT_ENABLE eIntSource,
	PFN_DRVPDMA_CALLBACK pfncallback   
)
{

    if (eChannel > eDRVPDMA_CHANNEL_3)								/* Check PDMA Channel is Valid */
        return E_DRVPDMA_FALSE_INPUT;

	g_pfnPDMACallback[eChannel][eIntSource>>1] = pfncallback;	    /* Install Callback funtion */
	
	NVIC_EnableIRQ(PDMA_IRQn);     

    return E_SUCCESS;      
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetCurrentSourceAddr                                                              */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               Current Source Address from channelx                                                      */
/* Description:                                                                                            */
/*               The function is used to get current source address from channelx                          */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPDMA_GetCurrentSourceAddr(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
    return PDMA->channel[eChannel].CSAR;    
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetCurrentDestAddr                                                                */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               Current destination address form channelx                                                 */
/* Description:                                                                                            */
/*               The function is used to get current destination address from channelx                     */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPDMA_GetCurrentDestAddr(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
	return PDMA->channel[eChannel].CDAR;    
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetCurrentTransferCount				                                           */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/* Returns:                                                                                                */
/*               Get Current Transfer Count from channelx                                                  */
/* Description:                                                                                            */
/*               The function is used to get current transfer count from channelx                          */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPDMA_GetCurrentTransferCount(E_DRVPDMA_CHANNEL_INDEX eChannel)
{
	return PDMA->channel[eChannel].CBCR.CBCR;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_SetTransferSetting       				                                           */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/*				 psSrcAddr				-[in]	  pointer to SrcAddr 									   */
/*               psDestAddr             -[in]     pointer to DestAddr                                      */
/*               u32TransferLength		-[in]     transfer length in byte 								   */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                                 */
/*               E_DRVPDMA_FALSE_INPUT    invalid Argument                                                 */
/* Description:                                                                                            */
/*               The function is used to set transfer setting from channelx                                */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_SetTransferSetting(
	E_DRVPDMA_CHANNEL_INDEX eChannel, 
	S_DRVPDMA_CH_ADDR_SETTING* psSrcAddr, 
	S_DRVPDMA_CH_ADDR_SETTING* psDestAddr, 
	uint32_t u32TransferLength
)
{
    if (eChannel > eDRVPDMA_CHANNEL_3)					/* Check PDMA Channel is Valid */
        return E_DRVPDMA_FALSE_INPUT;

    PDMA->channel[eChannel].SAR = psSrcAddr->u32Addr;   /* Set Source Address */
    PDMA->channel[eChannel].DAR = psDestAddr->u32Addr;  /* Set Destination Address */
    PDMA->channel[eChannel].CSR.SAD_SEL = psSrcAddr->eAddrDirection;
    PDMA->channel[eChannel].CSR.DAD_SEL = psDestAddr->eAddrDirection;
	PDMA->channel[eChannel].BCR = u32TransferLength;    /* Set Byte Count Register */

	return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetTransferSetting       				                                           */
/*                                                                                                         */
/* Parameter:        																					   */	
/*				 eChannel				-[in]     PDMA Source: eDRVPDMA_CHANNEL_0 ~ 3                      */
/*				 eTarget				-[in]	  Specify PDMA source or destination                       */
/*               pu32Addr               -[out]    pointer to return address                                */
/*               peDirection		    -[out]    pointer to return direction                              */
/* Returns:                                                                                                */
/*               E_SUCCESS                                                                                 */
/*               E_DRVPDMA_FALSE_INPUT    invalid Argument                                                 */
/* Description:                                                                                            */
/*               The function is used to get transfer setting from channelx                                */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetTransferSetting(
	E_DRVPDMA_CHANNEL_INDEX eChannel, 
	E_DRVPDMA_TARGET eTarget, 
	uint32_t* pu32Addr, 
	E_DRVPDMA_DIRECTION_SELECT* peDirection
)
{
    if (eChannel > eDRVPDMA_CHANNEL_3)							/* Check PDMA Channel is Valid */
        return E_DRVPDMA_FALSE_INPUT;

	if(eTarget == eDRVPDMA_TARGET_SOURCE) {
		*pu32Addr = PDMA->channel[eChannel].SAR;
		*peDirection = (E_DRVPDMA_DIRECTION_SELECT)PDMA->channel[eChannel].CSR.SAD_SEL;
	} else if(eTarget == eDRVPDMA_TARGET_DESTINATION) {
		*pu32Addr = PDMA->channel[eChannel].DAR;
		*peDirection = (E_DRVPDMA_DIRECTION_SELECT)PDMA->channel[eChannel].CSR.DAD_SEL;
	}

	return E_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     DrvPDMA_GetVersion                                                                        */
/*                                                                                                         */
/* Parameter:        																					   */	
/*	             None                                                                                 	   */
/* Returns:                                                                                                */
/*               Version Number                                                                            */
/* Description:                                                                                            */
/*               The function is used to get DrvPDMA Version Number                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPDMA_GetVersion(void)
{
	return DRVPDMA_VERSION_NUM;
}


