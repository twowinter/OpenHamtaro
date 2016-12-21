#include "BufCtrl.h"

void BufCtrl_ReadWithCount(S_BUF_CTRL *psSrc, UINT16 u16ReadCount, INT16 *pi16Des)
{
	while ( u16ReadCount )
	{
		u16ReadCount --;
		*pi16Des ++ = psSrc->pi16Buf[psSrc->u16BufReadIdx ++];
		if (psSrc->u16BufReadIdx>=psSrc->u16BufCount)
			psSrc->u16BufReadIdx = 0;		
	}
}

void BufCtrl_WriteWithCount(S_BUF_CTRL *psDes, UINT16 u16WriteCount, INT16 *pi16Src)
{
	while ( u16WriteCount )
	{
		u16WriteCount --;
		psDes->pi16Buf[psDes->u16BufReadIdx ++] = *pi16Src ++;
		if (psDes->u16BufWriteIdx >= psDes->u16BufCount)
			psDes->u16BufWriteIdx = 0;		
	}
}
