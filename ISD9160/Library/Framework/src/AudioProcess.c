#include "AudioProcess.h"

#ifdef USE_AUDIOPROCESS_IRQ

S_AUDIOPROCESS_CTRL *g_psAudioProcessCtrlList;

void AudioProcess_InitiateUseIRQ(void)
{
	NVIC_EnableIRQ(AUDIOPROCESS_PRIORITY_0);
	NVIC_SetPriority(AUDIOPROCESS_PRIORITY_0, AUDIOPROCESS_IRQ_HIGHEST_PRIORITY);
	
	NVIC_EnableIRQ(AUDIOPROCESS_PRIORITY_1);
	NVIC_SetPriority(AUDIOPROCESS_PRIORITY_1, AUDIOPROCESS_IRQ_LOWEST_PRIORITY);
}

void AudioProcess_ConfigureUseIRQ(
	S_AUDIOPROCESS_CTRL *psProcessCtrlList
)
{
	g_psAudioProcessCtrlList = psProcessCtrlList;
}

void AudioProcess_CheckAndTrigIRQ(UINT16 u16IncreaseCount)
{
	S_AUDIOPROCESS_CTRL *psProcessCtrl;
	
	psProcessCtrl = g_psAudioProcessCtrlList;
	while(psProcessCtrl->pfnProcess!=NULL)
	{
		if (BUF_CTRL_IS_ACTIVE(psProcessCtrl->psOutBufCtrl->u8Flag))
		{
			if (*psProcessCtrl->pu16IrqTrigCounter >= psProcessCtrl->u16IrqTrigThreshold)
			{
				NVIC_SetPendingIRQ((IRQn_Type)psProcessCtrl->i8Priority);

				*psProcessCtrl->pu16IrqTrigCounter = 0;
			}
			*psProcessCtrl->pu16IrqTrigCounter += u16IncreaseCount;
		}
		psProcessCtrl ++;
	}
}

BOOL AudioProcess_Run( INT8 i8Priority )
{
	UINT8 u8ActiveProcessCount;
	S_AUDIOPROCESS_CTRL *psProcessCtrlList;
	
	psProcessCtrlList = g_psAudioProcessCtrlList;
	if ( psProcessCtrlList == NULL )
		return FALSE;
	
	u8ActiveProcessCount = 0;
	while(psProcessCtrlList->pfnProcess != NULL)
	{
		if ( psProcessCtrlList->i8Priority != i8Priority )
		{
			if (BUF_CTRL_IS_ACTIVE(psProcessCtrlList->psOutBufCtrl->u8Flag))
				u8ActiveProcessCount ++;
		}
		else	
		{
			if ( psProcessCtrlList->pfnProcess(psProcessCtrlList->pu8AudioPocessParam) == TRUE )
				u8ActiveProcessCount ++;
		}
		psProcessCtrlList ++;
	}
	
	if ( u8ActiveProcessCount )
		return TRUE;
	
	return FALSE;
}

#else
BOOL AudioProcess_Run( S_AUDIOPROCESS_CTRL *psProcessCtrlList )
{
	UINT8 u8ActiveProcessCount;
	
	if ( psProcessCtrlList == NULL )
		return FALSE;
	
	
	u8ActiveProcessCount = 0;
	while(psProcessCtrlList->pfnProcess != NULL)
	{
		if ( psProcessCtrlList->pfnProcess(psProcessCtrlList->pu8AudioPocessParam) == TRUE )
			u8ActiveProcessCount ++;

		psProcessCtrlList ++;
	}
	
	if ( u8ActiveProcessCount )
		return TRUE;
	
	return FALSE;
}
#endif


