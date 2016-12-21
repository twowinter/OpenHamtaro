/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef __DRVADC_H__
#define __DRVADC_H__


/*---------------------------------------------------------------------------------------------------------*/
/* Include related headers                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#include "ISD9xx.h"
#include "System/SysInfra.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
/* version definition with SysInfra */
#define	DRVADC_MAJOR_NUM 1
#define	DRVADC_MINOR_NUM 00
#define	DRVADC_BUILD_NUM 1
#define DRVADC_VERSION_NUM    _SYSINFRA_VERSION(DRVADC_MAJOR_NUM, DRVADC_MINOR_NUM, DRVADC_BUILD_NUM)

/* error code definition */
#define E_DRVADC_ARGUMENT           _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVADC, 1)
#define E_DRVADC_CHANNELNUM         _SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVADC, 2)
#define E_DRVADC_OPEN_BEFORE		_SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVADC, 3)
#define E_DRVADC_ANA_OPEN			_SYSINFRA_ERRCODE(TRUE, MODULE_ID_DRVADC, 10)

/* constant definition */
typedef int32_t	ERRCODE;
typedef unsigned char BOOL;

typedef enum {
	eDRVADC_PDHIRES_CONNECTED    = 0,
	eDRVADC_PDHIRES_DISCONNECTED = 1
} E_DRVADC_PDHIRES;

typedef enum {
	eDRVADC_PDLORES_CONNECTED    = 0,
	eDRVADC_PDLORES_DISCONNECTED = 1
} E_DRVADC_PDLORES;

typedef enum {
	eDRVADC_PULLDOWN_VMID_RELEASE = 0,
	eDRVADC_PULLDOWN_VMID_GND	  = 1
} E_DRVADC_PULLDOWN;

typedef enum {
	eDRVADC_BOOSTGAIN_0DB    = 0,
	eDRVADC_BOOSTGAIN_26DB	 = 1
} E_DRVADC_BOOSTGAIN;

typedef enum {
	eDRVADC_PU_BOOST_OFF = 0,
	eDRVADC_PU_BOOST_ON	 = 1
} E_DRVADC_PU_BOOST;

typedef enum {
	eDRVADC_PU_PGA_OFF   = 0,
	eDRVADC_PU_PGA_ON	 = 1
} E_DRVADC_PU_PGA;

typedef enum {
	eDRVADC_REF_SEL_VMID = 0,
	eDRVADC_REF_SEL_VBG	 = 1
} E_DRVADC_REF_SEL;

typedef enum {
	eDRVADC_MUTE_PGA = 0,
	eDRVADC_MUTE_IPBOOST = 1
} E_DRVADC_MUTE;


typedef enum {
	eDRVADC_PU_IBGEN_OFF = 0,
	eDRVADC_PU_IBGEN_ON	 = 1
} E_DRVADC_PU_IBGEN;

typedef enum {
	eDRVADC_PU_MOD_OFF   = 0,
	eDRVADC_PU_MOD_ON	 = 1
} E_DRVADC_PU_MOD;
 
typedef enum {
	eDRVADC_PU_BUFADC_OFF = 0,
	eDRVADC_PU_BUFADC_ON  = 1
} E_DRVADC_PU_BUFADC;

typedef enum {
	eDRVADC_PU_BUFPGA_OFF   = 0,
	eDRVADC_PU_BUFPGA_ON	= 1
} E_DRVADC_PU_BUFPGA;

typedef enum {
	eDRVADC_PU_ZCD_OFF   = 0,
	eDRVADC_PU_ZCD_ON	 = 1
} E_DRVADC_PU_ZCD;

typedef enum {
	eDRVADC_MIC_BIAS_90_VCCA = 0,
	eDRVADC_MIC_BIAS_65_VCCA = 1,
	eDRVADC_MIC_BIAS_75_VCCA = 2,
	eDRVADC_MIC_BIAS_50_VCCA = 3,
	eDRVADC_MIC_BIAS_2p4V = 4,
	eDRVADC_MIC_BIAS_1p7V = 5,
	eDRVADC_MIC_BIAS_2p0V = 6,
	eDRVADC_MIC_BIAS_1p3V = 7
} E_DRVADC_MIC_BIAS_SEL;

#define eDRVADC_MIC_BIAS_SEL_0 eDRVADC_MIC_BIAS_90_VCCA
#define eDRVADC_MIC_BIAS_SEL_1 eDRVADC_MIC_BIAS_65_VCCA
#define eDRVADC_MIC_BIAS_SEL_2 eDRVADC_MIC_BIAS_75_VCCA
#define eDRVADC_MIC_BIAS_SEL_3 eDRVADC_MIC_BIAS_50_VCCA
#define	BIQ_EN	BIQALC_EN

#define AMUX_OFF	   0x00000000
#define AMUX_MIC	   0x00006000
#define AMUX_TEMP	   0x00005000
#define AMUX_GPIO	   0x00004000

typedef enum {
	eDRVADC_MUXP_GPB7_SEL  = 0x08,
	eDRVADC_MUXP_GPB5_SEL  = 0x04,
    eDRVADC_MUXP_GPB3_SEL  = 0x02, 
    eDRVADC_MUXP_GPB1_SEL  = 0x01,
	eDRVADC_MUXP_NONE      = 0x00
} E_DRVADC_MUXP_SEL;

typedef enum {
	eDRVADC_MUXN_GPB7_SEL  = 0x80,
	eDRVADC_MUXN_GPB6_SEL  = 0x40,
	eDRVADC_MUXN_GPB5_SEL  = 0x20, 
	eDRVADC_MUXN_GPB4_SEL  = 0x10,
	eDRVADC_MUXN_GPB3_SEL  = 0x08,
	eDRVADC_MUXN_GPB2_SEL  = 0x04,
	eDRVADC_MUXN_GPB1_SEL  = 0x02, 
	eDRVADC_MUXN_GPB0_SEL  = 0x01,
	eDRVADC_MUXN_NONE      = 0x00
} E_DRVADC_MUXN_SEL;

typedef enum {
	eDRVADC_SINGLE_END_CH0_IN_N   = 0, 
	eDRVADC_SINGLE_END_CH1_IN_N   = 1,
	eDRVADC_SINGLE_END_CH1_IN_P   = 2,
	eDRVADC_SINGLE_END_CH2_IN_N   = 3, 
	eDRVADC_SINGLE_END_CH3_IN_N   = 4,
	eDRVADC_SINGLE_END_CH3_IN_P   = 5,
	eDRVADC_SINGLE_END_CH4_IN_N   = 6, 
	eDRVADC_SINGLE_END_CH5_IN_N   = 7,
	eDRVADC_SINGLE_END_CH5_IN_P   = 8,
	eDRVADC_SINGLE_END_CH6_IN_N   = 9, 
	eDRVADC_SINGLE_END_CH7_IN_N   = 10,
	eDRVADC_SINGLE_END_CH7_IN_P   = 11,
	eDRVADC_DIFFERENTIAL_CH01     = 12,
	eDRVADC_DIFFERENTIAL_CH23     = 13,
	eDRVADC_DIFFERENTIAL_CH45     = 14,
	eDRVADC_DIFFERENTIAL_CH67     = 15,
	eDRVADC_DIFFERENTIAL	      = 16
} E_DRVADC_INPUT_MODE;

typedef enum {
	eDRVADC_MIC   = 0,
	eDRVADC_GPIO  = 1,
	eDRVADC_TEMP  = 2
} E_DRVADC_INPUT_SRC;

typedef enum {
	eDRVADC_OSR_64   = 0,
	eDRVADC_OSR_128  = 1,
	eDRVADC_OSR_192  = 2,
	eDRVADC_OSR_384	 = 3
} E_DRVADC_OSR;

typedef enum {
	eDRVADC_LESS_THAN        = 0, 
	eDRVADC_GREATER_OR_EQUAL = 1
} E_DRVADC_COMP_CONDITION;

typedef enum {
	eDRVADC_BIQ_IN_ADC   = 0,
	eDRVADC_BIQ_IN_DPWM	 = 1
} E_DRVADC_BIQ_SELPATH;

typedef struct {
	uint8_t u8AdcDivisor;
	uint8_t u8SDAdcDivisor; 
	E_DRVADC_OSR eOSR; 
	E_DRVADC_INPUT_SRC eInputSrc;
	E_DRVADC_INPUT_MODE eInputMode;
	uint8_t u8ADCFifoIntLevel; 
} S_DRVADC_PARAM;

typedef void (DRVADC_ADC_CALLBACK)(uint32_t u32UserData);
typedef void (DRVADC_ADCMP0_CALLBACK)(uint32_t u32UserData);
typedef void (DRVADC_ADCMP1_CALLBACK)(uint32_t u32UserData);

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_AnaOpen(void);
void DrvADC_AnaClose(void);
void DrvADC_SetAMUX(uint32_t u32AMUXSel, E_DRVADC_MUXP_SEL eMUXPSel, E_DRVADC_MUXN_SEL eMUXNSel);
void DrvADC_SetVMID(
	E_DRVADC_PULLDOWN ePULLDOWN,
	E_DRVADC_PDLORES  ePDLORES,
	E_DRVADC_PDHIRES  ePDHIRES
);
void DrvADC_SetMIC(BOOL bEnable, E_DRVADC_MIC_BIAS_SEL eMIC_BIAS_SEL);
void DrvADC_SetPGA(
	E_DRVADC_REF_SEL   eREF_SEL,
	E_DRVADC_PU_PGA    ePU_PGA,
	E_DRVADC_PU_BOOST  ePU_BOOST,
	E_DRVADC_BOOSTGAIN eBOOSTGAIN);

int32_t DrvADC_SetPGAGaindB(int32_t i32PGAGainIndB);
void DrvADC_PGAMute(E_DRVADC_MUTE ePGAStage);
void DrvADC_PGAUnMute(E_DRVADC_MUTE ePGAStage);
void DrvADC_SetPower(
	E_DRVADC_PU_MOD ePU_MOD, 
	E_DRVADC_PU_IBGEN ePU_IBGEN, 
	E_DRVADC_PU_BUFADC ePU_BUFADC, 
	E_DRVADC_PU_BUFPGA ePU_PGA, 
	E_DRVADC_PU_ZCD ePU_ZCD);

ERRCODE DrvADC_Open(S_DRVADC_PARAM *sParam);
void DrvADC_Close(void);
ERRCODE DrvADC_SetAdcChannel(E_DRVADC_INPUT_SRC eInputSrc, E_DRVADC_INPUT_MODE eInputMode);
uint32_t DrvADC_GetConversionRate(void);
uint32_t DrvADC_GetConversionData(void);
void DrvADC_SetAdcDivisor(uint8_t u8AdcDivisor);
void DrvADC_SetAdcOverSamplingClockDivisor(uint8_t u8SDAdcDivisor);
void DrvADC_SetOverSamplingRatio(E_DRVADC_OSR eOSR);
void DrvADC_SetCICGain(uint8_t u8CICGain);
void DrvADC_SetFIFOIntLevel(uint8_t u8ADCFifoIntLevel);
void DrvADC_EnableAdcInt(DRVADC_ADC_CALLBACK Callback, uint32_t u32UserData);
void DrvADC_DisableAdcInt(void);
void DrvADC_PdmaEnable(void);
void DrvADC_PdmaDisable(void);
ERRCODE DrvADC_Adcmp0Enable(E_DRVADC_COMP_CONDITION eCmpCondition, uint16_t u16CmpData, uint8_t u8CmpMatchCount);
void DrvADC_Adcmp0Disable(void);
void DrvADC_EnableAdcmp0Int(DRVADC_ADCMP0_CALLBACK Callback, uint32_t u32UserData);
void DrvADC_DisableAdcmp0Int(void);
ERRCODE DrvADC_Adcmp1Enable(E_DRVADC_COMP_CONDITION eCmpCondition, uint16_t u16CmpData, uint8_t u8CmpMatchCount);
void DrvADC_Adcmp1Disable(void);
void DrvADC_EnableAdcmp1Int(DRVADC_ADCMP1_CALLBACK Callback, uint32_t u32UserData);
void DrvADC_DisableAdcmp1Int(void);
void ADC_IRQHandler(void);
void DrvADC_StartConvert(void);
void DrvADC_StopConvert(void);

void DrvADC_SetBIQ(uint16_t u16SR_DIV, uint8_t u8PWM_UPSR, E_DRVADC_BIQ_SELPATH eBiqSelPath, uint32_t u32BiqCoeff[15]);
void DrvADC_BIQEnable(void);
void DrvADC_BIQDisable(void);
void DrvADC_RstBIQ(BOOL bFlag);
uint32_t DrvADC_GetVersion(void);


#endif
