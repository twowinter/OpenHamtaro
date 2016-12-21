#include "PdmaCtrl.h"

//---------------------------------------------------------------------------
// Description:
//		Init PDMA engine clock and NVIC interrupt  
// Argument:
//		None
// Return:
//		None 
//---------------------------------------------------------------------------
void PdmaCtrl_Init(void)
{
	/* Enable PDMA clock */
	CLK_EnableModuleClock(PDMA_MODULE);
	/* Reset IP */
	SYS_ResetModule(PDMA_RST);
	
	/* Enable NVIC interrupt*/
	NVIC_ClearPendingIRQ(PDMA_IRQn);
	NVIC_EnableIRQ(PDMA_IRQn);
}

void PdmaCtrl_Open(
	UINT32 u32Ch,
	UINT32 u32ServiceSel
)
{

	PDMA_T *pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * u32Ch));
	volatile int32_t i = 10;
	
	//PDMA Controller Channel Clock Enable
	PDMA_Open( 1<<u32Ch );	
	//Writing 1 to this bit will reset the internal state machine and pointers
	pdma->DSCT_CTL |= PDMA_DSCT_CTL_SWRST_Msk;
	while(i--);                                  //Need a delay to allow reset
	
	
	//PDMA channel is connected to which peripheral transmit request.
	PDMA_SetTransferMode( u32Ch,(u32ServiceSel>>24), 0, 0 );
	//Transfer Source address is incremented/fixed/wrapped.
	//Transfer Destination Address is incremented/fixed/wrapped.
	PDMA_SetTransferAddr( u32Ch, 0, (u32ServiceSel&PDMA_DSCT_CTL_SASEL_Msk), 0, (u32ServiceSel&PDMA_DSCT_CTL_DASEL_Msk));
	//8/16/32 bits is transferred for every PDMA operation	
	PDMA_SetTransferCnt( u32Ch, (u32ServiceSel&PDMA_DSCT_CTL_TXWIDTH_Msk), 0 );
	//Memory to IP mode (SRAM-to-APB)/(SRAM-to-APB)/(SRAM-to-SRAM).
	PDMA_SetTransferDirection( u32Ch, (u32ServiceSel&PDMA_DSCT_CTL_MODESEL_Msk) );	
	//Wrap Interrupt: Both half and end buffer.
	pdma->DSCT_CTL |= (u32ServiceSel&PDMA_DSCT_CTL_WAINTSEL_Msk);
	
	//Interrupt Enable, shot or wrap
	PDMA_CLR_CH_INT_FLAG(u32Ch, 0x00000503);
	PDMA_EnableInt( u32Ch, (u32ServiceSel&0x00000f00)>>8);

}

void PdmaCtrl_Start(
	UINT32 u32Ch,
	UINT32 *p32SrcAdd,
	UINT32 *p32DesAdd,
	UINT16 u16Count
)
{
	PDMA_SET_SRC_ADDR(u32Ch, (uint32_t)p32SrcAdd); 
	PDMA_SET_DST_ADDR(u32Ch, (uint32_t)p32DesAdd);
	PDMA_SetTransferCnt( u32Ch, PDMA_WIDTH_16, u16Count );
	PDMA_Trigger(u32Ch);
}

void PdmaCtrl_Stop(UINT32 u32Ch)
{
	PDMA_STOP(u32Ch);
}

void PdmaCtrl_Close(UINT32 u32Ch)
{
	PDMA_STOP(u32Ch);
	PDMA_GCR->GLOCTL &= ~((1<<u32Ch)<<8);
}

void PdmaCtrl_UnInit(void)
{
	/* Enable NVIC interrupt*/
	NVIC_ClearPendingIRQ(PDMA_IRQn);
	NVIC_DisableIRQ(PDMA_IRQn);
	/* Disable PDMA clock */
	CLK_DisableModuleClock(PDMA_MODULE);
}
