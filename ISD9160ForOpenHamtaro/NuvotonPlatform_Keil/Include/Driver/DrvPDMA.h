/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DRVPDMA_H
#define DRVPDMA_H

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "ISD9xx.h"

#ifdef  __cplusplus
extern "C"
{
#endif


#define CHANNEL_OFFSET 0x100
/*---------------------------------------------------------------------------------------------------------*/
/* Version number                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVPDMA_MAJOR_NUM 1
#define DRVPDMA_MINOR_NUM 00
#define DRVPDMA_BUILD_NUM 1

							    
#define DRVPDMA_VERSION_NUM     _SYSINFRA_VERSION(DRVPDMA_MAJOR_NUM, DRVPDMA_MINOR_NUM, DRVPDMA_BUILD_NUM)

/*---------------------------------------------------------------------------------------------------------*/
/* Error Code define                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
// E_DRVPDMA_FALSE_INPUT		    Non-support specified parameter
// E_DRVPDMA_ERR_PORT_INVALID		Invaild port parameter
#define E_DRVPDMA_FALSE_INPUT   	_SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVPDMA, 1)
#define E_DRVPDMA_ERR_PORT_INVALID	_SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVPDMA, 2)

/*---------------------------------------------------------------------------------------------------------*/
/* For interrupt CallBack Function                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*PFN_DRVPDMA_CALLBACK)(uint32_t);

/*---------------------------------------------------------------------------------------------------------*/
/* Enumerate Type                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum
{
	eDRVPDMA_DISABLE = 0,
	eDRVPDMA_ENABLE	 = 1
}E_DRVPDMA_OPERATION;

typedef enum
{
	eDRVPDMA_CHANNEL_0	=	0,
	eDRVPDMA_CHANNEL_1 	=	1,
	eDRVPDMA_CHANNEL_2	=	2,
	eDRVPDMA_CHANNEL_3	=	3
}E_DRVPDMA_CHANNEL_INDEX;

typedef enum
{
	eDRVPDMA_TARGET_SOURCE      = 0,
	eDRVPDMA_TARGET_DESTINATION = 1
}E_DRVPDMA_TARGET;



typedef enum
{
	eDRVPDMA_TABORT_FLAG            = 1,
	eDRVPDMA_BLKD_FLAG              = 2,
	eDRVPDMA_WRA_EMPTY_FLAG         = 0x100,	          	    
	eDRVPDMA_WRA_THREE_FOURTHS_FLAG = 0x200,	  
	eDRVPDMA_WRA_HALF_FLAG          = 0x400,             
	eDRVPDMA_WRA_QUARTER_FLAG       = 0x800           
}E_DRVPDMA_INT_FLAG;

typedef enum
{
	eDRVPDMA_WRA_WRAP_INT = 0x01,	          	    
	eDRVPDMA_WRA_HALF_INT  = 0x04,             
	eDRVPDMA_WRA_WRAP_HALF_INT  = 0x05,             
	eDRVPDMA_WRA_NO_INT    = 0x00           
}E_DRVPDMA_WRA_INT_EN;

typedef enum
{
	eDRVPDMA_DIRECTION_INCREMENTED 	=	0,
	eDRVPDMA_DIRECTION_DECREMENTED	=   1,
	eDRVPDMA_DIRECTION_FIXED      	=	2,
	eDRVPDMA_DIRECTION_WRAPAROUND	=	3,
}E_DRVPDMA_DIRECTION_SELECT;

typedef enum
{
	eDRVPDMA_WIDTH_32BITS = 0,    
	eDRVPDMA_WIDTH_8BITS  = 1,
	eDRVPDMA_WIDTH_16BITS = 2
}E_DRVPDMA_TRANSFER_WIDTH;

typedef enum
{
	eDRVPDMA_TABORT = 1,
	eDRVPDMA_BLKD   = 2,
	eDRVPDMA_WAR    = 4			
}E_DRVPDMA_INT_ENABLE;


typedef enum
{
	eDRVPDMA_SPI0	=	0,    
	eDRVPDMA_DPWM	=	1,  
	eDRVPDMA_UART0	=	2,	  
	eDRVPDMA_I2S	=	3,
	eDRVPDMA_ADC	=	4						  
}E_DRVPDMA_APB_DEVICE;

typedef enum
{
	eDRVPDMA_READ_APB	= 0,    
	eDRVPDMA_WRITE_APB  = 1
}E_DRVPDMA_APB_RW;


typedef enum
{
	eDRVPDMA_MODE_MEM2MEM	=	0,    
	eDRVPDMA_MODE_APB2MEM	=	1,    
	eDRVPDMA_MODE_MEM2APB	=	2
}E_DRVPDMA_MODE;									


typedef struct {
    uint32_t u32Addr;
    E_DRVPDMA_DIRECTION_SELECT eAddrDirection;
}S_DRVPDMA_CH_ADDR_SETTING;


typedef struct DRVPDMA_STRUCT
{
    S_DRVPDMA_CH_ADDR_SETTING sSrcAddr;    		/* Source Address */
	S_DRVPDMA_CH_ADDR_SETTING sDestAddr;		/* Destination Address */
    E_DRVPDMA_TRANSFER_WIDTH  u8TransWidth;		/* Transfer Width */
    E_DRVPDMA_MODE 			  u8Mode;			/* Operation Mode */
	E_DRVPDMA_WRA_INT_EN	  u8WrapBcr;		/* Wrap Mode */
	int32_t              	  i32ByteCnt;		/* Byte Count */
}STR_PDMA_T;



/*---------------------------------------------------------------------------------------------------------*/
/* APIs declaration                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

void DrvPDMA_Init(void);
void DrvPDMA_Close(void);
void DrvPDMA_DisableInt(E_DRVPDMA_CHANNEL_INDEX eChannel, E_DRVPDMA_INT_ENABLE eIntSource);
void DrvPDMA_ClearInt(E_DRVPDMA_CHANNEL_INDEX eChannel, E_DRVPDMA_INT_FLAG eIntFlag);
void DrvPDMA_EnableCH(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_OPERATION eOP);

int32_t DrvPDMA_IsCHBusy(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_Open(E_DRVPDMA_CHANNEL_INDEX sChannel, STR_PDMA_T *sParam);
int32_t DrvPDMA_IsEnabledCH(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_GetTransferLength(E_DRVPDMA_CHANNEL_INDEX eChannel,uint32_t* pu32TransferLength);
int32_t DrvPDMA_SetAPBTransferWidth(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_TRANSFER_WIDTH eTransferWidth);
int32_t DrvPDMA_GetAPBTransferWidth(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_GetCHForAPBDevice(E_DRVPDMA_APB_DEVICE eDevice,E_DRVPDMA_APB_RW eRWAPB);
int32_t DrvPDMA_GetWrapIntType(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_CHSoftwareReset(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_CHEnablelTransfer(E_DRVPDMA_CHANNEL_INDEX eChannel);
int32_t DrvPDMA_EnableInt(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_ENABLE eIntSource);
int32_t DrvPDMA_IsIntEnabled(E_DRVPDMA_CHANNEL_INDEX eChannel,E_DRVPDMA_INT_ENABLE eIntSource);
int32_t DrvPDMA_PollInt(E_DRVPDMA_CHANNEL_INDEX eChannel,  E_DRVPDMA_INT_FLAG eIntFlag);

uint32_t DrvPDMA_GetCurrentSourceAddr(E_DRVPDMA_CHANNEL_INDEX eChannel);
uint32_t DrvPDMA_GetCurrentDestAddr(E_DRVPDMA_CHANNEL_INDEX eChannel);
uint32_t DrvPDMA_GetCurrentTransferCount(E_DRVPDMA_CHANNEL_INDEX eChannel);
uint32_t DrvPDMA_GetInternalBufPointer(E_DRVPDMA_CHANNEL_INDEX eChannel);
uint32_t DrvPDMA_GetSharedBufData(E_DRVPDMA_CHANNEL_INDEX eChannel);

int32_t DrvPDMA_SetTransferSetting(
	E_DRVPDMA_CHANNEL_INDEX eChannel, 
	S_DRVPDMA_CH_ADDR_SETTING* psSrcAddr, 
	S_DRVPDMA_CH_ADDR_SETTING* psDestAddr, 
	uint32_t u32TransferLength
);

int32_t DrvPDMA_GetTransferSetting(
	E_DRVPDMA_CHANNEL_INDEX eChannel, 
	E_DRVPDMA_TARGET eTarget, 
	uint32_t* pu32Addr, 
	E_DRVPDMA_DIRECTION_SELECT* peDirection
);

int32_t DrvPDMA_SetCHForAPBDevice(
    E_DRVPDMA_CHANNEL_INDEX eChannel, 
    E_DRVPDMA_APB_DEVICE eDevice,
    E_DRVPDMA_APB_RW eRWAPB    
);

int32_t DrvPDMA_InstallCallBack(
    E_DRVPDMA_CHANNEL_INDEX eChannel, 
    E_DRVPDMA_INT_ENABLE eIntSource,
	PFN_DRVPDMA_CALLBACK pfncallback
);


int32_t DrvPDMA_SetCHForAPBDevice(
    E_DRVPDMA_CHANNEL_INDEX eChannel, 
    E_DRVPDMA_APB_DEVICE 	eDevice,
    E_DRVPDMA_APB_RW 		eRWAPB    
);

int32_t DrvPDMA_GetVersion(void);


#ifdef  __cplusplus
}
#endif

#endif	// DRVPDMA_H














