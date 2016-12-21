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
#include "DrvOSC.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvOSC_Absolute				                                                    	       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               i32arg 		- [in]		input value                                  	               */
/* Returns:                                                                                                */
/*               |i32arg|											                    		       	   */
/*               							                                                               */
/* Description:                                                                                            */
/*               return absolute of input value										           		   	   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvOSC_Absolute(int32_t i32arg)
{
	return (i32arg >= 0 ? i32arg : -i32arg);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvOSC_MeasureOsc				                                                    	       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               hclk div - the current division of OSC48M to HCLK	                                       */
/*               num_cycles - the number of 32k cycles to measure HCLK                     	               */
/* Returns:                                                                                                */
/*               measurement frequency  							                    		       	   */
/*               							                                                               */
/* Description:                                                                                            */
/*               To measure the internal oscillator frequency against 32.768 kHz crystal referecne         */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvOSC_MeasureOsc(int32_t hclk_div, int32_t num_cycles)
{
	int32_t i32Freq, i32Timeout=0;
	
	ANA->FREQ_CTRL.FM_CYCLE = num_cycles-1;
	ANA->FREQ_CTRL.FM_GO = TRUE;
	while( (ANA->FREQ_CTRL.FM_DONE != 1) && (i32Timeout++ < 0x100000));
	if(	i32Timeout >= 0x100000)
		return(E_DRVOSC_MEAS_TIMEOUT);
	i32Freq = ANA->FREQ_CNT;
	ANA->FREQ_CTRL.FM_GO = FALSE;
	// Frequency is calculated assuming HCLK is divided by hclk_div and we are measuring 
	// num_cycles cycles and reference frequency is 32768Hz
	i32Freq =(int32_t)((uint32_t)i32Freq*(uint32_t)(32768/8)*(uint32_t)hclk_div/(uint32_t)num_cycles*(uint32_t)8);
	return(i32Freq);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvOSC_TrimOscillator				                                                    	   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               Target frequency                                                   	                   */
/*               Trim Index 0 or 1                                                                         */
/* Returns:                                                                                                */
/*               Trim frequency                                                         		       	   */
/*               							                                                               */
/* Description:                                                                                            */
/*               To trim the oscillator to a desired frequency							   	               */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvOSC_TrimOscillator(int32_t i32Target, int32_t i32Index)
  
{
	// Trim index [0,1] to target Hz [nominally 32,768,000 or 49,152,000]
	uint8_t u8Coarse, u8Fine, maxFine, minFine;
	int32_t i32Freq, i32LastFreq;


	
   	SYSCLK->APBCLK.ANA_EN   = 1;               /* Turn on analog peripheral clock */
	// Osc Trim and Measurement test...	 
	ANA->FREQ_CTRL.FM_SEL = 1;   // Select reference source as 32kHz XTAL input
	ANA->FREQ_CTRL.FM_GO = 0;
	UNLOCKREG();
	// Set HCLK divide to ensure oscillator does not violate access timing.
	SYSCLK->CLKDIV.HCLK_N = DRVOSC_MEAS_HCLK_DIV-1;
	
	// Select TRIM soruce
	SYSCLK->CLKSEL0.OSCFSel = i32Index;
	SYS->OSCTRIM[i32Index].RANGE = 0;
	// Make sure slowest frequency is lower than target
	u8Fine = DRVOSC_RESERVE_RANGE;
	u8Coarse = 0;
	SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*(DRVOSC_MAX_FINE+1);
	i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
	if(i32Freq > i32Target)
			SYS->OSCTRIM[i32Index].RANGE = 1;
	
	// There are 32 (DRVOSC_MAX_FINE+1) fine settings and 8 coarse ranges. 
	// Find suitable coarse range, the range where measured frequency first exceeds target.
	u8Fine = DRVOSC_MAX_FINE - DRVOSC_RESERVE_RANGE;
	u8Coarse = 0;
	do{
    	SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*(DRVOSC_MAX_FINE+1);
		i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
		u8Coarse++;
    }while((i32Freq < i32Target) && (u8Coarse <= DRVOSC_MAX_COARSE) );
	u8Coarse--;
	
    // At this point measured frequency should be greater than target and a valid coarse range
    // selected. Check for error condition: coarse range overflow
    if(i32Freq < i32Target) 
        return(E_DRVOSC_COARSE_OVF);
       
#ifdef BINARY_SEARCH	 	
	//Now trim to this range using binary search. 
	maxFine = DRVOSC_MAX_FINE-DRVOSC_RESERVE_RANGE;
	minFine = DRVOSC_RESERVE_RANGE;
	
	do{
		i32LastFreq = i32Freq; //Store last frequency 
		u8Fine = ((maxFine+minFine)/2);
		SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*32;
		i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
		if (i32Freq < i32Target){  //Check if Freq is greter or less than target, move max and mins accordingly  
			minFine = u8Fine;  }
		else{
			maxFine = u8Fine;}
	}while ((maxFine-minFine != 1) && (u8Fine <= DRVOSC_MAX_FINE));
	if (i32Freq <= i32Target){
		u8Fine++;
		i32LastFreq = i32Freq;
		SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*32;
		i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
	}

	i32LastFreq = i32Freq;
	SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*32;
	i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
	   // At this point measured frequency should be greater than target and a valid fine range
    // selected. Check for error condition: fine range overflow
    if(i32Freq < i32Target) 
        return(E_DRVOSC_FINE_OVF);


    // See which trim has best error.
	if(DrvOSC_Absolute(i32Target-i32LastFreq) < DrvOSC_Absolute(i32Freq-i32Target)){
        // LastFreq was closer, decrement fine trim.
		u8Fine--;
		i32Freq = i32LastFreq;
	}
    // Set final trim and return clock divider to unity.
    SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*(DRVOSC_MAX_FINE+1);
    SYSCLK->CLKDIV.HCLK_N = 0;
    return(i32Freq);	  	   
#else									  
	// Now trim to this range. Could do a binary search later, for now linear search.
	u8Fine = DRVOSC_RESERVE_RANGE;
	i32Freq = 0;
    do{
   		i32LastFreq = i32Freq; // Store last frequency
    	SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*(DRVOSC_MAX_FINE+1);
		i32Freq = DrvOSC_MeasureOsc(DRVOSC_MEAS_HCLK_DIV,DRVOSC_NUM_CYCLES);
		u8Fine++;
	}while((i32Freq < i32Target) && (u8Fine <= DRVOSC_MAX_FINE));
	u8Fine--; 
	   // At this point measured frequency should be greater than target and a valid fine range
    // selected. Check for error condition: fine range overflow
    if(i32Freq < i32Target) 
        return(E_DRVOSC_FINE_OVF);


    // See which trim has best error.
	if(DrvOSC_Absolute(i32Target-i32LastFreq) < DrvOSC_Absolute(i32Freq-i32Target)){
        // LastFreq was closer, decrement fine trim.
		u8Fine--;
		i32Freq = i32LastFreq;
	}
    // Set final trim and return clock divider to unity.
    SYS->OSCTRIM[i32Index].TRIM = u8Fine + u8Coarse*(DRVOSC_MAX_FINE+1);
    SYSCLK->CLKDIV.HCLK_N = 0;
    return(i32Freq);		
#endif

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvOSC_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              None                                                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/*              None                                                                                       */
/*                                                                                                         */
/* Description:                                                                                            */
/*              Get the version number of OSC driver.                                                      */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvOSC_GetVersion(void)
{
	return DRVOSC_VERSION_NUM;
}
