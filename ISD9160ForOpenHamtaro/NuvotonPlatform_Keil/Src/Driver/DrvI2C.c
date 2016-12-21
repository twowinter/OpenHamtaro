/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "DrvI2C.h"


#define I2C_STA              0x20
#define I2C_STO              0x10
#define I2C_SI               0x08
#define I2C_AA               0x04

static I2C_CALLBACK_T I2CHandler0 = {0};

		  

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <I2C0_IRQHandler>                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle I2C0 interrupt event           		                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32status;

  	u32status  = I2C0->STATUS;

    if (I2C0->TOC.TIF)
	{
		I2C0->TOC.TIF = 1;	/* Clear TIF */
       	
		if (I2CHandler0.TimeoutCallBackFn)
		{
			I2CHandler0.TimeoutCallBackFn(u32status); 
		}
	}
	else
	{
		switch (u32status)
    	{	
     		case 0x38:	/* Arbitration loss */
     		{
        		if (I2CHandler0.ArbitLossCallBackFn)
					I2CHandler0.ArbitLossCallBackFn(u32status); 
     	    	break;
     		}
 			case 0x00: 	/* Bus error */
			{
        		if (I2CHandler0.BusErrCallBackFn)
					I2CHandler0.BusErrCallBackFn(u32status); 
     	    	break;
			}
			default:
			{
        		if (I2CHandler0.I2CCallBackFn)
					I2CHandler0.I2CCallBackFn(u32status); 			
			}		
		}
		
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_Ctrl				                                                   			   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0                      	       		                       */
/*               u8start 	- [in]		1:Enable / 0:Disable	       		           				  	   */
/*               u8stop 	- [in]		1:Enable / 0:Disable	       		                           	   */
/*               u8intFlag 	- [in]		Wrtie '1' to clear this flag	       		                       */
/*               u8ack 		- [in]		1:Enable / 0:Disable	       		                               */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set I2C control bit										           		   			   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvI2C_Ctrl(E_I2C_PORT port, uint8_t u8start, uint8_t u8stop, uint8_t u8intFlag, uint8_t u8ack)
{
	
	uint32_t Reg = 0;
		
	if (u8start)
		Reg |= I2C_STA;
	if (u8stop)
	    Reg |= I2C_STO;
	if (u8intFlag)
		Reg |= I2C_SI;
	if (u8ack)
		Reg |= I2C_AA;

	if (port)
		;
	else
		*((__IO uint32_t *)&I2C0->CON) = (*((__IO uint32_t *)&I2C0->CON) & ~0x3C) | Reg;	    	
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_WriteData				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 	       		                                           */
/*               u8data 	- [in]		Data	       		                               				   */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Write Data into I2C Data register										           		   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvI2C_WriteData(E_I2C_PORT port, uint8_t u8data)
{
	if (port)
	{						   
		;
	}	
	else
	{
		I2C0->DATA = u8data;	
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_ReadData				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 	       		                                           */
/* Returns:                                                                                                */
/*               Data from I2C Data register											       		   	   */
/*                                                                                      			       */
/* Description:                                                                                            */
/*               Read Data from I2C Data register										               	   */		
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DrvI2C_ReadData(E_I2C_PORT port)
{
	if (port)
	{						   
		return 0;
	}	
	else
	{
		return I2C0->DATA;	
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_GetIntFlag				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 	       		                                           */
/* Returns:                                                                                                */
/*               0 : Flag is not set											                    	   */
/*               1 : Flag is set							                                               */
/* Description:                                                                                            */
/*               Get I2C interrupt flag										               				   */		
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DrvI2C_GetIntFlag(E_I2C_PORT port)
{
	if (port)
	{						   
		return 0;
	}	
	else
	{
		return I2C0->CON.SI;	
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_ClearIntFlag				                                                   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 	       		                               			   */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Clear I2C interrupt flag										       					   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvI2C_ClearIntFlag(E_I2C_PORT port)
{
	if (port)
	{						   
		;
	}	
	else
	{
		I2C0->CON.SI = 1;	
	}	    	
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_ClearTimeoutFlag				                                                   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 	       		                               			   */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Clear I2C time out flag										          				   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvI2C_ClearTimeoutFlag(E_I2C_PORT port)
{
	if (port)
	{						   
		;
	}	
	else
	{
		I2C0->TOC.TIF = 1;	
	}	    	
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_GetStatus				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0           	       		                               */
/* Returns:                                                                                                */
/*               status											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Get I2C status										               						   */		
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvI2C_GetStatus(E_I2C_PORT port)
{
	if (port)
	{
		return 0;
	}
	else
	{
		return I2C0->STATUS;// 0x4002_000C
	}	
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_Open				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/*               u32clock_Hz 	   - [in]		I2C clock source ftequency (Hz)       		               */
/*               u32baudrate_Hz   - [in]		I2C bit rate (Hz)       		                           */
/* Returns:                                                                                                */
/*               0 : Success											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable I2C function and set clock divider. I2C clock = PCLK / (4x(divider+1))	           */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_Open(E_I2C_PORT port, uint32_t u32clock_Hz, uint32_t u32baudrate_Hz)
{
    uint32_t u32divider;     
 
 	u32divider = (uint32_t) (((u32clock_Hz*16)/(u32baudrate_Hz * 4) + 8) / 16 - 1);	
	/* Compute proper divider for I2C clock - scale up by 16, add 8 to round, scale back by 16 */
	
    if (port)
	{
		;
	}
	else
	{
		SYSCLK->APBCLK.I2C0_EN = 1;
		SYS->IPRSTC2.I2C0_RST = 1;
		SYS->IPRSTC2.I2C0_RST = 0;	
		I2C0->CON.ENSI = 1;
		I2C0->CLK = u32divider;
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_Close				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		- [in]		I2C_PORT0 				       		                               */
/* Returns:                                                                                                */
/*               0 : Success											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable I2C function and clock source										               */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_Close(E_I2C_PORT port)
{
    if (port)
	{
		;
	}
	else
	{
		I2C0->CON.ENSI = 0;
		SYS->IPRSTC2.I2C0_RST = 1;
		SYS->IPRSTC2.I2C0_RST = 0;
		SYSCLK->APBCLK.I2C0_EN = 0;
	}
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_SetClock				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0                      	                               */
/*               u32clock_kHz 	  - [in]	I2C clock source ftequency (KHz)       		                   */
/*               u32baudrate_kHz  - [in]	I2C bit rate (KHz)       		                               */
/* Returns:                                                                                                */
/*               0 : Success 											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set clock divider. I2C clock = PCLK / (4x(divider+1))		       						   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_SetClock(E_I2C_PORT port, uint32_t u32clock_Hz, uint32_t u32baudrate_Hz)
{
    uint32_t u32divider;     
 
 	u32divider = (uint32_t) (((u32clock_Hz*16)/(u32baudrate_Hz * 4) + 8) / 16 - 1);	
	/* Compute proper divider for I2C clock - scale up by 16, add 8 to round, scale back by 16 */

    if (port)
	{
		;
	}
	else
	{
		I2C0->CLK = u32divider;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_GetClock				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/*               clock_kHz 	   - [in]		I2C clock source ftequency (KHz)       		                   */
/* Returns:                                                                                                */
/*               I2C bit rate 											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Get I2C bit rate										           						   */		
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvI2C_GetClock(E_I2C_PORT port, uint32_t u32clock)
{
    uint32_t u32divider;     
 
	u32divider = ( (port)? 0:I2C0->CLK );
	return ( u32clock / ((u32divider+1)<<2) );
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_EnableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/* Returns:                                                                                                */
/*               0 : Success  											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable I2C interrupt and NVIC corresponding to I2C		        						   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_EnableInt(E_I2C_PORT port)
{
    if (port)
	{
		;
	}
	else
	{
	    I2C0->CON.EI = 1;
		NVIC_EnableIRQ(I2C0_IRQn);
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_DisableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/* Returns:                                                                                                */
/*               0 : Success  											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable I2C interrupt and NVIC corresponding to I2C		        					   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_DisableInt(E_I2C_PORT port)
{
    if (port)
	{
		;
	}
	else
	{
	    I2C0->CON.EI = 0;
		NVIC_DisableIRQ(I2C0_IRQn);
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_EnableTimeoutCount				                                                   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/*               i32enable 	   - [in]		1:Enable / 0:Disable	       		                           */
/*               u8div4 	   - [in]		1:Enable / 0:Disable	       		                           */
/* Returns:                                                                                                */
/*               0 : Success  											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable I2C 14-bit timeout counter and set div4 bit of timeout counter					   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_EnableTimeoutCount(E_I2C_PORT port, int32_t i32enable, uint8_t u8div4)
{
    if (port)
	{
		;
	}
	else
	{
	    I2C0->TOC.DIV4 = u8div4;
		I2C0->TOC.ENTI = i32enable;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_SetAddress				                                                   	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0 				       		                           */
/*               u8slaveNo 	   - [in]		There are 4 slave addrss. The slaveNo is 0 ~ 3.        		   */
/*               u8slave_addr  - [in]		7-bit data	       		                           			   */
/*               u8GC_Flag 	   - [in]		1:Enable / 0:Disable	       		                           */
/* Returns:                                                                                                */
/*               0 : Success 											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set 4 7-bit slave addresses and enable General Call function		        			   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_SetAddress(E_I2C_PORT port, uint8_t u8slaveNo, uint8_t u8slave_addr, uint8_t u8GC_Flag)
{
	if (port)
	{
		;
	}
	else
	{
		switch (u8slaveNo)
		{
			case 0:
				I2C0->ADDR0.ADDR  = u8slave_addr;
				I2C0->ADDR0.GC    = u8GC_Flag;
				break;
			case 1:
				I2C0->ADDR1.ADDR = u8slave_addr;
				I2C0->ADDR1.GC   =	u8GC_Flag;
				break;
			case 2:
				I2C0->ADDR2.ADDR = u8slave_addr;
				I2C0->ADDR2.GC   =	u8GC_Flag;
				break;
			case 3:
				I2C0->ADDR3.ADDR = u8slave_addr;
				I2C0->ADDR3.GC   =	u8GC_Flag;
				break;
			default:
				return E_DRVI2C_ERR_ARGUMENT;
		}	
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_SetAddressMask				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0                      	       		                   */
/*               u8slaveNo 	   - [in]		There are 4 slave addrss. The slaveNo is 0 ~ 3.        		   */
/*               u8slaveAddrMask - [in]		7-bit data	       		                           			   */
/*               									      		                           				   */
/* Returns:                                                                                                */
/*               0 : Success											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set 4 7-bit slave address mask. The corresponding address bit is "Don't Care".			   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_SetAddressMask(E_I2C_PORT port, uint8_t u8slaveNo, uint8_t u8slaveAddrMask)
{
	if (port)
	{
		;
	}
	else
	{
		switch (u8slaveNo)
		{
			case 0:
				I2C0->ADRM0.ADM = u8slaveAddrMask;
				break;
			case 1:
				I2C0->ADRM1.ADM = u8slaveAddrMask;
				break;
			case 2:
				I2C0->ADRM2.ADM = u8slaveAddrMask;
				break;
			case 3:
				I2C0->ADRM3.ADM = u8slaveAddrMask;
				break;
			default:
				return E_DRVI2C_ERR_ARGUMENT;
		}	
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_InstallCallback				                                               	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0                      	       		                   */
/*               Type 	   	   - [in]		I2CFUNC / ARBITLOSS / BUSERROR / TIMEOUT      				   */
/*               callbackfn    - [in]		Call back function	       		                           	   */
/*               									      		                           				   */
/* Returns:                                                                                                */
/*               0		 Success																		   */
/*               <0      Failed 											                    		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Install I2C call back function for I2C normal function, Arbitration loss, Bus error	   */
/*                                                  and Counter timeout .			   					   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_InstallCallback(E_I2C_PORT port, E_I2C_CALLBACK_TYPE Type, I2C_CALLBACK callbackfn)
{
	if (port == I2C_PORT0)
	{	
		switch(Type)
    	{
        	case I2CFUNC:
        	{
            	I2CHandler0.I2CCallBackFn = callbackfn;  
        		break;
        	}
        	case ARBITLOSS:
        	{
            	I2CHandler0.ArbitLossCallBackFn = callbackfn;  
        		break;
        	}
        	case BUSERROR:
        	{
            	I2CHandler0.BusErrCallBackFn = callbackfn;  
        		break;
        	}        
        	case TIMEOUT:
        	{
            	I2CHandler0.TimeoutCallBackFn = callbackfn;  
        		break;
        	}
			default:
				return E_DRVI2C_ERR_ARGUMENT;                             	
		}
	}
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_UninstallCallBack				                                               	   	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               port 		   - [in]		I2C_PORT0                      		                           */
/*               Type 	   	   - [in]		I2CFUNC / ARBITLOSS / BUSERROR / TIMEOUT      				   */
/*                     		                           	   												   */
/* Returns:                                                                                                */
/*               0		 Success																		   */
/*               <0      Failed 											                    		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Uninstall I2C call back function for I2C normal function, Arbitration loss, Bus error	   */
/*                                                  and Counter timeout .			   					   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_UninstallCallBack(E_I2C_PORT port, E_I2C_CALLBACK_TYPE Type)
{
	if (port == I2C_PORT0)
	{ 
		switch(Type)
    	{
        	case I2CFUNC:
        	{
            	I2CHandler0.I2CCallBackFn = 0;  
        		break;
        	}
        	case ARBITLOSS:
        	{
            	I2CHandler0.ArbitLossCallBackFn = 0;  
        		break;
        	}
        	case BUSERROR:
        	{
            	I2CHandler0.BusErrCallBackFn = 0;  
        		break;
        	}        
        	case TIMEOUT:
        	{
            	I2CHandler0.TimeoutCallBackFn = 0;  
        		break;
        	}
			default:
				return E_DRVI2C_ERR_ARGUMENT;                             	
		}	                  	
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               None	                                                         						   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               The DrvI2C version number                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to get the DrvI2C version number                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvI2C_GetVersion(void)
{
	return DRVI2C_VERSION_NUM;
}

