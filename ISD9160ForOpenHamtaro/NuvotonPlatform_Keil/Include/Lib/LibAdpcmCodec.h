/*----------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                        */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/* Include related headers                                                                */
/*----------------------------------------------------------------------------------------*/
#ifndef __LIBADPCMCODEC_H__
#define __LIBADPCMCODEC_H__

#include "ISD9xx.h"
#include "System/SysInfra.h"
#include "NVTTypes.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define LIBADPCM_HEADER_SIZE		4

typedef struct tsAdpcm_BufInfo
{
	BYTE* pbyInBufAddr;
	uint16_t ui16InBufSize;
	uint16_t ui16RIndex;
	uint16_t ui16WIndex;
	uint16_t ui16WSize;
}sAdpcm_BufInfo;

typedef struct tsAdpcmState 
{
    int16_t	   i16PrevValue;		// Previous output value.
    int8_t	   i8StepIndex;		    // Index of stepsize table

	uint8_t    ui8Channels;		    // Channel number.
	uint16_t   ui16SmplPerBlock;	// Total samples per block.
	uint16_t   ui16BytesPerBlock;	// Total bytes per block.
	uint16_t   ui16SmplIndex;		// Sample index of current block
	uint8_t	   ui8PrevByte;         // pointer to previous output bytes address
}sLibAdpcmState;

/*----------------------------------------------------------------------------------------*/
/* Define functions prototype                                                             */
/*----------------------------------------------------------------------------------------*/
uint32_t LibAdpcmInit(sLibAdpcmState* psState, uint16_t	ui16Channels,
	                  uint32_t ui32SamplingRate, uint16_t ui16BitsPerSample);

void LibAdpcm_Encode(PINT16	pi16Indata,	BYTE* pbOutdata, 
                     uint16_t ui16Len, sLibAdpcmState*	psState);
	                  
void LibAdpcm_Decode(BYTE* pbIndata, PINT16 pi16Outdata, uint16_t ui16Len, sLibAdpcmState* psState, 
                     uint8_t ui8ChannelIndex, uint8_t ui8ChannelNextDataWordOffset);

extern BOOL LibAdpcm_EncodeBlock(PINT16 pi16InData, BYTE* pbOutData, 
                          uint16_t ui16SampleCount, sLibAdpcmState*	psState);
                          
extern BOOL LibAdpcm_DecodeBlock(BYTE* pbInData, PINT16 pi16OutData, uint16_t ui16SampleCount,
	                          sLibAdpcmState* psState);
#endif //__LIBADPCMCODEC_H__






