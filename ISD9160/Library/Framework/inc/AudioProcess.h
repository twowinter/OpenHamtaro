#ifndef _AUDIOPROCESS_H_
#define _AUDIOPROCESS_H_

#include "ConfigApp.h"
#include "BufCtrl.h"

typedef BOOL (*PFN_AUDIOPROCESS)( void *pu8PocessParam );
typedef struct
{
	PFN_AUDIOPROCESS pfnProcess;		// The address of audio process function
	void *pu8AudioPocessParam;
#ifdef USE_AUDIOPROCESS_IRQ
	S_BUF_CTRL *psOutBufCtrl;
	UINT16	*pu16IrqTrigCounter;
	UINT16	u16IrqTrigThreshold;
	INT8 i8Priority;
#endif
} S_AUDIOPROCESS_CTRL;

#ifdef USE_AUDIOPROCESS_IRQ
	#define AUDIOPROCESS_PRIORITY_0		TMR0_IRQn
	#define AUDIOPROCESS_PRIORITY_1		PWRWU_IRQn
	#define AUDIOPROCESS_PRIORITY_ALL	(UINT8)-1

	#define AUDIOPROCESS_IRQ_HIGHEST_PRIORITY	1
	#define AUDIOPROCESS_IRQ_LOWEST_PRIORITY	2

	#define AUDIOPROCESS_DELARE(pfnProcess, pu8AudioPocessParam, psBufCtrl, pu16IrqTrigCounter, u16IrqTrigThreshold, i8IrqPriority)	\
		{(PFN_AUDIOPROCESS)pfnProcess, (void*)pu8AudioPocessParam, psBufCtrl, pu16IrqTrigCounter, u16IrqTrigThreshold, i8IrqPriority}
	#define AUDIOPROCESS_CRITICAL_SECTION_ENTRY() \
		NVIC_DisableIRQ(AUDIOPROCESS_PRIORITY_0);	\
		NVIC_DisableIRQ(AUDIOPROCESS_PRIORITY_1);
		
	#define AUDIOPROCESS_CRITICAL_SECTION_EXIT()	\
		NVIC_EnableIRQ(AUDIOPROCESS_PRIORITY_0);	\
		NVIC_EnableIRQ(AUDIOPROCESS_PRIORITY_1);

	void AudioProcess_InitiateUseIRQ(void);
	void AudioProcess_CheckAndTrigIRQ(UINT16 u16IncreaseCount);
	void AudioProcess_ConfigureUseIRQ(S_AUDIOPROCESS_CTRL *psProcessCtrlList);
	BOOL AudioProcess_Run( INT8 i8Priority );

#else
	#define AUDIOPROCESS_DELARE(pfnProcess, pu8AudioPocessParam, psBufCtrl, pu16IrqTrigCounter, u16IrqTrigThreshold, i8IrqPriority)	\
		{(PFN_AUDIOPROCESS)pfnProcess, (void*)pu8AudioPocessParam}

	BOOL AudioProcess_Run( S_AUDIOPROCESS_CTRL *psProcessCtrlList );
#endif

#define AUDIOPROCESS_CTRL_END			AUDIOPROCESS_DELARE(NULL, NULL, NULL, NULL, 0, 0)

#endif

