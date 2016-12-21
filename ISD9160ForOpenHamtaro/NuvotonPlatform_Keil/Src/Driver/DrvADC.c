/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Include related headers                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#include "ISD9xx.h"
#include "core_cm0.h"
#include "DrvADC.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct {
    BOOL                   bOpenFlag;
    DRVADC_ADC_CALLBACK    *g_ptADCCallBack;
    DRVADC_ADCMP0_CALLBACK *g_ptADCMP0CallBack;
    DRVADC_ADCMP1_CALLBACK *g_ptADCMP1CallBack;
    uint32_t g_pu32UserData[3];
} S_DRVADC_TABLE;

S_DRVADC_TABLE gsAdcTable;

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_AnaOpen                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    E_SUCCESS: analog block has been successfully opened.                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Open analog block clock for config.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_AnaOpen(void)
{
	uint32_t LdoFlag,LdoValue;
	LdoFlag = ANA->LDOPD.PD;
	LdoValue= ANA->LDOSET;

    SYSCLK->APBCLK.ANA_EN = 1;
   SYS->IPRSTC2.ANA_RST = 1;
    SYS->IPRSTC2.ANA_RST = 0;   

	ANA->LDOPD.PD=LdoFlag;
	ANA->LDOSET=LdoValue;	

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_AnaClose                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Close analog block clock and return reset state.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_AnaClose(void)
{   
	uint32_t LdoFlag,LdoValue; 
	LdoFlag = ANA->LDOPD.PD;
	LdoValue= ANA->LDOSET;

    SYS->IPRSTC2.ANA_RST = 1;
    SYS->IPRSTC2.ANA_RST = 0;

	if (LdoFlag>0)
	{
		ANA->LDOPD.PD=LdoFlag;
		ANA->LDOSET=LdoValue;
	}
	else
    SYSCLK->APBCLK.ANA_EN = 0;  
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetAMUX                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u32AMUXSel  [in]: Specify analog mux input source.                                                   */
/*                      The source contains MICP/MICN, 8 GPIOs/VMID(VBG), GPIO/GPIO,                       */
/*                      temp sensor input.                                                                 */
/*                      Valid values are: AMUX_MIC, AMUX_TEMP, AMUX_GPIO, AMUX_OFF                         */
/*                                                                                                         */
/*    eMUXPSel    [in]: Specify analog muxp input.                                                         */
/*                      eDRVADC_MUXP_GPB7_SEL                                                              */
/*                      eDRVADC_MUXP_GPB5_SEL                                                              */
/*                      eDRVADC_MUXP_GPB3_SEL                                                              */
/*                      eDRVADC_MUXP_GPB1_SEL                                                              */
/*                                                                                                         */
/*    eMUXNSel    [in]: Specify analog muxn input.                                                         */
/*                      eDRVADC_MUXN_GPB7_SEL                                                              */
/*                      eDRVADC_MUXN_GPB6_SEL                                                              */
/*                      eDRVADC_MUXN_GPB5_SEL                                                              */
/*                      eDRVADC_MUXN_GPB4_SEL                                                              */
/*                      eDRVADC_MUXN_GPB3_SEL                                                              */
/*                      eDRVADC_MUXN_GPB2_SEL                                                              */
/*                      eDRVADC_MUXN_GPB1_SEL                                                              */
/*                      eDRVADC_MUXN_GPB0_SEL                                                              */
/*                                                                                                         */
/*                            BIT       Description                                                        */
/*                            =======================                                                      */
/*                            [14]      MUX_EN                                                             */
/*                            [13]      MIC_SEL                                                            */
/*                            [12]      TEMP_SEL                                                           */
/*                            [11:8]    MUXP_SEL                                                           */
/*                                      MUXP_GPB7_SEL  0x00000800                                          */
/*                                      MUXP_GPB5_SEL  0x00000400                                          */
/*                                      MUXP_GPB3_SEL  0x00000200                                          */
/*                                      MUXP_GPB1_SEL  0x00000100                                          */
/*                                                                                                         */
/*                            [7:0]     MUXN_SEL                                                           */
/*                                      MUXN_GPB7_SEL  0x00000080                                          */
/*                                      MUXN_GPB6_SEL  0x00000040                                          */
/*                                      MUXN_GPB5_SEL  0x00000020                                          */
/*                                      MUXN_GPB4_SEL  0x00000010                                          */
/*                                      MUXN_GPB3_SEL  0x00000008                                          */
/*                                      MUXN_GPB2_SEL  0x00000004                                          */
/*                                      MUXN_GPB1_SEL  0x00000002                                          */
/*                                      MUXN_GPB0_SEL  0x00000001                                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Specify analog mux input source.                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetAMUX(uint32_t u32AMUXSel, E_DRVADC_MUXP_SEL eMUXPSel, E_DRVADC_MUXN_SEL eMUXNSel)
{
    if (u32AMUXSel == AMUX_MIC)
    {
        ANA->AMUX.EN       = 1;
        ANA->AMUX.MIC_SEL  = 1;
        ANA->AMUX.TEMP_SEL = 0;
        ANA->AMUX.MUXP_SEL = 0;
        ANA->AMUX.MUXN_SEL = 0;
    }
    else if (u32AMUXSel == AMUX_TEMP)
    {
        ANA->AMUX.EN       = 1;
        ANA->AMUX.MIC_SEL  = 0;
        ANA->AMUX.TEMP_SEL = 1;
        ANA->AMUX.MUXP_SEL = 0;
        ANA->AMUX.MUXN_SEL = 0;
    }
    else if (u32AMUXSel == AMUX_GPIO)
    {
        ANA->AMUX.EN       = 1;
        ANA->AMUX.MIC_SEL  = 0;
        ANA->AMUX.TEMP_SEL = 0;
        ANA->AMUX.MUXP_SEL = eMUXPSel;
        ANA->AMUX.MUXN_SEL = eMUXNSel;
    }
    else
    {
        ANA->AMUX.EN       = 0;
        ANA->AMUX.MIC_SEL  = 0;
        ANA->AMUX.TEMP_SEL = 0;
        ANA->AMUX.MUXP_SEL = 0;
        ANA->AMUX.MUXN_SEL = 0;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetVMID                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    ePULLDOWN     [in]: VMID Pulldwon                                                                    */
/*                        eDRVADC_PULLDOWN_VMID_RELEASE                                                    */
/*                             - Release VMID pin for reference operation.                                 */
/*                        eDRVADC_PULLDOWN_VMID_GND                                                        */
/*                             - Pull VMID pin to ground. Default powerdown and reset condition.           */
/*    ePDLORES      [in]: Power Down Low(4.8Kohm) resistance reference                                     */
/*                        eDRVADC_PDLORES_CONNECTED                                                        */
/*                             - Connect the low resistance reference to VMID. Use this setting for fast   */
/*                               power up of VMID . Can be turned off after 50ms to save power.            */
/*                        eDRVADC_PDLORES_DISCONNECTED                                                     */
/*                             - The low resistance reference is disconnected from VMID. Default powerdown */
/*                               and reset condition.                                                      */
/*    ePDHIRES      [in]: Power Down High(360Kohm) resistance reference                                    */
/*                        eDRVADC_PDHIRES_CONNECTED                                                        */
/*                             - Connect the high resistance reference to VMID. Use this setting for       */
/*                               minimum power consumption.                                                */
/*                        eDRVADC_PDHIRES_DISCONNECTED                                                     */
/*                             - The high resistance reference is disconnected from VMID. Default          */
/*                               powerdown and reset condition.                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the pull high or pull low resistor reference for VMID.                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetVMID(
    E_DRVADC_PULLDOWN ePULLDOWN,
    E_DRVADC_PDLORES  ePDLORES,
    E_DRVADC_PDHIRES  ePDHIRES
)
{
    ANA->VMID.PULLDOWN = ePULLDOWN;
    ANA->VMID.PDLORES  = ePDLORES;
    ANA->VMID.PDHIRES  = ePDHIRES;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetMIC                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    bEnable           [in]: Specify microphone bias enable status.                                       */
/*    eMIC_BIAS_SEL     [in]: Specify microphone bias voltage.                                             */
/*                                                                                                         */
/*                            BIT       Description                                                        */
/*                            =======================                                                      */
/*                            [2]       MODE                                                               */
/*                            [1:0]     VSEL                                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable/disable the microphone and set the bias voltage.                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetMIC(BOOL bEnable, E_DRVADC_MIC_BIAS_SEL eMIC_BIAS_SEL)
{
    if (bEnable)
        ANA->MICBEN = 1;
    else
        ANA->MICBEN = 0;

    ANA->MICBSEL = eMIC_BIAS_SEL;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetPGA                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    eREF_SEL          [in]: Select reference for analog path.                                            */
/*                            eDRVADC_REF_SEL_VMID: Select VMID voltage as analog ground reference.        */
/*                            eDRVADC_REF_SEL_VBG: Select Bandgap voltage as analog ground reference.      */
/*    ePU_PGA           [in]: Specify PGA power status.                                                    */
/*                            eDRVADC_PU_PGA_OFF: power down                                               */
/*                            eDRVADC_PU_PGA_ON: power up                                                  */
/*    ePU_BOOST         [in]: Specify boost stage power status.                                            */
/*                            eDRVADC_PU_BOOST_OFF: power down                                             */
/*                            eDRVADC_PU_BOOST_ON: power up                                                */
/*    eBOOSTGAIN        [in]: Specify boost gain setting.                                                  */
/*                            eDRVADC_BOOSTGAIN_0DB: gain = 0 dB                                           */
/*                            eDRVADC_BOOSTGAIN_26DB: gain = 26 dB                                         */
/*                                                                                                         */
/*                            BIT         Description                                                      */
/*                            =========================                                                    */
/*                            [3]         BOOSTGAIN                                                        */
/*                            [2]         PU_IPBOOST                                                       */
/*                            [1]         PU_PGA                                                           */
/*                            [0]         REF_SEL                                                          */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set boost gain, boost stage/PGA power status and Vref of PGA.                                        */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetPGA(
    E_DRVADC_REF_SEL   eREF_SEL,
    E_DRVADC_PU_PGA    ePU_PGA,
    E_DRVADC_PU_BOOST  ePU_BOOST,
    E_DRVADC_BOOSTGAIN eBOOSTGAIN
)
{
    ANA->PGAEN.REF_SEL    = eREF_SEL;
    ANA->PGAEN.PU_PGA     = ePU_PGA;
    ANA->PGAEN.PU_IPBOOST = ePU_BOOST;
    ANA->PGAEN.BOOSTGAIN  = eBOOSTGAIN;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetPGAGaindB                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    i32PGAGainIndB        [in]: Specify PGA gain in dB. The Scale is 100.                                */
/*                                                                                                         */
/*                                i32PGAGainIndB/100 = u32PGAGain * 0.75 - 12                              */
/*                                                                                                         */
/*                                i32PGAGainIndB    mapping dB                                             */
/*                                =============================                                            */
/*                                3525           -> 35.25 dB                                               */
/*                                 100           -> 1.00 dB                                                */
/*                                   0           -> 0 dB                                                   */
/*                               -1200           -> -12.00 dB                                              */
/* Returns:                                                                                                */
/*    PGA gain in dB after scale 100.                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set PGA gain.                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvADC_SetPGAGaindB(int32_t i32PGAGainIndB)
{
    if(i32PGAGainIndB < -1200)
        i32PGAGainIndB = -1200;
    else if(i32PGAGainIndB > 3525)
        i32PGAGainIndB = 3525;
    
    ANA->PGA_GAIN.GAIN  = ((i32PGAGainIndB + 1200) / 75);
    
    return ((ANA->PGA_GAIN.GAIN & 0x3f) * 75  - 1200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_PGAMute                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    ePGAStage   [in] : Specify PGA stage.                                                                */
/*                         Valid values are: eDRVADC_MUTE_PGA, eDRVADC_MUTE_IPBOOST                        */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Mute the PGA pre-amplify or boost stage.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_PGAMute(E_DRVADC_MUTE ePGAStage)
{
    if (ePGAStage == eDRVADC_MUTE_PGA)
        ANA->SIGCTRL.MUTE_PGA = 0x1;
    if (ePGAStage == eDRVADC_MUTE_IPBOOST)
        ANA->SIGCTRL.MUTE_IPBOOST = 0x1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_PGAUnMute                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u32PGAStage   [in] : Specify PGA stage.                                                              */ 
/*                         Valid values are: eDRVADC_MUTE_PGA eDRVADC_MUTE_IPBOOST                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Unmute the PGA pre-amplify or boost stage.                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_PGAUnMute(E_DRVADC_MUTE ePGAStage)
{
    if (ePGAStage == eDRVADC_MUTE_PGA)
        ANA->SIGCTRL.MUTE_PGA = 0x0;
    if (ePGAStage == eDRVADC_MUTE_IPBOOST)
        ANA->SIGCTRL.MUTE_IPBOOST = 0x0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetPower                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    ePU_MOD          [in] : Specify ADC sigma-delta modulator power status.                              */
/*                            ePU_MOD_ON: power up     ePU_MOD_OFF: power down                             */
/*    ePU_IBGEN        [in] : Specify current bias generation power status.                                */
/*                            ePU_IBGEN_ON: power up   ePU_IBGEN_OFF: power down                           */
/*    ePU_BUFADC       [in] : Specify ADC reference buffer power status.                                   */
/*                            ePU_BUFADC_ON: power up  ePU_BUFADC_OFF: power down                          */
/*    ePU_BUFPGA       [in] : Specify PGA reference buffer power status.                                   */
/*                            ePU_BUFPGA_ON: power up  ePU_BUFPGA_OFF: power down                          */
/*    ePU_ZCD          [in] : Specify Zero Cross Detect Comparator power status.                           */
/*                            ePU_ZCD_ON: power up     ePU_ZCD_OFF: power down                             */
/*                                                                                                         */
/*                              BIT       Description                                                      */
/*                            =========================                                                    */
/*                              [4]       PU_IBGEN                                                         */
/*                              [3]       PU_MOD                                                           */
/*                              [2]       PU_BUFADC                                                        */
/*                              [1]       PU_BUFPGA                                                        */
/*                              [0]       PU_ZCD                                                           */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Specify ZCD, PGA Buffer, ADC Buffer, ADC MOD, IBG power up status.                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetPower(
    E_DRVADC_PU_MOD ePU_MOD, 
    E_DRVADC_PU_IBGEN ePU_IBGEN, 
    E_DRVADC_PU_BUFADC ePU_BUFADC, 
    E_DRVADC_PU_BUFPGA ePU_BUFPGA, 
    E_DRVADC_PU_ZCD ePU_ZCD)
{
    ANA->SIGCTRL.PU_MOD    = ePU_MOD;
    ANA->SIGCTRL.PU_IBGEN  = ePU_IBGEN;
    ANA->SIGCTRL.PU_BUFADC = ePU_BUFADC;
    ANA->SIGCTRL.PU_BUFPGA = ePU_BUFPGA;
    ANA->SIGCTRL.PU_ZCD    = ePU_ZCD;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Open                                                                                              */
/*                                                                                                                    */
/* Parameters:                                                                                                        */
/*   sParam              [in]:  pointer to structure for config the ADC function.It contains the following parameter. */
/*                                                                                                                    */
/*      u8AdcDivisor         :  Determine the ADC clock frequency.                                                    */
/*                              ADC clock frequency = ADC clock source frequency / ( u8AdcDivisor + 1 )               */
/*                                                                                                                    */
/*      u8SDAdcDivisor       :  Specify oversampling clock divisor. This and eOSR will decide the ADC sampling rate.  */
/*                              Fs = HCLK/SDAdcDivisor/OSR                                                            */
/*                                                                                                                    */
/*      eOSR                 :  Specify SDADC over sampling ratio. Valid values are:                                  */
/*                                  eDRVADC_OSR_64                                                                    */
/*                                  eDRVADC_OSR_128                                                                   */
/*                                  eDRVADC_OSR_192                                                                   */
/*                                  eDRVADC_OSR_384                                                                   */
/*                                                                                                                    */
/*      u8ADCFifoIntLevel    :  Specify the FIFO interrupt level.                                                     */
/*                                                                                                                    */
/*      eInputSrc            :  Specify the analog input source. It includes:                                         */
/*                              eDRVADC_MIC                                                                           */
/*                              eDRVADC_TEMP                                                                          */
/*                              eDRVADC_GPIO                                                                          */
/*                                                                                                                    */
/*      eInputMode           :  Specify the analog input mode and channel, It is used only when                       */
/*                              eInputSrc == eDRVADC_GPIO.The input mode contains:                                    */
/*                              eDRVADC_SINGLE_END_CH0_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH1_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH1_IN_P                                                           */
/*                              eDRVADC_SINGLE_END_CH2_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH3_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH3_IN_P                                                           */
/*                              eDRVADC_SINGLE_END_CH4_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH5_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH5_IN_P                                                           */
/*                              eDRVADC_SINGLE_END_CH6_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH6_IN_N                                                           */
/*                              eDRVADC_SINGLE_END_CH7_IN_P                                                           */
/*                              eDRVADC_DIFFERENTIAL_CH01                                                             */
/*                              eDRVADC_DIFFERENTIAL_CH23                                                             */
/*                              eDRVADC_DIFFERENTIAL_CH45                                                             */
/*                              eDRVADC_DIFFERENTIAL_CH67                                                             */
/*                              eDRVADC_DIFFERENTIAL                                                                  */
/* Returns:                                                                                                           */
/*   E_DRVADC_ARGUMENT : Argument is invalid.                                                                         */
/*   E_SUCCESS: ADC has been successfully opened.                                                                     */
/*                                                                                                                    */
/* Description:                                                                                                       */
/*   Enable the ADC function. Configure the corresponding pins of the specified channels as analog input pins.        */
/*--------------------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_Open(S_DRVADC_PARAM *sParam)
{
    /* reset ADC */
    SYS->IPRSTC2.ADC_RST = 1;
    SYS->IPRSTC2.ADC_RST = 0;   
    
    /* Set ADC divisor */
    SYSCLK->CLKDIV.ADC_N = sParam->u8AdcDivisor;

    /* ADC engine clock enable */
    SYSCLK->APBCLK.ADC_EN = 1;

    /* ADC enable */
    SDADC->EN = 1;

    /* Sampling rate setting : Fs = HCLK/SDAdcDivisor/OSR */
    if((sParam->u8SDAdcDivisor > 48) || (sParam->u8SDAdcDivisor < 4))
        return E_DRVADC_ARGUMENT;

    SDADC->CLK_DIV = sParam->u8SDAdcDivisor;
    SDADC->DEC.OSR = sParam->eOSR;

    /* Set FIFO interrupt level */
    if(sParam->u8ADCFifoIntLevel > 7) sParam->u8ADCFifoIntLevel = 7;
    SDADC->INT.FIFO_IE_LEV = sParam->u8ADCFifoIntLevel;

    /* Set BIQ to default state */
    BIQ->BIQ_CTRL.RSTn = 1;
    
    /* Set adc input channel */
    DrvADC_SetAdcChannel(sParam->eInputSrc, sParam->eInputMode);

    gsAdcTable.bOpenFlag = TRUE;

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Close                                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable the ADC function, ADC engine clock and ADC interrupt.                                        */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_Close(void)
{   
    /* disable ADC interrupt */
    NVIC_DisableIRQ(ADC_IRQn);

    /* reset ADC */
    SYS->IPRSTC2.ADC_RST = 1;
    SYS->IPRSTC2.ADC_RST = 0;

    /* ADC engine clock disable */
    SYSCLK->APBCLK.ADC_EN = 0;         

    gsAdcTable.bOpenFlag = FALSE;
    gsAdcTable.g_ptADCCallBack = NULL;
    gsAdcTable.g_pu32UserData[0] = 0;
    gsAdcTable.g_ptADCMP0CallBack = NULL;
    gsAdcTable.g_pu32UserData[1] = 0;
    gsAdcTable.g_ptADCMP1CallBack = NULL;
    gsAdcTable.g_pu32UserData[2] = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetAdcChannel                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    eInputSrc    [in]:  Specify the analog input source. It includes:                                    */
/*                             eDRVADC_MIC                                                                 */
/*                             eDRVADC_TEMP                                                                */
/*                             eDRVADC_GPIO                                                                */
/*                                                                                                         */
/*    eInputMode   [in]:  Specify the analog input mode and channel, It is used only when                  */
/*                        eInputSrc == eDRVADC_GPIO.                                                       */
/*                             eDRVADC_SINGLE_END_CH0_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH1_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH1_IN_P                                                 */
/*                             eDRVADC_SINGLE_END_CH2_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH3_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH3_IN_P                                                 */
/*                             eDRVADC_SINGLE_END_CH4_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH5_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH5_IN_P                                                 */
/*                             eDRVADC_SINGLE_END_CH6_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH6_IN_N                                                 */
/*                             eDRVADC_SINGLE_END_CH7_IN_P                                                 */
/*                             eDRVADC_DIFFERENTIAL_CH01                                                   */
/*                             eDRVADC_DIFFERENTIAL_CH23                                                   */
/*                             eDRVADC_DIFFERENTIAL_CH45                                                   */
/*                             eDRVADC_DIFFERENTIAL_CH67                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    E_DRVADC_ANA_OPEN : analog block should open before call this function.                              */
/*    E_DRVADC_ARGUMENT : analog input mode not support.                                                   */
/*    E_SUCCESS : set adc channel successfully.                                                            */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Select ADC input channels.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_SetAdcChannel(E_DRVADC_INPUT_SRC eInputSrc, E_DRVADC_INPUT_MODE eInputMode)
{     
    if(SYSCLK->APBCLK.ANA_EN == 0)      /* check if analog block open*/
        return E_DRVADC_ANA_OPEN;

    //outpw(&SYS->GPB_ALT, 0);            /* return GPIOB mode */

    if(eInputSrc == eDRVADC_MIC) {
        DrvADC_SetAMUX(AMUX_MIC, eDRVADC_MUXP_NONE, eDRVADC_MUXN_NONE);
    } else if(eInputSrc == eDRVADC_TEMP) {
        DrvADC_SetAMUX(AMUX_TEMP, eDRVADC_MUXP_NONE, eDRVADC_MUXN_NONE);
    } else if(eInputSrc == eDRVADC_GPIO) {      
        switch(eInputMode)
        {
         case eDRVADC_DIFFERENTIAL_CH01:
               GPIOB->PMD.PMD0 = 0;     /* configure GPB0 as input mode */
               GPIOB->PMD.PMD1 = 0;     /* configure GPB1 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB1_SEL, eDRVADC_MUXN_GPB0_SEL);
         break;

         case eDRVADC_DIFFERENTIAL_CH23:
               GPIOB->PMD.PMD2 = 0;     /* configure GPB2 as input mode */
               GPIOB->PMD.PMD3 = 0;     /* configure GPB3 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB3_SEL, eDRVADC_MUXN_GPB2_SEL);
         break;

         case eDRVADC_DIFFERENTIAL_CH45:
               GPIOB->PMD.PMD4 = 0;     /* configure GPB4 as input mode */
               GPIOB->PMD.PMD5 = 0;     /* configure GPB5 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB5_SEL, eDRVADC_MUXN_GPB4_SEL);
         break;

         case eDRVADC_DIFFERENTIAL_CH67:
               GPIOB->PMD.PMD6 = 0;     /* configure GPB6 as input mode */
               GPIOB->PMD.PMD7 = 0;     /* configure GPB7 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB7_SEL, eDRVADC_MUXN_GPB6_SEL);
         break;

         case eDRVADC_SINGLE_END_CH0_IN_N:
               GPIOB->PMD.PMD0 = 0;     /* configure GPB0 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB0_SEL);
         break;

         case eDRVADC_SINGLE_END_CH1_IN_N:
         case eDRVADC_SINGLE_END_CH1_IN_P:
               GPIOB->PMD.PMD1 = 0;     /* configure GPB1 as input mode */
               if (eInputMode == eDRVADC_SINGLE_END_CH1_IN_N)              
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB1_SEL);
               else
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB1_SEL, eDRVADC_MUXN_NONE);
         break;

         case eDRVADC_SINGLE_END_CH2_IN_N:
               GPIOB->PMD.PMD2 = 0;     /* configure GPB2 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB2_SEL);
         break;

         case eDRVADC_SINGLE_END_CH3_IN_N:
         case eDRVADC_SINGLE_END_CH3_IN_P:
               GPIOB->PMD.PMD3 = 0;     /* configure GPB3 as input mode */
               if (eInputMode == eDRVADC_SINGLE_END_CH3_IN_N)
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB3_SEL);
               else
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB3_SEL, eDRVADC_MUXN_NONE);
         break;

         case eDRVADC_SINGLE_END_CH4_IN_N:
               GPIOB->PMD.PMD4 = 0;     /* configure GPB4 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB4_SEL);
         break;

         case eDRVADC_SINGLE_END_CH5_IN_N:
         case eDRVADC_SINGLE_END_CH5_IN_P:
               GPIOB->PMD.PMD5 = 0;     /* configure GPB5 as input mode */
               if (eInputMode == eDRVADC_SINGLE_END_CH5_IN_N)
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB5_SEL);
               else
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB5_SEL, eDRVADC_MUXN_NONE);
         break;

         case eDRVADC_SINGLE_END_CH6_IN_N:
               GPIOB->PMD.PMD6 = 0;     /* configure GPB6 as input mode */
               DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB6_SEL);
         break;

         case eDRVADC_SINGLE_END_CH7_IN_N:
         case eDRVADC_SINGLE_END_CH7_IN_P:
              GPIOB->PMD.PMD7 = 0;     /* configure GPB7 as input mode */
               if (eInputMode == eDRVADC_SINGLE_END_CH7_IN_N)
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_NONE, eDRVADC_MUXN_GPB7_SEL);
               else
                    DrvADC_SetAMUX(AMUX_GPIO, eDRVADC_MUXP_GPB7_SEL, eDRVADC_MUXN_NONE);
         break;

         default:
            return E_DRVADC_ARGUMENT;   
        }
    }

    return E_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_GetConversionRate                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    Conversion rate.                                                                                     */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Return the A/D conversion rate (sample/second.)                                                      */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvADC_GetConversionRate(void)
{
    uint32_t u32OSR;

    switch(SDADC->DEC.OSR)
    {
        case eDRVADC_OSR_64:
            u32OSR=64;
            break;
        case eDRVADC_OSR_128:
            u32OSR=128;
            break;
        case eDRVADC_OSR_192:
            u32OSR=192;
            break;
        case eDRVADC_OSR_384:
            u32OSR=384;
            break;
        default:
            u32OSR=64;
    }

    return (DrvSYS_GetHCLK()*1000/(SDADC->CLK_DIV)/u32OSR);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_GetConversionData                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    16-bit conversion result.                                                                            */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Get the conversion result of SDADC.                                                                  */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvADC_GetConversionData(void)
{
    return (SDADC->ADCOUT);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetAdcDivisor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u8AdcDivisor [in]:  Specify the divisor value.                                                       */
/*                        ADC clock frequency = ADC clock source frequency / ( u8AdcDivisor + 1 )          */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the divisor value of ADC clock.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetAdcDivisor(uint8_t u8AdcDivisor)
{
    SYSCLK->CLKDIV.ADC_N = u8AdcDivisor;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetAdcOverSamplingClockDivisor                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u8AdcDivisor [in]:  Specify the divisor value.                                                       */
/*                        SDADC clock frequency = ADC clock source frequency / ( u8SDAdcDivisor)           */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the divisor value of SDADC clock.                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetAdcOverSamplingClockDivisor(uint8_t u8SDAdcDivisor)
{
    SDADC->CLK_DIV = u8SDAdcDivisor;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetOverSamplingRatio                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    eOSR         [in]:  Specify the over sampling ratio.                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the over sampling ratio of SDADC                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetOverSamplingRatio(E_DRVADC_OSR eOSR)
{
    SDADC->DEC.OSR = eOSR;
}                                  

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetCICGain                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u8CICGain         [in]:  Specify the CIC filter additional gain.                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the additional digital gain from the decimation filter. An additional gain is applied to signal  */
/*    of u8CICGain/2.                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetCICGain(uint8_t u8CICGain)
{
    SDADC->DEC.GAIN = u8CICGain & 0xf;  
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetFIFOIntLevel                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u8ADCFifoIntLevel [in]:  Specify the FIFO interrupt level.                                           */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Set the interrupt level of SDADC FIFO.                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetFIFOIntLevel(uint8_t u8ADCFifoIntLevel)
{
     SDADC->INT.FIFO_IE_LEV = u8ADCFifoIntLevel & 0x0f;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_EnableAdcInt                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    Callback    [in]:  The callback function of ADC interrupt.                                           */
/*    u32UserData [in]:  The user's data to pass to the callback function.                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable the ADC interrupt and setup callback function.                                                */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_EnableAdcInt(DRVADC_ADC_CALLBACK Callback, uint32_t u32UserData)
{
    gsAdcTable.g_ptADCCallBack = Callback;
    gsAdcTable.g_pu32UserData[0] = u32UserData;
    
    SDADC->INT.IE = 1;
    NVIC_EnableIRQ(ADC_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_DisableAdcInt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable the ADC interrupt.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_DisableAdcInt(void)
{
    gsAdcTable.g_ptADCCallBack = NULL;
    gsAdcTable.g_pu32UserData[0] = 0;
    SDADC->INT.IE = 0;
}

/*------------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_PdmaEnable                                                                                */
/*                                                                                                            */
/* Parameters:                                                                                                */
/*    None.                                                                                                   */
/*                                                                                                            */
/* Returns:                                                                                                   */
/*    None.                                                                                                   */
/*                                                                                                            */
/* Description:                                                                                               */
/*    Enable the PDMA data transfer function. When PDMA transfer is enabled, the IE bit must be set to '0'.   */
/*------------------------------------------------------------------------------------------------------------*/
void DrvADC_PdmaEnable(void)
{
    SDADC->INT.IE = 0;
    SDADC->ADCPDMA.RxDmaEn = 1;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_PdmaDisable                                                                              */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Disable the PDMA data transfer function.                                                               */
/*-----------------------------------------------------------------------------------------------------------*/
void DrvADC_PdmaDisable(void)
{
    SDADC->ADCPDMA.RxDmaEn = 0;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Adcmp0Enable                                                                             */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    eCmpCondition   [in]:  Specify the compare condition.                                                  */
/*                           eDRVADC_LESS_THAN for the condition of "less than the compare data";            */
/*                           eDRVADC_GREATER_OR_EQUAL for the condition of "greater or equal to the compare  */
/*                           data."                                                                          */
/*    u16CmpData      [in]:  Specify the compare data.                                                       */
/*    u8CmpMatchCount [in]:  Specify the compare match count.                                                */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    E_DRVADC_ARGUMENT: one of the input arguments is out of the range.                                     */
/*    E_SUCCESS: the compare function is enabled.                                                            */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Enable the ADC result monitor 0 and configure the necessary settings.                                  */
/*-----------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_Adcmp0Enable(E_DRVADC_COMP_CONDITION eCmpCondition, uint16_t u16CmpData, uint8_t u8CmpMatchCount)
{   
    //if(u16CmpData>0x0FFF)
    //  return E_DRVADC_ARGUMENT;
    
    if(u8CmpMatchCount>0x0F)
        return E_DRVADC_ARGUMENT;
    
    SDADC->ADCMPR[0].CMPCOND   = eCmpCondition;
    SDADC->ADCMPR[0].CMPD      = u16CmpData;
    SDADC->ADCMPR[0].CMPMATCHCNT = u8CmpMatchCount;
    SDADC->ADCMPR[0].CMPEN = 1;
    
    return E_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Adcmp0Disable                                                                            */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Disable the ADC result monitor 0.                                                                      */
/*-----------------------------------------------------------------------------------------------------------*/
void DrvADC_Adcmp0Disable(void)
{
    SDADC->ADCMPR[0].CMPEN = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_EnableAdcmp0Int                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    Callback    [in]:  The callback function of ADC result monitor 0 (compare 0) interrupt.              */
/*    u32UserData [in]:  The user's data to pass to the callback function.                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable the ADC compare 0 interrupt and setup callback function.                                      */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_EnableAdcmp0Int(DRVADC_ADCMP0_CALLBACK Callback, uint32_t u32UserData)
{
    gsAdcTable.g_ptADCMP0CallBack = Callback;
    gsAdcTable.g_pu32UserData[1] = u32UserData;
    SDADC->ADCMPR[0].CMPF = 1;        /* clear the A/D interrupt flags for safe */
    SDADC->ADCMPR[0].CMPIE = 1;
    NVIC_EnableIRQ(ADC_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_DisableAdcmp0Int                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable the ADC compare 0 interrupt.                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_DisableAdcmp0Int(void)
{
    gsAdcTable.g_ptADCMP0CallBack = NULL;
    gsAdcTable.g_pu32UserData[1] = 0;
    SDADC->ADCMPR[0].CMPIE = 0;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Adcmp1Enable                                                                             */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    eCmpCondition   [in]:  Specify the compare condition.                                                  */
/*                           eDRVADC_LESS_THAN for the condition of "less than the compare data";            */
/*                           eDRVADC_GREATER_OR_EQUAL for the condition of "greater or equal to the compare  */
/*                           data."                                                                          */
/*    u16CmpData      [in]:  Specify the compare data.                                                       */
/*    u8CmpMatchCount [in]:  Specify the compare match count.                                                */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    E_DRVADC_ARGUMENT: one of the input arguments is out of the range.                                     */
/*    E_SUCCESS: the compare function is enabled.                                                            */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Enable the ADC result monitor 1 and configure the necessary settings.                                  */
/*-----------------------------------------------------------------------------------------------------------*/
ERRCODE DrvADC_Adcmp1Enable(E_DRVADC_COMP_CONDITION eCmpCondition, uint16_t u16CmpData, uint8_t u8CmpMatchCount)
{   
    //if(u16CmpData>0x0FFF)
    //  return E_DRVADC_ARGUMENT;
    
    if(u8CmpMatchCount>0x0F)
        return E_DRVADC_ARGUMENT;
    
    SDADC->ADCMPR[1].CMPCOND   = eCmpCondition;
    SDADC->ADCMPR[1].CMPD      = u16CmpData;
    SDADC->ADCMPR[1].CMPMATCHCNT = u8CmpMatchCount;
    SDADC->ADCMPR[1].CMPEN = 1;
    
    return E_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_Adcmp1Disable                                                                            */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Disable the ADC result monitor 1.                                                                      */
/*-----------------------------------------------------------------------------------------------------------*/
void DrvADC_Adcmp1Disable(void)
{
    SDADC->ADCMPR[1].CMPEN = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_EnableAdcmp1Int                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    Callback    [in]:  The callback function of ADC result monitor 1 (compare 1) interrupt.              */
/*    u32UserData [in]:  The user's data to pass to the callback function.                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable the ADC compare 1 interrupt and setup callback function.                                      */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_EnableAdcmp1Int(DRVADC_ADCMP1_CALLBACK Callback, uint32_t u32UserData)
{
    gsAdcTable.g_ptADCMP1CallBack = Callback;
    gsAdcTable.g_pu32UserData[2] = u32UserData;
    SDADC->ADCMPR[1].CMPF = 1;        /* clear the A/D interrupt flags for safe */
    SDADC->ADCMPR[1].CMPIE = 1;
    NVIC_EnableIRQ(ADC_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_DisableAdcmp1Int                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable the ADC compare 1 interrupt.                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_DisableAdcmp1Int(void)
{
    gsAdcTable.g_ptADCMP1CallBack = NULL;
    gsAdcTable.g_pu32UserData[2] = 0;
    SDADC->ADCMPR[1].CMPIE = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: ADC_IRQHandler                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    ADC Interrupt Service Routine.                                                                       */
/*    According to the interrupt flags to call the corresponding callback function.                        */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_IRQHandler(void)
{
    if(SDADC->INT.IE==1 && (SDADC->ADCMPR[0].CMPF==0) && (SDADC->ADCMPR[1].CMPF==0))
    {
        if(gsAdcTable.g_ptADCCallBack)
            gsAdcTable.g_ptADCCallBack(gsAdcTable.g_pu32UserData[0]);
        
        /* clear the A/D conversion flag */
        /* user should read adc fifo to clear fifo interrupt */
    }
    
    if(SDADC->ADCMPR[0].CMPF==1)
    {
        if(gsAdcTable.g_ptADCMP0CallBack)
            gsAdcTable.g_ptADCMP0CallBack(gsAdcTable.g_pu32UserData[1]);
        
        /* clear the A/D compare flag 0 */
        SDADC->ADCMPR[0].CMPF=1;
    }
    
    if(SDADC->ADCMPR[1].CMPF==1)
    {
        if(gsAdcTable.g_ptADCMP1CallBack)
            gsAdcTable.g_ptADCMP1CallBack(gsAdcTable.g_pu32UserData[2]);
        
        /* clear the A/D compare flag 1 */
        SDADC->ADCMPR[1].CMPF=1;
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_StartConvert                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    ADC Conversion enabled.                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_StartConvert(void)
{
    SDADC->EN = 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_StopConvert                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Conversion stopped and ADC is reset including FIFO pointers.                                         */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_StopConvert(void)
{
    SDADC->EN = 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_SetBIQ                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    u16SR_DIV         [in]: This is used to program the operating sampling rate of the biq filter.       */
/*                            The sampling rate is defined as 49.152M/(u16SR_DIV+1).                       */
/*                                                                                                         */
/*    u8PWM_UPSR        [in]: This is only used when SELPATH is set to 1. The operating sample rate for the*/
/*                            biq filter will be u8PWM_UPSR*49.152M/(u16SR_DIV+1).                         */
/*                                                                                                         */
/*    eBiqSelPath       [in]: AC path selection for BIQ                                                    */
/*                            eDRVADC_BIQ_IN_ADC : biq filter is in ADC path.                              */
/*                            eDRVADC_BIQ_IN_DPWM : biq filter is in DPWM path.                            */
/*                                                                                                         */
/*    u32BiqCoeff       [in]: BIQ coefficient array for programming the 3 stage BIQ filter. There are 5    */
/*                            coefficeints in each stage. Totally 15 coefficients in 3.16 format.          */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Specify the BIQ operating sampling rate, path and config the BIQ coefficients.                       */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetBIQ(uint16_t u16SR_DIV, uint8_t u8PWM_UPSR, E_DRVADC_BIQ_SELPATH eBiqSelPath, uint32_t u32BiqCoeff[15])
{
    int i;
    DrvADC_RstBIQ(0);
    DrvADC_RstBIQ(1);
	for(i=0;i<=32;i++);

    BIQ->BIQ_CTRL.PRGCOEF = 1;

    BIQ->BIQ_CTRL.SR_DIV = u16SR_DIV & 0x1fff;
    BIQ->BIQ_CTRL.UPSR = u8PWM_UPSR & 0x07;
    BIQ->BIQ_CTRL.SELPWM = eBiqSelPath;

    for(i=0;i<15;i++)
        BIQ->BIQ_COEFF[i] = u32BiqCoeff[i]; 

    BIQ->BIQ_CTRL.PRGCOEF = 0;

    DrvADC_BIQEnable();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_BIQEnable                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Enable the BIQ filter                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_BIQEnable(void)
{
    BIQ->BIQ_CTRL.EN = 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_BIQDisable                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None.                                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Disable the BIQ filter                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_BIQDisable(void)
{
    BIQ->BIQ_CTRL.EN = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_RstBIQ                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    bFlag         [in] : Move BIQ out of reset state                                                     */
/*                         0: BIQ filter is in reset state.                                                */
/*                         1: the default coefficients will be downloaded to the coefficient ram           */
/*                            automatically in 32 internal system clock. Processor must delay enough time  */
/*                            before changing the coefficients or turn the BIQ on.                         */
/* Returns:                                                                                                */
/*    None.                                                                                                */
/*                                                                                                         */
/* Description:                                                                                            */
/*    Determine the BIQ is in reset state or active state.                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void DrvADC_RstBIQ(BOOL bFlag)
{
    BIQ->BIQ_CTRL.RSTn = bFlag;
}

/*-----------------------------------------------------------------------------------------------------------*/
/* Function: DrvADC_GetVersion                                                                               */
/*                                                                                                           */
/* Parameters:                                                                                               */
/*    None.                                                                                                  */
/*                                                                                                           */
/* Returns:                                                                                                  */
/*    Version number.                                                                                        */
/*                                                                                                           */
/* Description:                                                                                              */
/*    Get the version number of ISD9160 ADC driver.                                                          */
/*-----------------------------------------------------------------------------------------------------------*/
uint32_t DrvADC_GetVersion (void)
{
    return DRVADC_VERSION_NUM;
}



