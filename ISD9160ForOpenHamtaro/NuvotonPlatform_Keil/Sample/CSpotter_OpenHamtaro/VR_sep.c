#include "ISD9xx.h"
#include "base_types.h"
#include "CSpotterSDKApi.h"
#include <stdio.h>
#include "DrvUART.h"
#include "DrvGPIO.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void Wave_StartRecord(void);
void Wave_StopRecord(void);
INT Wave_GetSample(SHORT **lppsSample);
INT Wave_UnlockSample(SHORT **lppsSample);
INT Wave_CheckCpuOverload(void);
void Wave_ClearCpuOverload(void);
void ShowLED(int32_t n);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
#define LEDOFF0 DrvGPIO_SetBit(GPB,1)
#define LEDOFF1 DrvGPIO_SetBit(GPB,0)
#define LEDOFF2 DrvGPIO_SetBit(GPB,2)
#define LEDOFF3 DrvGPIO_SetBit(GPB,4)
#define LEDOFF4 DrvGPIO_SetBit(GPB,6)

#define LEDON0 DrvGPIO_ClrBit(GPB,1)
#define LEDON1 DrvGPIO_ClrBit(GPB,0)
#define LEDON2 DrvGPIO_ClrBit(GPB,2)
#define LEDON3 DrvGPIO_ClrBit(GPB,4)
#define LEDON4 DrvGPIO_ClrBit(GPB,6)
#else
#define LEDOFF0 DrvGPIO_SetBit(GPB,5)
#define LEDOFF1 DrvGPIO_SetBit(GPB,6)
#define LEDOFF2 DrvGPIO_SetBit(GPB,7)
#define LEDOFF3 DrvGPIO_SetBit(GPA,13)
#define LEDOFF4 DrvGPIO_SetBit(GPA,11)

#define LEDON0 DrvGPIO_ClrBit(GPB,5)
#define LEDON1 DrvGPIO_ClrBit(GPB,6)
#define LEDON2 DrvGPIO_ClrBit(GPB,7)
#define LEDON3 DrvGPIO_ClrBit(GPA,13)
#define LEDON4 DrvGPIO_ClrBit(GPA,11)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Define macro & global variables                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define k_nMemSize			9792//9728//(10 * 1024)		//9.5KB
INT nMemSize = k_nMemSize;
__align(4) BYTE lpbyMemPool[k_nMemSize];

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function									                                           			                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DoVR_sep(uint32_t lpbyBaseModel, uint32_t lpbyModel, uint32_t bShowOnly, uint32_t nTimeOut, uint32_t nPinOn, int (*funcCheckKeyPress)(void))
{
	HANDLE hCSpotter = NULL;
	INT n, nRet;
	BYTE	nID, nLED, nWait;
	SHORT *lpsSample;
	INT nNumSample;

	n = CSpotter_GetMemoryUsage_Sep((BYTE *)lpbyBaseModel, (BYTE *)lpbyModel);

	hCSpotter = CSpotter_Init_Sep((BYTE *)lpbyBaseModel, (BYTE *)lpbyModel, lpbyMemPool, nMemSize, &nRet);
	if (hCSpotter == NULL)
		goto L_ERROR;

	Wave_StartRecord();
	nWait = 0;
	nRet = 0;

	CSpotter_Reset(hCSpotter);

	while (1)
	{
		if (funcCheckKeyPress && (nID = funcCheckKeyPress()) > 0)
		{
			nRet = nID;
			break;
		}
		nNumSample = Wave_GetSample(&lpsSample);
		if (nNumSample < 0)
			break;
		if (nNumSample == 0)
			continue;
		if (CSpotter_AddSample(hCSpotter, lpsSample, nNumSample) == CSPOTTER_SUCCESS)
		{
			nID = CSpotter_GetResult(hCSpotter);
			nLED = nID + 1;
			if (Wave_CheckCpuOverload())
			{
//				nLED |= 0x40;
				Wave_ClearCpuOverload();
			}
			nLED |= nPinOn;
			ShowLED(nLED);
			if (bShowOnly == 0)
			{
				nRet = nID + 1;
				break;
			}
			nWait = 0;
			DrvUART_Write(UART_PORT0,&nID,1);		//confid 20150603
//            printf("%x",nID);
			LEDOFF0;
			LEDOFF1;
			LEDOFF2;
			LEDOFF3;
			LEDOFF4;
		
			nID += 1;
			#if 0
			if(nID&0x01)
			{LEDON0;}
			if(nID&0x02)
			{LEDON1;}
			if(nID&0x04)
			{LEDON2;}
			if(nID&0x08)
			{LEDON3;}
			if(nID&0x10)
			{LEDON4;}
//			if(nID == 0x00)	//
//			{LEDON0;LEDON1;LEDON2;LEDON3;LEDON4;}
			#else
			if(nID == 0x01)   //   全开模式  
			{
				LEDON0;LEDON1;LEDON2;LEDON3;LEDON4;
			}
			else if(nID == 0x02)   //    外出模式
			{
				//LEDOFF0;LEDOFF1;LEDOFF2;LEDOFF3;LEDOFF4;
			}
			else if(nID == 0x03)   //    房间开启
			{
				LEDON0;LEDON1;LEDON2;
			}
			else if(nID == 0x04)     //   房间关闭
			{
				LEDOFF0;LEDOFF1;LEDOFF2;
			}
			else
			{
				if(nID&0x01)
				{LEDON0;}
				if(nID&0x02)
				{LEDON1;}
				if(nID&0x04)
				{LEDON2;}
				if(nID&0x08)
				{LEDON3;}
				if(nID&0x10)
				{LEDON4;}
	//			if(nID == 0x00)	//
	//			{LEDON0;LEDON1;LEDON2;LEDON3;LEDON4;}

			}
			
			#endif
			
		}

		Wave_UnlockSample(&lpsSample);

		nWait++;
		if (nTimeOut > 0 && nWait >= nTimeOut)
			break;
	}

L_ERROR:
	Wave_StopRecord();
	CSpotter_Release(hCSpotter);

	return nRet;
}
