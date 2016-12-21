/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Include related headers                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#include "ISD9xx.h"
#include "core_cm0.h"
#include "DrvSPI.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    BOOL bBusy;
    PFN_DRVSPI_CALLBACK pfncallback;
    uint32_t u32userData;
} S_DRVSPI_HANDLE;

static S_DRVSPI_HANDLE g_sSpiHandler[4];

static SPI_T * SPI_PORT[1]={SPI0};

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function: SPI0_IRQHandler                                                                               */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    SPI0 interrupt handler.                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SPI0_IRQHandler(void)
{
	// write '1' to clear SPI0 interrupt flag
	SPI0->CNTRL.IF = 1;
	
	if(g_sSpiHandler[0].pfncallback != NULL)
	{
	g_sSpiHandler[0].pfncallback(g_sSpiHandler[0].u32userData);
	}
	
}


/*--------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_Open                                                                                        */
/*                                                                                                              */
/* Parameters:        	                                                                                        */
/*         eSpiPort     [in]: Specify the SPI port                                                              */
/*         eMode        [in]: Specify the operation mode (eDRVSPI_MASTER/eDRVSPI_SLAVE/eDRVSPI_JOYSTICK)        */
/*         eType        [in]: Specify the transfer type (eDRVSPI_TYPE0 ~ eDRVSPI_TYPE7)                         */
/*         i32BitLength [in]: Specify the bit length in a transaction (1~32)                                    */
/*                                                                                                              */
/* Returns:                                                                                                     */
/*         E_DRVSPI_ERR_INIT: The specified SPI port has been opened before.                                    */
/*         E_DRVSPI_ERR_BUSY: The specified SPI port is in busy status.                                         */
/*         E_DRVSPI_ERR_BIT_LENGTH: The specified bit length is out of range.                                   */
/*         E_SUCCESS: Success.                                                                                  */
/*                                                                                                              */
/* Description:                                                                                                 */
/*       Configure the operation mode, transfer type and bit length of a transaction of the specified SPI port. */
/*        The Timing of each SPI types:                                                                         */
/*
DRVSPI_TYPE0:          
             _________________________________
    CS    __|                                 |___
               _   _   _   _   _   _   _   _  
    CLK   ____| |_| |_| |_| |_| |_| |_| |_| |_____
              
    Tx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |---
          
    Rx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |-----


DRVSPI_TYPE1:          
             _________________________________
    CS    __|                                 |___
               _   _   _   _   _   _   _   _  
    CLK   ____| |_| |_| |_| |_| |_| |_| |_| |_____
              
    Tx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |-----
          
    Rx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |-----


DRVSPI_TYPE2:          
             _________________________________
    CS    __|                                 |___
               _   _   _   _   _   _   _   _  
    CLK   ____| |_| |_| |_| |_| |_| |_| |_| |_____
              
    Tx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |---
          
    Rx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |---


DRVSPI_TYPE3:          
             _________________________________
    CS    __|                                 |___
               _   _   _   _   _   _   _   _  
    CLK   ____| |_| |_| |_| |_| |_| |_| |_| |_____
              
    Tx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |-----
          
    Rx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |---


DRVSPI_TYPE4:          
             __________________________________
    CS    __|                                  |___
           ___   _   _   _   _   _   _   _   ______ 
    CLK       |_| |_| |_| |_| |_| |_| |_| |_|  
              
    Tx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |------
          
    Rx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |----


DRVSPI_TYPE5:
             _________________________________
    CS    __|                                 |____
           ___   _   _   _   _   _   _   _   ______ 
    CLK       |_| |_| |_| |_| |_| |_| |_| |_|  
              
    Tx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |----
          
    Rx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |----


DRVSPI_TYPE6:
             _________________________________
    CS    __|                                 |____
           ___   _   _   _   _   _   _   _   ______ 
    CLK       |_| |_| |_| |_| |_| |_| |_| |_|  
              
    Tx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |------
          
    Rx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |------


DRVSPI_TYPE7:
             _________________________________
    CS    __|                                 |____
           ___   _   _   _   _   _   _   _   ______ 
    CLK       |_| |_| |_| |_| |_| |_| |_| |_|  
              
    Tx    ----| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |----
          
    Rx    --| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |----


Master / Slave Transfer Type Matching Table

    DRVSPI_TYPE0 <==> DRVSPI_TYPE3
    DRVSPI_TYPE1 <==> DRVSPI_TYPE1
    DRVSPI_TYPE2 <==> DRVSPI_TYPE2
    DRVSPI_TYPE3 <==> DRVSPI_TYPE0
    DRVSPI_TYPE4 <==> DRVSPI_TYPE7
    DRVSPI_TYPE5 <==> DRVSPI_TYPE5
    DRVSPI_TYPE6 <==> DRVSPI_TYPE6
    DRVSPI_TYPE7 <==> DRVSPI_TYPE4
*/
/*--------------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_Open(E_DRVSPI_PORT eSpiPort, E_DRVSPI_MODE eMode, E_DRVSPI_TRANS_TYPE eType, int32_t i32BitLength)
{
	int32_t i32TimeOut;

	if(eSpiPort == SPI_PORT0)
	{
		SYSCLK->APBCLK.SPI0_EN        =1;
		SYS->IPRSTC2.SPI0_RST         =1;
		SYS->IPRSTC2.SPI0_RST         =0;
		SYS->GPA_ALT.GPA0              =1; // MOSI0
		SYS->GPA_ALT.GPA1              =1; // SCLK
		SYS->GPA_ALT.GPA2              =1; // SSB0
		SYS->GPA_ALT.GPA3              =1; // MISO0
		
	}
	
	if(g_sSpiHandler[eSpiPort].bBusy)
	{
		return E_DRVSPI_ERR_INIT;
	}
	
	/* Check busy*/
	i32TimeOut = 0x10000;
	while(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY == 1)
	{
		if(i32TimeOut-- <= 0)
			return E_DRVSPI_ERR_BUSY;
	}
	
	g_sSpiHandler[eSpiPort].bBusy = TRUE;
	g_sSpiHandler[eSpiPort].pfncallback = NULL;
	g_sSpiHandler[eSpiPort].u32userData = 0;
	
	if(eMode != eDRVSPI_JOYSTICK)
	{
	    /* Bit length 1 ~ 32 */
	    if((i32BitLength <= 0) || (i32BitLength > 32))
	    {
	        return E_DRVSPI_ERR_BIT_LENGTH;
	    }
    }
   
	if(eMode != eDRVSPI_JOYSTICK)
	{
		/* "i32BitLength = 0" means 32 bits */
		if(i32BitLength == 32)
		{
			i32BitLength = 0;
		}
		SPI_PORT[eSpiPort]->CNTRL.TX_BIT_LEN = i32BitLength;
	}
	
	if(eMode == eDRVSPI_JOYSTICK)
	{
		SPI_PORT[eSpiPort]->JS.READYB = 1;
		SPI_PORT[eSpiPort]->JS.JS = 1;
		SPI_PORT[eSpiPort]->SSR.SSR = 0;
		SPI_PORT[eSpiPort]->SSR.SS_LVL = 0;
		SPI_PORT[eSpiPort]->SSR.ASS = 0;
		SPI_PORT[eSpiPort]->SSR.SS_LTRIG = 0;
		SPI_PORT[eSpiPort]->TX[0] = 0;
		SPI_PORT[eSpiPort]->TX[1] = 0;
		SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 1;
		SPI_PORT[eSpiPort]->CNTRL.CLKP = 1;
		SPI_PORT[eSpiPort]->CNTRL.SLAVE = 1;
		SPI_PORT[eSpiPort]->CNTRL.IE = 1;
	}
	else
	{
		SPI_PORT[eSpiPort]->JS.JS = 0;
		if(eMode == eDRVSPI_SLAVE)
			SPI_PORT[eSpiPort]->CNTRL.SLAVE = 1;
	
		/* Default to automatic slave select and it is low active */
		SPI_PORT[eSpiPort]->SSR.ASS = 1;
		
		/* Transition types */
		if(eType==eDRVSPI_TYPE0)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 0;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 0;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 0;
		}
		else if(eType==eDRVSPI_TYPE1)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 0;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 1;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 0;
		}
		else if(eType==eDRVSPI_TYPE2)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 0;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 0;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 1;
		}
		else if(eType==eDRVSPI_TYPE3)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 0;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 1;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 1;
		}
		else if(eType==eDRVSPI_TYPE4)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 1;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 0;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 0;
		}
		else if(eType==eDRVSPI_TYPE5)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 1;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 1;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 0;
		}
		else if(eType==eDRVSPI_TYPE6)
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 1;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 0;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 1;
		}
		else
		{
			SPI_PORT[eSpiPort]->CNTRL.CLKP = 1;
			SPI_PORT[eSpiPort]->CNTRL.TX_NEG = 1;
			SPI_PORT[eSpiPort]->CNTRL.RX_NEG = 1;
		}
		
    }

//To avoid SPI pins loading, set SPI clk as low rate
	SPI_PORT[eSpiPort]->DIVIDER.DIVIDER = 1;
	SPI_PORT[eSpiPort]->DIVIDER.DIVIDER2 = 1;
			
    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_Close                                                                                  */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort [in]:  Specify the SPI port.                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Close the specified SPI module and disable the SPI interrupt.                                        */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSPI_Close(E_DRVSPI_PORT eSpiPort)
{
	int32_t i32TimeOut;
	
	g_sSpiHandler[eSpiPort].bBusy = FALSE;
	g_sSpiHandler[eSpiPort].pfncallback = NULL;
	g_sSpiHandler[eSpiPort].u32userData = 0;

	/* Wait SPIMS Busy */
	i32TimeOut = 0x10000;
	while(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY == 1)
	{
		if(i32TimeOut-- <= 0)
			break;
	}
   
   if(eSpiPort == SPI_PORT0)
	{
		NVIC_DisableIRQ(SPI0_IRQn);
		SYS->IPRSTC2.SPI0_RST=1;
		SYS->IPRSTC2.SPI0_RST=0;
		SYSCLK->APBCLK.SPI0_EN=0;
	}
   
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_Set2BitSerialDataIOMode                                                                           */
/*                                                                                                                    */
/* Parameters:        	                                                                                              */
/*    eSpiPort [in]: Specify the SPI port                                                                             */
/*    bEnable  [in]: Enable (TRUE) / Disable (FALSE)                                                                  */
/*                                                                                                                    */
/* Returns:                                                                                                           */
/*    None.                                                                                                           */
/*                                                                                                                    */
/* Description:                                                                                                       */
/*    Set 2-bit serial data I/O mode.                                                                                 */
/*    When enable 2-bit serial data I/O mode, the Tx_NUM must be configure as 0x00 (one transaction in one transfer.) */
/*--------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_Set2BitSerialDataIOMode(E_DRVSPI_PORT eSpiPort, BOOL bEnable)
{
	if(bEnable)
	{
		SPI_PORT[eSpiPort]->CNTRL.TWOB = 1;
		SPI_PORT[eSpiPort]->CNTRL.TX_NUM = 0;
		if(eSpiPort == SPI_PORT0)
		{			
			SYS->GPB_ALT.GPB5              =3; // MISO1
			SYS->GPB_ALT.GPB6              =3; // MOSI1
		}
	}
	else
		SPI_PORT[eSpiPort]->CNTRL.TWOB = 0;
}

/*------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetEndian                                                                                       */
/*                                                                                                                  */
/* Parameters:        	                                                                                            */
/*    eSpiPort [in]: Specify the SPI port                                                                           */
/*    eEndian  [in]: Specify LSB first or MSB first                                                                 */
/*                                                                                                                  */
/* Returns:                                                                                                         */
/*    None.                                                                                                         */
/*                                                                                                                  */
/* Description:                                                                                                     */
/*    Dertermine to transfer data with LSB first or MSB first                                                       */
/*------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetEndian(E_DRVSPI_PORT eSpiPort, E_DRVSPI_ENDIAN eEndian)
{

	if(eEndian == eDRVSPI_LSB_FIRST)
	{
		SPI_PORT[eSpiPort]->CNTRL.LSB = 1;
	}
	else
		SPI_PORT[eSpiPort]->CNTRL.LSB = 0;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetBitLength                                                                           */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort     [in]: Specify the SPI port                                                              */
/*    i32BitLength [in]: Specify the bit length (1~32 bits)                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    E_SUCCESS:                 Success.                                                                  */
/*    E_DRVSPI_ERR_BIT_LENGTH: The bit length is out of range.                                             */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the bit length of SPI transfer.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_SetBitLength(E_DRVSPI_PORT eSpiPort, int32_t i32BitLength)
{

    if((i32BitLength < 1) || (i32BitLength > 32))
    {
        return E_DRVSPI_ERR_BIT_LENGTH;
    }
    if(i32BitLength == 32)
        i32BitLength = 0;

	SPI_PORT[eSpiPort]->CNTRL.TX_BIT_LEN = i32BitLength;
	
    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetByteSleep                                                                           */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort     [in]: Specify the SPI port                                                              */
/*    bEnable      [in]: Enable (TRUE) / Disable (FALSE)                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    E_SUCCESS:                 Success.                                                                  */
/*    E_DRVSPI_ERR_BIT_LENGTH: The bit length is not 32 bits.                                              */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable/disable Byte Sleep function.                                                                  */
/*    The Byte Sleep function is supported only in word (32 bits) transaction mode.                        */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_SetByteSleep(E_DRVSPI_PORT eSpiPort, BOOL bEnable)
{
	
	if(SPI_PORT[eSpiPort]->CNTRL.TX_BIT_LEN != 0)
		return E_DRVSPI_ERR_BIT_LENGTH;

	if(bEnable)
		SPI_PORT[eSpiPort]->CNTRL.SLEEP = 1;
	else
		SPI_PORT[eSpiPort]->CNTRL.SLEEP = 0;

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetByteReorder                                                                         */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort     [in]: Specify the SPI port                                                              */
/*    bEnable      [in]: Enable (TRUE) / Disable (FALSE)                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    E_SUCCESS:                 Success.                                                                  */
/*    E_DRVSPI_ERR_BIT_LENGTH: The bit length is not 16-, 24- or 32-bit.                                   */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable/disable Byte Reorder function.                                                                */
/*    The Byte Reorder function is supported only in 16-, 24- and 32-bit transaction mode.                 */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_SetByteEndian(E_DRVSPI_PORT eSpiPort, BOOL bEnable)
{
	
	if( (SPI_PORT[eSpiPort]->CNTRL.TX_BIT_LEN) % 8 )
		return E_DRVSPI_ERR_BIT_LENGTH;

	if(bEnable)
		SPI_PORT[eSpiPort]->CNTRL.BYTE_ENDIAN = 1;
	else
		SPI_PORT[eSpiPort]->CNTRL.BYTE_ENDIAN = 0;

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetTriggerMode                                                                         */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort       [in]: Specify the SPI port                                                            */
/*    eSSTriggerMode [in]: Specify the trigger mode. (eDRVSPI_EDGE_TRIGGER or eDRVSPI_LEVEL_TRIGGER)       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the trigger mode of slave select pin.                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetTriggerMode(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SSLTRIG eSSTriggerMode)
{
    SPI_PORT[eSpiPort]->SSR.SS_LTRIG = eSSTriggerMode;
}


/*----------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetSlaveSelectActiveLevel                                                               */
/*                                                                                                          */
/* Parameters:        	                                                                                    */
/*    eSpiPort   [in]: Specify the SPI port                                                                 */
/*    eSSActType [in]: Select the active type of slave select pin.                                          */
/*                     eDRVSPI_ACTIVE_LOW_FALLING: Slave select pin is active low in level-trigger mode;    */
/*                                                 or falling-edge trigger in edge-trigger mode.            */
/*                     eDRVSPI_ACTIVE_HIGH_RISING: Slave select pin is active high in level-trigger mode;   */
/*                                                 or rising-edge trigger in edge-trigger mode.             */
/*                                                                                                          */
/* Returns:                                                                                                 */
/*    None.                                                                                                 */
/*                                                                                                          */
/* Description:                                                                                             */
/*    Set the active level of slave select.                                                                    */
/*----------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetSlaveSelectActiveLevel(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SS_ACT_TYPE eSSActType)
{
    SPI_PORT[eSpiPort]->SSR.SS_LVL = eSSActType;
}

/*--------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetLevelTriggerStatus                                                                       */
/*                                                                                                              */
/* Parameters:        	                                                                                        */
/*    eSpiPort   [in]: Specify the SPI port                                                                     */
/*                                                                                                              */
/* Returns:                                                                                                     */
/*    TRUE : The received trasaction number and received bits met the requirement which defines in TX_NUM       */
/*           and TX_BIT_LEN among one transfer.                                                                 */
/*    FALSE: The transaction number or the received bit length of one transaction doesn't meet the requirement  */
/*           in one transfer.                                                                                   */
/*                                                                                                              */
/* Description:                                                                                                 */
/*    Get the level-trigger transmission status.                                                                */
/*--------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_GetLevelTriggerStatus(E_DRVSPI_PORT eSpiPort)
{
	if(SPI_PORT[eSpiPort]->SSR.LTRIG_FLAG==1)
		return TRUE;
	else
		return FALSE;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_EnableAutoCS                                                                            */
/*                                                                                                          */
/* Parameters:        	                                                                                    */
/*    eSpiPort   [in]: Specify the SPI port                                                                 */
/*    eSlaveSel  [in]: Select the slave select pins which will be used.                                     */
/*                                                                                                          */
/* Returns:                                                                                                 */
/*    None.                                                                                                 */
/*                                                                                                          */
/* Description:                                                                                             */
/*    Enable the automatic slave select function and set the specified slave select pin.                    */
/*----------------------------------------------------------------------------------------------------------*/
void DrvSPI_EnableAutoCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel)
{
    SPI_PORT[eSpiPort]->SSR.ASS = 1;
    SPI_PORT[eSpiPort]->SSR.SSR = eSlaveSel;
}

/*------------------------------------------------------------------------------------
  DMA Burst SS
*/
void DrvSPI_EnableDMABurstSS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel)
{SPI_PORT[eSpiPort]->CNTRL.DMA_ASS_BURST = 1;}

void DrvSPI_DisableDMABurstSS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel)
{SPI_PORT[eSpiPort]->CNTRL.DMA_ASS_BURST = 0;}


/*----------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_DisableAutoCS                                                                           */
/*                                                                                                          */
/* Parameters:        	                                                                                    */
/*    eSpiPort   [in]: Specify the SPI port                                                                 */
/*                                                                                                          */
/* Returns:                                                                                                 */
/*    None.                                                                                                 */
/*                                                                                                          */
/* Description:                                                                                             */
/*    Disable the Automatic Slave Slect function and deselect slave select pins.                            */
/*----------------------------------------------------------------------------------------------------------*/
void DrvSPI_DisableAutoCS(E_DRVSPI_PORT eSpiPort)
{
    SPI_PORT[eSpiPort]->SSR.ASS = 0;
    SPI_PORT[eSpiPort]->SSR.SSR = eDRVSPI_NONE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetCS                                                                                            */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*   eSpiPort   [in]: Specify the SPI port                                                                           */
/*   eSlaveSel  [in]: In automatic slave select operation, to use this parameter to select the slave select pins     */
/*                    which will be used.                                                                            */
/*                    In manual slave select operation, the specified slave select pins will be set to active state. */
/*                    It could be eDRVSPI_NONE, eDRVSPI_SS0, eDRVSPI_SS1 or eDRVSPI_SS0_SS1.                         */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Configure the slave select pins.                                                                               */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel)
{
    SPI_PORT[eSpiPort]->SSR.SSR = eSlaveSel;
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_ClrCS                                                                                            */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort  [in]: Specify the SPI port                                                                           */
/*    eSlaveSel [in]: Specify slave select pins                                                                      */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Set the specified slave select pins to inactive state.                                                         */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_ClrCS(E_DRVSPI_PORT eSpiPort, E_DRVSPI_SLAVE_SEL eSlaveSel)
{
	uint32_t u32Reg;
	
	u32Reg = SPI_PORT[eSpiPort]->SSR.SSR;
	u32Reg = u32Reg & (~eSlaveSel);
	SPI_PORT[eSpiPort]->SSR.SSR = u32Reg;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_Busy                                                                                             */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  The SPI port is in busy.                                                                                */
/*    FALSE: The SPI port is not in busy.                                                                            */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Check the busy status of the specified SPI port.                                                               */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_Busy(E_DRVSPI_PORT eSpiPort)
{
    return ((SPI_PORT[eSpiPort]->CNTRL.GO_BUSY)?TRUE:FALSE);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_BurstTransfer                                                                                    */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort    [in]: Specify the SPI port                                                                         */
/*    i32BurstCnt [in]: Specify the transaction number in one transfer. It could be 1 or 2.                          */
/*    i32Interval [in]: Specify the delay clocks between successive transactions. It could be 2~17.                  */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    E_DRVSPI_ERR_BURST_CNT: The transaction number is out of range.                                                */
/*    E_DRVSPI_ERR_TRANSMIT_INTERVAL: The suspend interval setting is out of range.                                  */
/*    E_SUCCESS: Success.                                                                                            */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Configure the burst transfer settings.                                                                         */
/*-------------------------------------------------------------------------------------------------------------------*/
ERRCODE DrvSPI_BurstTransfer(E_DRVSPI_PORT eSpiPort, int32_t i32BurstCnt, int32_t i32Interval)
{

	if((i32BurstCnt < 1) || (i32BurstCnt > 2))
	{
		return E_DRVSPI_ERR_BURST_CNT;
	}
	
	if((i32Interval < 2) || (i32Interval > 17))
	{
		return E_DRVSPI_ERR_TRANSMIT_INTERVAL;
	}

	SPI_PORT[eSpiPort]->CNTRL.TX_NUM = i32BurstCnt-1;
	SPI_PORT[eSpiPort]->CNTRL.SLEEP = i32Interval-2;

	return E_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetClock                                                                                         */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort  [in]: Specify the SPI port                                                                           */
/*    u32Clock1 [in]: Specify the SPI clock rate in Hz. It's the clock rate of SPI base clock or variable clock 1.   */
/*    u32Clock2 [in]: Specify the SPI clock rate in Hz. It's the clock rate of variable clock 2.                     */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    The actual value of divisor 1 is returned. SPI engine clock rate: APB clock rate / ((divisor 1 + 1) * 2)       */
/*    The actual clock may be different to the target SPI clock due to hardware limitation.                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Configure the SPI clock.                                                                                       */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_SetClock(E_DRVSPI_PORT eSpiPort, uint32_t u32Clock1, uint32_t u32Clock2)
{
    uint32_t u32Div;
    uint32_t u32Pclk;
    
    u32Pclk = DrvSYS_GetHCLK() * 1000;
    
	if(u32Clock2!=0)
	{
		u32Div = (((u32Pclk / u32Clock2) + 1) >> 1) - 1;
		if(u32Div > 65535)
		    u32Div = 65535;
		if(u32Div < 1)
		    u32Div = 1;
		SPI_PORT[eSpiPort]->DIVIDER.DIVIDER2 = u32Div;
	}
	
	if(u32Clock1!=0)
	{
		u32Div = ((((2*u32Pclk) / (u32Clock1)) + 1) >> 2) - 1;
		if(u32Div > 65535)
		    u32Div = 65535;
		if(u32Div < 1)
		    u32Div = 1;
		SPI_PORT[eSpiPort]->DIVIDER.DIVIDER = u32Div;
		u32Div = u32Pclk / ((u32Div+1)*2);
		return u32Div;
	}
	else
		return 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetClock1                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    The current SPI bus clock frequency in Hz.                                                                     */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get the SPI engine clock rate in Hz.                                                                           */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_GetClock1(E_DRVSPI_PORT eSpiPort)
{
	uint32_t u32Div;
	uint32_t u32ApbClock;

	u32ApbClock = DrvSYS_GetHCLK() * 1000;
	u32Div = SPI_PORT[eSpiPort]->DIVIDER.DIVIDER;
	return ((u32ApbClock >> 1) / (u32Div + 1));   /* SPI_CLK = APB_CLK / ((Divider + 1) * 2) */
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetClock2                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    The frequency of variable clock 2 in Hz.                                                                       */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get the clock rate of variable clock 2 in Hz.                                                                  */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_GetClock2(E_DRVSPI_PORT eSpiPort)
{
	uint32_t u32Div;
	uint32_t u32ApbClock;

	u32ApbClock = DrvSYS_GetHCLK() * 1000;
	u32Div = SPI_PORT[eSpiPort]->DIVIDER.DIVIDER2;
	return ((u32ApbClock >> 1) / (u32Div + 1));   /* SPI_CLK = APB_CLK / ((Divider + 1) * 2) */
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetVariableClockPattern                                                                          */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*    u32Pattern [in]: Specify the variable clock pattern                                                            */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    If the bit pattern of VARCLK is ．0・, the output frequency of SPICLK is according to the value of DIVIDER.      */
/*    If the bit pattern of VARCLK is ．1・, the output frequency of SPICLK is according to the value of DIVIDER2.     */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetVariableClockPattern(E_DRVSPI_PORT eSpiPort, uint32_t u32Pattern)
{
	SPI_PORT[eSpiPort]->VARCLK = u32Pattern;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetVariableClockFunction                                                                         */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*    bEnable    [in]: TRUE -- Enable variable clock;                                                                */
/*                     FALSE -- Disable variable clock.                                                              */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Set the variable clock function.                                                                               */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetVariableClockFunction(E_DRVSPI_PORT eSpiPort, BOOL bEnable)
{
	if(bEnable)
		SPI_PORT[eSpiPort]->CNTRL.VARCLK_EN = 1;
	else
		SPI_PORT[eSpiPort]->CNTRL.VARCLK_EN = 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_EnableInt                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort    [in]: Specify the SPI port                                                                         */
/*    pfnCallback [in]: The callback function of SPI interrupt.                                                      */
/*    u32UserData [in]: The parameter which will be passed to the callback function.                                 */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Enable the SPI interrupt of the specified SPI port and install the callback function.                          */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_EnableInt(E_DRVSPI_PORT eSpiPort, PFN_DRVSPI_CALLBACK pfnCallback, uint32_t u32UserData)
{
	
	if(pfnCallback != NULL)
	{
		g_sSpiHandler[eSpiPort].pfncallback = pfnCallback;
		g_sSpiHandler[eSpiPort].u32userData = u32UserData;
	}
	
	SPI_PORT[eSpiPort]->CNTRL.IE = 1;
	
	if(eSpiPort == SPI_PORT0)
	{
	    NVIC_EnableIRQ(SPI0_IRQn);
	}
	
}	

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_DisableInt                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Disable the SPI interrupt.                                                                                     */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_DisableInt(E_DRVSPI_PORT eSpiPort)
{
	g_sSpiHandler[eSpiPort].pfncallback = NULL;
	g_sSpiHandler[eSpiPort].u32userData = 0;
	
	SPI_PORT[eSpiPort]->CNTRL.IE = 0;

	if(eSpiPort == SPI_PORT0)
	{
	    NVIC_DisableIRQ(SPI0_IRQn);
	}

}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SingleReadWrite                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]:  Specify the SPI port                                                                         */
/*    pu32DataIn [in]:  Write data to the SPI bus.                                                         */
/*    pu32Data   [out]: Store the data got from the SPI bus.                                                         */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  The data stored in pu32Data is valid.                                                                   */
/*    FALSE: The data stored in pu32Data is invalid.                                                                 */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Read data from SPI Rx registers and trigger SPI for next transfer.                                             */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_SingleReadWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data, uint32_t pu32DataIn)
{
	if(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY==1)
		return FALSE;
	SPI_PORT[eSpiPort]->TX[0] = pu32DataIn;
	*pu32Data = SPI_PORT[eSpiPort]->RX[0];
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;
	return TRUE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SingleRead                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]:  Specify the SPI port                                                                         */
/*    pu32Data   [out]: Store the data got from the SPI bus.                                                         */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  The data stored in pu32Data is valid.                                                                   */
/*    FALSE: The data stored in pu32Data is invalid.                                                                 */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Read data from SPI Rx registers and trigger SPI for next transfer.                                             */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_SingleRead(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data)
{
	if(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY==1)
		return FALSE;

	*pu32Data = SPI_PORT[eSpiPort]->RX[0];
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;
	return TRUE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SingleWrite                                                                                      */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort  [in]:  Specify the SPI port                                                                          */
/*    pu32Data  [in]:  Store the data which will be transmitted through the SPI bus.                                 */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  The data stored in pu32Data has been transferred.                                                       */
/*    FALSE: The SPI is in busy. The data stored in pu32Data has not been transferred.                               */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Write data to SPI bus and trigger SPI to start transfer.                                                       */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_SingleWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Data)
{
	if(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY==1)
		return FALSE;

	SPI_PORT[eSpiPort]->TX[0] = *pu32Data;
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;
	return TRUE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_BurstRead                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort  [in]:  Specify the SPI port                                                                          */
/*    pu32Buf   [out]: Store the data got from the SPI bus.                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE: The data stored in pu32Buf is valid.                                                                     */
/*    FALSE: The data stored in pu32Buf is invalid.                                                                  */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Read two words of data from SPI Rx registers and then trigger SPI for next transfer.                           */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_BurstRead(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf)
{
	if(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY==1)
		return FALSE;

	pu32Buf[0] = SPI_PORT[eSpiPort]->RX[0];
	pu32Buf[1] = SPI_PORT[eSpiPort]->RX[1];
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;

	return TRUE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_BurstWrite                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort  [in]:  Specify the SPI port                                                                          */
/*    pu32Buf   [in]:  Store the data which will be transmitted through the SPI bus.                                 */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE: The data stored in pu32Buf has been transferred.                                                         */
/*    FALSE: The SPI is in busy. The data stored in pu32Buf has not been transferred.                                */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Write two words of data to SPI bus and then trigger SPI to start transfer.                                     */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_BurstWrite(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf)
{
	if(SPI_PORT[eSpiPort]->CNTRL.GO_BUSY==1)
		return FALSE;

	SPI_PORT[eSpiPort]->TX[0] = pu32Buf[0];
	SPI_PORT[eSpiPort]->TX[1] = pu32Buf[1];
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;

	return TRUE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_DumpRxRegister                                                                                   */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]:  Specify the SPI port                                                                       */
/*    pu32Buf      [out]: Store the data got from Rx registers.                                                      */
/*    u32DataCount [in]:  The count of data read from Rx registers.                                                  */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    The count of data actually read from Rx registers.                                                             */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Read data from Rx registers. This function will not trigger another data transfer.                             */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_DumpRxRegister(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf, uint32_t u32DataCount)
{
	uint32_t i;
	
	if(u32DataCount>2)
		u32DataCount = 2;
	
	for(i=0; i<u32DataCount; i++)
	{
		pu32Buf[i] = SPI_PORT[eSpiPort]->RX[i];
	}

    return u32DataCount;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetTxRegister                                                                                    */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]:  Specify the SPI port                                                                       */
/*    pu32Buf      [in]:  Store the data which will be written to Tx registers.                                      */
/*    u32DataCount [in]:  The count of data write to Tx registers.                                                   */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    The count of data actually written to Tx registers.                                                            */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Write data to Tx registers. This function will not trigger another data transfer.                              */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_SetTxRegister(E_DRVSPI_PORT eSpiPort, uint32_t *pu32Buf, uint32_t u32DataCount)
{
	uint32_t i;

	if(u32DataCount>2)
		u32DataCount = 2;
	
	for(i=0; i<u32DataCount; i++)
	{
		SPI_PORT[eSpiPort]->TX[i] = pu32Buf[i];
	}

    return u32DataCount;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetGo                                                                                            */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]:  Specify the SPI port                                                                       */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Set the GO_BUSY bit to trigger a SPI data trasfer.                                                             */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetGo(E_DRVSPI_PORT eSpiPort)
{
	SPI_PORT[eSpiPort]->CNTRL.GO_BUSY = 1;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetJoyStickIntType                                                                               */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]:  Specify the SPI port                                                                       */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    eDRVSPI_JOYSTICK_DATA_READY: 8-byte data available in the buffer.                                              */
/*    eDRVSPI_JOYSTICK_CS_ACTIVE:  Chip Select is actived.                                                           */
/*    eDRVSPI_JOYSTICK_CS_DEACT:   Chip Select is de-actived.                                                        */
/*    eDRVSPI_JOYSTICK_NONE:       None.                                                                             */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get interrupt flag of JOYSTICK mode.                                                                           */
/*-------------------------------------------------------------------------------------------------------------------*/
E_DRVSPI_JOYSTICK_INT_FLAG DrvSPI_GetJoyStickIntType(E_DRVSPI_PORT eSpiPort)
{
	if(SPI_PORT[eSpiPort]->JS.DATA_RDY == 1)
		return eDRVSPI_JOYSTICK_DATA_READY;
	
	if(SPI_PORT[eSpiPort]->JS.CS_ACT == 1)
		return eDRVSPI_JOYSTICK_CS_ACTIVE;
	
	if(SPI_PORT[eSpiPort]->JS.CS_DEACT == 1)
		return eDRVSPI_JOYSTICK_CS_DEACT;
	
	return eDRVSPI_JOYSTICK_NONE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetJoyStickStatus                                                                                */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]: Specify the SPI port                                                                        */
/*    bReady       [in]: TRUE  -- The SPI is ready to transfer data.                                                 */
/*                       FALSE -- The SPI is not ready to transfer data.                                             */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Set the JoyStick status to ready or not ready.                                                                 */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetJoyStickStatus(E_DRVSPI_PORT eSpiPort, BOOL bReady)
{
	if(bReady)
		SPI_PORT[eSpiPort]->JS.READYB = 0;
	else
		SPI_PORT[eSpiPort]->JS.READYB = 1;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetJoyStickMode                                                                                  */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]:  Specify the SPI port                                                                       */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    eDRVSPI_JOYSTICK_TRANSMIT_MODE: Master writes data to slave.                                                   */
/*    eDRVSPI_JOYSTICK_RECEIVE_MODE:  Master read data from slave.                                                   */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get the JoyStick operation mode.                                                                               */
/*-------------------------------------------------------------------------------------------------------------------*/
E_DRVSPI_JOYSTICK_RW_MODE DrvSPI_GetJoyStickMode(E_DRVSPI_PORT eSpiPort)
{
	if( (SPI_PORT[eSpiPort]->JS.JS_RW)==0 )
		return eDRVSPI_JOYSTICK_TRANSMIT_MODE;
	else
		return eDRVSPI_JOYSTICK_RECEIVE_MODE;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_StartPDMA                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort     [in]: Specify the SPI port                                                                        */
/*    eDmaMode     [in]: Specify the DMA mode.                                                                       */
/*    bEnable      [in]: TRUE  -- Enable DMA;                                                                        */
/*                       FALSE -- Disable DMA.                                                                       */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Configure the DMA settings.                                                                                    */
/*-------------------------------------------------------------------------------------------------------------------*/
void DrvSPI_StartPDMA(E_DRVSPI_PORT eSpiPort, E_DRVSPI_DMA_MODE eDmaMode, BOOL bEnable)
{
	if(eDmaMode==eDRVSPI_TX_DMA)
	{
		if(bEnable)
			SPI_PORT[eSpiPort]->DMA.TX_DMA_GO = 1;
		else
			SPI_PORT[eSpiPort]->DMA.TX_DMA_GO = 0;
	}
	else
	{
		if(bEnable)
			SPI_PORT[eSpiPort]->DMA.RX_DMA_GO = 1;
		else
			SPI_PORT[eSpiPort]->DMA.RX_DMA_GO = 0;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_SetFIFOMode                                                                            */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*    eSpiPort     [in]: Specify the SPI port                                                              */
/*    bEnable      [in]: Enable (TRUE) / Disable (FALSE)                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable/disable FIFO mode.                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void DrvSPI_SetFIFOMode(E_DRVSPI_PORT eSpiPort, BOOL bEnable)
{
	if(bEnable)
		SPI_PORT[eSpiPort]->CNTRL.FIFO = 1;
	else
		SPI_PORT[eSpiPort]->CNTRL.FIFO = 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_IsRxEmpty                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  Rx buffer empty.                                                                                        */
/*    FALSE: Rx buffer is not empty.                                                                                 */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Check the status of the Rx buffer of the specified SPI port.                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_IsRxEmpty(E_DRVSPI_PORT eSpiPort)
{
    return ((SPI_PORT[eSpiPort]->CNTRL.RX_EMPTY)?TRUE:FALSE);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_IsRxFull                                                                                         */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  Rx buffer full.                                                                                         */
/*    FALSE: Rx buffer is not full.                                                                                  */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Check the status of the Rx buffer of the specified SPI port.                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_IsRxFull(E_DRVSPI_PORT eSpiPort)
{
    return ((SPI_PORT[eSpiPort]->CNTRL.RX_FULL)?TRUE:FALSE);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_IsTxEmpty                                                                                        */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  Tx buffer empty.                                                                                        */
/*    FALSE: Tx buffer is not empty.                                                                                 */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Check the status of the Tx buffer of the specified SPI port.                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_IsTxEmpty(E_DRVSPI_PORT eSpiPort)
{
    return ((SPI_PORT[eSpiPort]->CNTRL.TX_EMPTY)?TRUE:FALSE);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_IsTxFull                                                                                         */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    eSpiPort   [in]: Specify the SPI port                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    TRUE:  Tx buffer full.                                                                                         */
/*    FALSE: Tx buffer is not full.                                                                                  */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Check the status of the Tx buffer of the specified SPI port.                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
BOOL DrvSPI_IsTxFull(E_DRVSPI_PORT eSpiPort)
{
    return ((SPI_PORT[eSpiPort]->CNTRL.TX_FULL)?TRUE:FALSE);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_GetVersion                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    Version number.                                                                                                */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get the version number of NUC100 SPI driver.                                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvSPI_GetVersion(void)
{
	return DRVSPI_VERSION_NUM;
}
