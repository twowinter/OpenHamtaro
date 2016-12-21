/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __MICSPK_H__
#define __MICSPK_H__

#include "Platform.h"

#ifdef  __cplusplus
extern "C"
{
#endif
	
//---------------------------------------------------------------------------
// Function: MIC_Open
//
// Description:
// 		Configure ADC pins and mode for MIC recording function
//
// Argument:
// 		none
//
// Return:
//		None.
//---------------------------------------------------------------------------
void MIC_Open(void);
	
//---------------------------------------------------------------------------
// Function: MIC_Start
//
// Description:
//		Start ADC to recording from MIC
//
// Argument:
// 		none
//
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Start(void);

//---------------------------------------------------------------------------
// Function: MIC_Stop
//
// Description:
//		Start ADC to recording from MIC
//
// Argument:
// 		none
//
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Stop(void);

//---------------------------------------------------------------------------
// Function: MIC_Close
//
// Description:
//		Close ADC to record from MIC
//
// Argument:
// 		none
//
// Return:
//		none.
//---------------------------------------------------------------------------
void MIC_Close(void);
	
//---------------------------------------------------------------------------
// Function: MIC_ChangeSR
//
// Description:
//		Change the sample rate of MIC recording
//
// Argument:
//		u32SampleRate: wanted sample rate
//
// Return:
//		none 
//---------------------------------------------------------------------------
void MIC_ChangeSR(UINT32 u32SampleRate);


//---------------------------------------------------------------------------
// Function: SPK_Open
//
// Description:
// 		Configure APU mode for speaker 
//
// Argument:
// 		none
//
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Open(void);

//---------------------------------------------------------------------------
// Function: SPK_Close
//
// Description:
// 		Close APU operation for speaker 
//
// Argument:
// 		none
//
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Close(void);

void SPK_Start(void);
void SPK_Stop(void);

//---------------------------------------------------------------------------
// Function: SPK_ChangeSR
//
// Description:
//		Change the sample rate of Speaker
//
// Argument:
//		u32SampleRate: wanted sample rate
//
// Return:
//		none 
//---------------------------------------------------------------------------
void SPK_ChangeSR(UINT32 u32Channel, UINT32 u32SampleRate);

//---------------------------------------------------------------------------
// Function: SPK_Pause
//
// Description:
// 		Pause APU operation to output to speaker 
//
// Argument:
// 		none
//
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Pause(UINT32 u32Channel);

//---------------------------------------------------------------------------
// Description:
// 		Resume APU operation to output to speaker 
//
// Argument:
// 		none
//
// Return:
//		None.
//---------------------------------------------------------------------------
void SPK_Resume(UINT32 u32Channel);

#ifdef  __cplusplus
}
#endif

#endif //__MICSPK_H__
