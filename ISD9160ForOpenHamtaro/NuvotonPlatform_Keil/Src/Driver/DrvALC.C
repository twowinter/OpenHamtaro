/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "DrvALC.h"
#include "DrvSYS.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetREG				                                                   			   	       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u32Value 		- [in]		Register value, write directly into ALC_REG 	               */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set ALC control register										           		   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetREG(uint32_t u32Value)
{
    M32(&ALC->ALC_CTRL) = u32Value;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetNGTH				                                                        		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               000 = -81dB        001 = -75dB        010 = -69dB        011 = -63dB	                   */
/*               100 = -57dB        101 = -51dB        110 = -45dB        111 = -39dB	                   */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG NGTH										                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetNGTH(uint8_t u8NGTH)
{
    ALC->ALC_CTRL.NGTH     = u8NGTH & 0x07; //ALC_REG bit[2:0]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetNGEN				                                                        		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               0: Noise gate disable	                                                                   */
/*               1: Noise gate enable	                                                                   */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG NGEN										                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetNGEN(uint8_t u8NGEN)
{
    ALC->ALC_CTRL.NGEN     = u8NGEN & 0x01; //ALC_REG bit[3]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_EnableNoiseGate				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Noise gate peak detector select   0: use P2P value for noise gate threshold determination */
/*               	                               1: use absolute peak value for NGTH                     */
/*               u8NGENval : 1 = Enable, 0 = Disable 											           */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Enable Noise Gate in normal mode only										               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_EnableNoiseGate(uint8_t u8NGSEL, uint8_t u8NGENval)
{
	if( u8NGENval )
{
    ALC->ALC_CTRL.ALCSEL   = 1; //ALC_REG bit[28]
    ALC->ALC_CTRL.ALCMODE  = 0; //ALC_REG bit[12]
    ALC->ALC_CTRL.NGEN     = 1; //ALC_REG bit[3]
    ALC->ALC_CTRL.ALCNGSEL = u8NGSEL & 0x01; //ALC_REG bit[29]
}
    else
        ALC->ALC_CTRL.NGEN     = 0; //ALC_REG bit[3]
        
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetZeroCrossing				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               0: Zero Crossing disable	                                                               */
/*               1: Zero Crossing enable	                                                               */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCZC; Only relevant when the ALC is enabled                                  */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetZeroCrossing(uint8_t u8ALCZC)
{
    ALC->ALC_CTRL.ALCZC     = u8ALCZC & 0x01; //ALC_REG bit[21]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetAttackTime				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Range N = 0~10 Steps          Normal mode (500us~512ms):  Time = 500us * 2^N 	           */
/*               	                           Limiter mode(125us~128ms):  Time = 125us * 2^N              */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCATK, ALC attack time                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetAttackTime(uint32_t u32ATKstep)
{
    ALC->ALC_CTRL.ALCATK     = u32ATKstep; //ALC_REG bit[7:4]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetDecayTime				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Range N = 0~10 Steps          Normal mode (125us~128ms):  Time = 125us * 2^N 	           */
/*               	                           Limiter mode( 31us~ 32ms):  Time =  31us * 2^N              */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCDCY, ALC decay time                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetDecayTime(uint32_t u32DCYstep)
{
    ALC->ALC_CTRL.ALCDCY     = u32DCYstep; //ALC_REG bit[11:8]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetMode				                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               0: Normal mode	                                                                           */
/*               1: Peak limiter mode	                                                                   */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCMODE;                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetMode(uint8_t u8Mode)
{
    ALC->ALC_CTRL.ALCMODE     = u8Mode & 0x01; //ALC_REG bit[12]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetTargetLevel				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Steps N   =    0   ~  15	          level = -28.5 + 1.5*N (dB)                           */
/*               Range From -28.5dB ~ -6dB                                                                 */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCLVL;                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetTargetLevel(uint32_t u32Level)
{
    ALC->ALC_CTRL.ALCLVL     = u32Level; //ALC_REG bit[16:13]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetHoldTime				                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Steps N   =    0   ~  10	          Time = 0 + 2^N (ms) (N>0)                            */
/*               Range From     0ms ~   1sec                                                               */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCHLD;                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetHoldTime(uint32_t u32HoldTime)
{
    ALC->ALC_CTRL.ALCHLD     = u32HoldTime; //ALC_REG bit[20:17]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetMinGain				                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Steps N   =    0   ~  7	          Minimum level = -12 + 6*N (dB)                       */
/*               Range From   -12dB ~ 30dB                                                                 */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCMIN;                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetMinGain(uint32_t u32MinGain)
{
    ALC->ALC_CTRL.ALCMIN     = u32MinGain; //ALC_REG bit[24:22]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetMaxGain				                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Steps N   =      0   ~  7	          Maximum level = -6.75 + 6*N (dB)                     */
/*               Range From   -6.75dB ~ 35.25dB                                                            */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCMAX;                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetMaxGain(uint32_t u32MaxGain)
{
    ALC->ALC_CTRL.ALCMAX     = u32MaxGain; //ALC_REG bit[27:25]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetALCselect				                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               0: ALC disabled	                                                                       */
/*               1: ALC enabled	                                                                           */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCSEL;                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetALCselect(uint8_t u8Select)
{
    ALC->ALC_CTRL.ALCSEL     = u8Select & 0x01; //ALC_REG bit[28]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_SetALCpeakLimiter				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               0:  enable fast dectrment when signal exceeds 87.5% of full scale	                       */
/*               1: disable fast dectrment when signal exceeds 87.5% of full scale                         */
/* Returns:                                                                                                */
/*               None											                    	                   */
/* Description:                                                                                            */
/*               Set ALC_REG ALCPKLIM;                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_SetALCpeakLimiter(uint8_t u8PKlimiter)
{
    ALC->ALC_CTRL.ALCPKLIM  = u8PKlimiter & 0x01; //ALC_REG bit[31]
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_GetFastDecrement				                                               		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               0 : Flag is not set											                    	   */
/*               1 : Flag is set when signal level is detected to be above 87.5% of full scale             */
/* Description:                                                                                            */
/*               Get Fast decrement ( Clipping Flag )										               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvALC_GetFastDecrement(void)
{
	return (uint32_t)ALC->ALC_STATUS.FAST_DEC;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_GetNoise				                                               		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               0 : Flag is not set											                    	   */
/*               1 : Flag is set when signal level is detected to be below NGTH                            */
/* Description:                                                                                            */
/*               Get NOISE ( Noise Flag )           										               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvALC_GetNoise(void)
{
	return (uint32_t)ALC->ALC_STATUS.NOISE;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_GetP2P				                                                 		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               9 MSBs of measured peak to peak value                                                     */
/* Description:                                                                                            */
/*               Get peak to peak value             										               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvALC_GetP2P(void)
{
	return (uint32_t)ALC->ALC_STATUS.P2P;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_GetPeak				                                                 		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               9 MSBs of measured absolute peak value                                                    */
/* Description:                                                                                            */
/*               Get Absolute peak value             										               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvALC_GetPeak(void)
{
	return (uint32_t)ALC->ALC_STATUS.PEAK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_EnableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               None  											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable ALC interrupt and NVIC corresponding to ALC		            					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_EnableInt(void)
{
    ALC->ALC_INTEN.EN = 1;
    NVIC_EnableIRQ(ALC_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_DisableInt				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                                                                       */
/* Returns:                                                                                                */
/*               None  											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable ALC interrupt and NVIC corresponding to ALC		        					   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvALC_DisableInt(void)
{
    ALC->ALC_INTEN.EN = 0;
    NVIC_DisableIRQ(ALC_IRQn);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvALC_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              None                                                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*              None                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get the version number of ALC driver.                                                      */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvALC_GetVersion(void)
{
	return DRVALC_VERSION_NUM;
}

