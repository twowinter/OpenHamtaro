/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/* Siren7 (G.722) licensed from Polycom Technology                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* This code will get PCM data from MIC then encode with S7 compression then write to data flash.          */
/* After recording, it will decode and play back the recorded sound. Recording time is fixed.              */
/* Should set the Data flash enable with ICPTool and set Data flash start address @ 0x10000.               */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "ISD9xx.h"
#include "Driver/DrvTimer.h"
#include "Driver\DrvUART.h"
#include "Driver/DrvGPIO.h"
#include "Driver/DrvSYS.h"
#include "Driver\DrvADC.h"
#include "Driver\DrvFMC.h"
#include "base_types.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SysTimerDelay(uint32_t us);
void ShowLED(int32_t n);
void FlashLED(void);
void Wave_Initial(void);
void Sleep(uint32_t ms);

int UnpackBin(uint32_t lpbyFlashAddr, char *lppbyModel[], int nMaxNumModel, int *lpnFlashSize);

int32_t DoVR_sep(uint32_t lpbyBaseModel, uint32_t lpbyModel, uint32_t bShowOnly, uint32_t nTimeOut, uint32_t nPinOn, int (*funcCheckKeyPress)(void));

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define BIN_START_ADDR		0x8C00		// 35KB
//#define BIN_START_ADDR		0xC800		// 50KB

#define IO_PIN0	0//1
#define IO_PIN1	2//3
#define IO_PIN2	4//5
#define IO_PIN3	6//7
#define IO_PIN4	8//9
#define IO_PIN5	10//11
#define IO_PIN6	12//13
#define IO_PIN7	14//15

//====================
// Functions & main
void LdoOn(void)
{
	SYSCLK->APBCLK.ANA_EN=1;
	ANA->LDOPD.PD=0;
	ANA->LDOSET=3;

	SysTimerDelay(6000000);
}

void InitialGPIO(void)
{
	DrvGPIO_Open(GPA,IO_PIN0,IO_OUTPUT);			//Set GPIO for PIN0
	DrvGPIO_Open(GPA,IO_PIN1,IO_OUTPUT);			//Set GPIO for PIN1
	DrvGPIO_Open(GPA,IO_PIN2,IO_OUTPUT);			//Set GPIO for PIN2
	DrvGPIO_Open(GPA,IO_PIN3,IO_OUTPUT);			//Set GPIO for PIN3
	DrvGPIO_Open(GPA,IO_PIN4,IO_OUTPUT);			//Set GPIO for PIN4
	DrvGPIO_Open(GPA,IO_PIN5,IO_OUTPUT);			//Set GPIO for PIN5
	DrvGPIO_Open(GPA,IO_PIN6,IO_OUTPUT);			//Set GPIO for PIN6
	DrvGPIO_Open(GPA,IO_PIN7,IO_OUTPUT);			//Set GPIO for PIN7
}

void ShowLED(int32_t n)
{
	if ((n & 0x1) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN0);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN0);
	if ((n & 0x2) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN1);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN1);
	if ((n & 0x4) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN2);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN2);
	if ((n & 0x8) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN3);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN3);
	if ((n & 0x10) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN4);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN4);
	if ((n & 0x20) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN5);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN5);
	if ((n & 0x40) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN6);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN6);
	if ((n & 0x80) != 0)
		DrvGPIO_SetBit(GPA,IO_PIN7);
	else
		DrvGPIO_ClrBit(GPA,IO_PIN7);
}

void FlashLED(void)
{
	ShowLED(0xffff);
	Sleep(400);
	ShowLED(0x0);
}

int UnpackBin(uint32_t lpbyFlashAddr, char *lppbyModel[], int nMaxNumModel, int *lpnFlashSize)
{
	int *lpnBin = (int *)lpbyFlashAddr;
	int nNumBin = lpnBin[0];
	int *lpnBinSize = lpnBin + 1;
	int i;

	lppbyModel[0] = (char *)(lpnBinSize + nNumBin);
	for (i = 1; i < nNumBin; i++){
		if (i >= nMaxNumModel)
			break;
		lppbyModel[i] = lppbyModel[i-1] + lpnBinSize[i-1];
	}

	*lpnFlashSize = (uint32_t)(lppbyModel[i-1] + lpnBinSize[i-1]) - lpbyFlashAddr;
	*lpnFlashSize = ((*lpnFlashSize + 0xFFF) >> 12) << 12;	// 4KB alignment for SPI

	return i;
}

/*---------------------------------------------------------------------------------------------------------*/
/* SysTimerDelay                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void SysTimerDelay(uint32_t us)
{
    SysTick->LOAD = us * 49; /* Assume the internal 49MHz RC used */
    SysTick->VAL  =  (0x00);
    SysTick->CTRL = (1 << SYSTICK_CLKSOURCE) | (1<<SYSTICK_ENABLE);

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

void UartInit(void)
{
	/* Reset IP */
	SYS->IPRSTC2.UART0_RST = 1;
	SYS->IPRSTC2.UART0_RST = 0;
	/* Enable UART clock */
    SYSCLK->APBCLK.UART0_EN = 1;
    /* Data format */
    UART0->LCR.WLS = 3;
    /* Configure the baud rate */
    M32(&UART0->BAUD) = 0x3F0001A8; /* Internal 48MHz, 115200 bps */

    /* Multi-Function Pin: Enable UART0:Tx Rx */
	SYS->GPA_ALT.GPA8 = 1;
	SYS->GPA_ALT.GPA9 = 1;
}


/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function									                                           			   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
	char *lppbyModel[2];	// 0:base, 1:SI
	int nBinSize;

	UNLOCKREG();
	SYSCLK->PWRCON.OSC49M_EN = 1;
	SYSCLK->CLKSEL0.HCLK_S = 0; /* Select HCLK source as 48MHz */
	SYSCLK->CLKDIV.HCLK_N  = 0;	/* Select no division          */
	SYSCLK->CLKSEL0.OSCFSel = 0;	/* 1= 32MHz, 0=48MHz */

	DrvADC_AnaOpen();				//Will reset all IP in ANA_BASE

	LdoOn();

	UartInit();

	Wave_Initial();
	InitialGPIO();
	UNLOCKREG();

	if (UnpackBin((uint32_t)BIN_START_ADDR, lppbyModel, 2, &nBinSize) < 2)
		goto L_ERROR;

	FlashLED();

	while (1)
	{
		ShowLED(0x80);
		if (DoVR_sep((uint32_t)lppbyModel[0], (uint32_t)lppbyModel[1], 1, 0, 0x80, NULL) <= 0)
			break;
	}

L_ERROR:
	ShowLED(0xffff);

//	printf("\n=== Test Done ===\n");


	while(1);

	/* Lock protected registers */

}



