/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __NOISEFILTER_H__
#define __NOISEFILTER_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#define	NOISEFILTER_MAJOR_NUM 2
#define	NOISEFILTER_MINOR_NUM 00
#define	NOISEFILTER_BUILD_NUM 001
#define NOISEFILTER_VERSION_NUM	_SYSINFRA_VERSION(NOISEFILTER_MAJOR_NUM, NOISEFILTER_MINOR_NUM, NOISEFILTER_BUILD_NUM)

// enable or disable different type of filter

#define NOISEFILTER_DISABLE	(0x00)		// disable all filters
#define NOISEFILTER_COMB	(0x01)		// enable comb filter
#define NOISEFILTER_SRC		(0x02)		// enable to suppress all "PCM < 512"
#define NOISEFILTER_SIL		(0x04)		// enable to suppress PCM those continous "PCM < 256"
#define NOISEFILTER_IIR2X6	(0x08)		// enable IIR 2nd order filter for intput sample rate == 6 x output sample rate
#define NOISEFILTER_IIR2X4	(0x10)		// enable IIR 2nd order filter for intput sample rate == 4 x output sample rate
#define NOISEFILTER_DCR1_8K	(0x20)		// enable DC removal filter for 8K sample rate (remove < 100Hz noise)
#define NOISEFILTER_IIR2X3	(0x40)		// enable IIR 2nd order filter for intput sample rate == 3 x output sample rate
#define NOISEFILTER_DCR1_12K	(0x80)		// enable DC removal filter for 12K sample rate (remove < 100Hz noise)
#define NOISEFILTER_DCR1_16K	(0x20)		// enable DC removal filter for 16K sample rate (remove < 100Hz noise)
#define NOISEFILTER_DCR1_24K	(0x20)		// enable DC removal filter for 24K sample rate (remove < 100Hz noise)
#define NOISEFILTER_DCR1_32K	(0x20)		// enable DC removal filter for 32K sample rate (remove < 100Hz noise)
#define NOISEFILTER_CLIP	(NOISEFILTER_COMB|NOISEFILTER_IIR2X6|NOISEFILTER_IIR2X4|NOISEFILTER_IIR2X3|NOISEFILTER_DCR1)
#define NoiseFilter_DCR1 NoiseFilter_DCR1_8K
#define NOISEFILTER_DCR1 NOISEFILTER_DCR1_8K

//----------------------------------------------------------------------------------------------------
extern INT32 g_i32MaxPcm;
extern INT32 g_i32MinPcm;

//----------------------------------------------------------------------------------------------------
UINT32
NoiseFilter_GetVersion(void);


//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_SuppressSrc
// Description: Suppress PCM value that is less than 512. The noise is suppressed, but sound quality is affected.
// Parameter:	i16Pcm		Input PCM
// Return:	Output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_SuppressSrc(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_SetParamSuppressSil
// Description: Set parameters for suppressing silence noise.
// Parameter:	i16SilCntThd	Silence means PCM value < 256, and keep under 256 for i16SilCntThd samples. 
//----------------------------------------------------------------------------------------------------
void
NoiseFilter_SetParamSuppressSil(
	INT16 i16SilCntThd
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_SuppressSil
// Description: Suppress PCM value that is treated as silence.	
// Parameter:	i16Pcm	 	Input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_SuppressSil(
	INT16 i16Pcm
);


//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_SetClipRange
// Description: Set clip range. (Note: The output PCM of some filters need to be clipped.
// Parameter:	i16MaxPcm	 	Max clip value.
//				i16MinPcm	 	Min clip value.
//----------------------------------------------------------------------------------------------------
static __inline
void
NoiseFilter_SetClipRange(
	INT32 i32MaxPcm,
	INT32 i32MinPcm
)
{
	g_i32MaxPcm = i32MaxPcm;
	g_i32MinPcm = i32MinPcm;
}

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_ClipPcm
// Description: Clip PCM value.
// Parameter:	i16Pcm	 	Input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
static __inline
INT16 
NoiseFilter_ClipPcm(
	INT32 i32Pcm
)
{
	if (i32Pcm > g_i32MaxPcm)
		i32Pcm = g_i32MaxPcm;
	if (i32Pcm < g_i32MinPcm)
		i32Pcm = g_i32MinPcm;
	return (INT16)i32Pcm;
}

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_SetParamComb
// Description: Set parameters for comb filter.	Some noise comes align with sample number such as saving PCM
//				data into SPI-flash.
// Parameter:	i16CombNum		Sample number of the "comb".
//				pCombBuf		Buffer to save samples for processing.
//				i16BufSize		Buffr size == (2*(i16CombNum+1)) bytes.
//----------------------------------------------------------------------------------------------------
void 
NoiseFilter_SetParamComb(
	INT16 i16CombNum,
	INT16 *pCombBuf
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_ResetComb
// Description: Reset comb filter. It must be called for filtering discontinuous samples.
//----------------------------------------------------------------------------------------------------
void
NoiseFilter_ResetComb(void);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_Comb
// Description: Do comb filter to input pcm. It filters regular noise that are caused by regular processing
//				on PCM samples such as saving 128 samples (256 bytes, 1 page) to SPI flash.
// Parameter:	i16Pcm		input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_Comb(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_ResetIIR2
// Description: Reset IIR2 filter. It must be called for filtering discontinuous samples.
//----------------------------------------------------------------------------------------------------
void
NoiseFilter_ResetIIR2(void);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_IIR2x6
// Description: Do IIR 2nd order filter to input pcm. Application must record in 6x sample rate.
//				Filter with IIR2 filter, and decimate to the wanted 1x sample rate.
// Parameter:	i16Pcm		input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_IIR2x6(
	INT16 i16Pcm			// 12 bits
);
//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_IIR2x4
// Description: Do IIR 2nd order filter to input pcm. Application must record in 4x sample rate.
//				Filter with IIR2 filter, and decimate to the wanted 1x sample rate.
// Parameter:	i16Pcm		input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_IIR2x4(
	INT16 i16Pcm			// 12 bits
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_IIR2x3
// Description: Do IIR 2nd order filter to input pcm. Application must record in 3x sample rate.
//				Filter with IIR2 filter, and decimate to the wanted 1x sample rate.
// Parameter:	i16Pcm		input PCM
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16
NoiseFilter_IIR2x3(
	INT16 i16Pcm			// 12 bits
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_ResetDCR1
// Description: Reset DCR 1st order filter. It must be called for filtering discontinuous samples.
//----------------------------------------------------------------------------------------------------
void 
NoiseFilter_ResetDCR1(void);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_DCR1_8K
// Description: Do DCR 1st order filter to input pcm. It filters noise under 100Hz for 6K~8K sample 
//				rate wavform.
// Parameter:	i16Pcm		input PCM													
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16 
NoiseFilter_DCR1_8K(
	INT16 i16Pcm
);
//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_DCR1_12K
// Description: Do DCR 1st order filter to input pcm. It filters noise under 100Hz for 8K~12K sample 
//				rate wavform.
// Parameter:	i16Pcm		input PCM													
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16 
NoiseFilter_DCR1_12K(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_DCR1_16K
// Description: Do DCR 1st order filter to input pcm. It filters noise under 100Hz for 12K~16K sample 
//				rate wavform.
// Parameter:	i16Pcm		input PCM													
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16 
NoiseFilter_DCR1_16K(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_DCR1_24K
// Description: Do DCR 1st order filter to input pcm. It filters noise under 100Hz for 16K~24K sample 
//				rate wavform.
// Parameter:	i16Pcm		input PCM													
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16 
NoiseFilter_DCR1_24K(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_DCR1_32K
// Description: Do DCR 1st order filter to input pcm. It filters noise under 100Hz for 24K~32K sample 
//				rate wavform.
// Parameter:	i16Pcm		input PCM													
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
INT16 
NoiseFilter_DCR1_32K(
	INT16 i16Pcm
);

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
UINT32 
NoiseFilter_GetSmoothRamSize(
	UINT8 u8SmoothNum,
	UINT8 u8SmoothSize
	);

void* 
NoiseFilter_SetParamSmooth(
	void* pSmooth,
	UINT8 u8SmoothNum,
	UINT8 u8SmoothSize
);

void 
NoiseFilter_ResetSmooth(
	void* pSmooth,
	UINT8 u8SmoothNum,
	UINT8 u8SmoothSize
);

INT16 
NoiseFilter_SmoothOne(
	void* pSmooth,
	INT16 i16Pcm
);

INT16 
NoiseFilter_Smooth(
	void* pSmooth,
	UINT8 u8SmoothNum,
	INT16 i16Pcm
);
/*
INT16 
NoiseFilter_LpfDownProcessing(
	INT16 i16Sample0,
	INT16 i16Sample1);
*/
//----------------------------------------------------------------------------------------------------
// Function: NoiseFilter_NoiseFilter
// Description: An API to composite all supported filters.
// Parameter:	i16Pcm			input PCM
//				u8EnableFunc	Bitwise options to enable/disable different kind of filters. 
//								(Note: It clips otput pcm if necessary.)
// Return:	output PCM
//----------------------------------------------------------------------------------------------------
/*
INT16
NoiseFilter_NoiseFilter(
	INT16 i16Pcm,
	UINT8 u8EnableFunc
);
*/



#ifdef  __cplusplus
}
#endif

#endif //#ifndef __NOISEFILTER_H__
