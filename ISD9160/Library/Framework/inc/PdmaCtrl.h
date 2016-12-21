#ifndef _PDMACTRL_H_
#define _PDMACTRL_H_

#include "Platform.h"


void PdmaCtrl_Init(void);

void PdmaCtrl_Open(
	UINT32 u32Ch,
	UINT32 u32ServiceSel
);

void PdmaCtrl_Start(
	UINT32 u32Ch,
	UINT32 *p32SrcAdd,
	UINT32 *p32DesAdd,
	UINT16 u16Count
);

void PdmaCtrl_Stop(UINT32 u32Ch);

void PdmaCtrl_Close(UINT32 u32Ch);

void PdmaCtrl_UnInit(void);
#endif


