/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Include related headers                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#include "ISD9xx.h"
#include "core_cm0.h"
#include "DrvCRC.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------*/
/* Function: DrvCRC_Open                                                                                        */
/*                                                                                                              */
/* Parameters:        	                                                                                        */
/*                                                                                                              */
/* Returns:                                                                                                     */
/*         E_DRVCRC_ERR_PACKET_TOO_LONG: The specified parameters are invalid                                   */
/*         E_SUCCESS: Success.                                                                                  */
/*                                                                                                              */
/* Description:                                                                                                 */
/*       Enable CRC Generator                                                                                   */
/*--------------------------------------------------------------------------------------------------------------*/
ERRCODE DrvCRC_Open(void)
{
	SYSCLK->APBCLK.CRC_EN        =1;
    SYS->IPRSTC2.CRC_RST         =1;
    SYS->IPRSTC2.CRC_RST         =0;

    return E_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------------------*/
/* Function: DrvCRC_Init                                                                                        */
/*                                                                                                              */
/* Parameters:        	                                                                                        */
/*         eLSB         [in]: Specify LSB/MSB mode                                                              */
/*         i32PacketLen [in]: Specify length of packet to be processed.                                         */
/*                                                                                                              */
/* Returns:                                                                                                     */
/*         E_DRVCRC_ERR_PACKET_TOO_LONG: The specified parameters are invalid                                   */
/*         E_SUCCESS: Success.                                                                                  */
/*                                                                                                              */
/* Description:                                                                                                 */
/*       Initialize CRC for new packet                                                                          */
/*--------------------------------------------------------------------------------------------------------------*/
ERRCODE DrvCRC_Init(E_DRVCRC_ENDIAN eLSB, int32_t i32PacketLen)
{
   
    CRC->CRC_EN.LSB = eLSB;
    if(i32PacketLen > 512)
        return E_DRVCRC_ERR_PACKET_TOO_LONG;
    if(i32PacketLen%2==1)
        return E_DRVCRC_ERR_ODD_PACKET;
    CRC->CRC_EN.LEN = i32PacketLen-1; 

    return E_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------------------*/
/* Function: DrvCRC_Calc                                                                                        */
/*                                                                                                              */
/* Parameters:        	                                                                                        */
/*         Data   [in]: Pointer to data. Word Aligned                                                              */
/*         i32PacketLen [in]: Specify length of packet to be processed.                                         */
/*                                                                                                              */
/* Returns:                                                                                                     */
/*         CRC Calculation of data.                                                                             */
/*         E_SUCCESS: Success.                                                                                  */
/*                                                                                                              */
/* Description:                                                                                                 */
/*       Process the data pointed to by Initialize CRC for new packet                                                                          */
/*--------------------------------------------------------------------------------------------------------------*/
int16_t DrvCRC_Calc( uint32_t *Data, int32_t i32PacketLen)
{
   
    while( i32PacketLen > 0){
		CRC->CRC_IN = *Data++;
		i32PacketLen-=4;
	}

    return CRC->CRC_OUT;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvSPI_Close                                                                                  */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Close the CRC Generator                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DrvCRC_Close(void)
{

		SYS->IPRSTC2.CRC_RST=1;
		SYS->IPRSTC2.CRC_RST=0;
		SYSCLK->APBCLK.CRC_EN=0;
   
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


/*-------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvCRC_GetVersion                                                                                       */
/*                                                                                                                   */
/* Parameters:        	                                                                                             */
/*    None.                                                                                                          */
/*                                                                                                                   */
/* Returns:                                                                                                          */
/*    Version number.                                                                                                */
/*                                                                                                                   */
/* Description:                                                                                                      */
/*    Get the version number of CRC driver.                                                                   */
/*-------------------------------------------------------------------------------------------------------------------*/
uint32_t DrvCRC_GetVersion(void)
{
	return DRVCRC_VERSION_NUM;
}
