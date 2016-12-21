#ifndef _BUFCTRL_H_
#define _BUFCTRL_H_

#include "Platform.h"

#define S_BUF_CTRL_FLAG_BUF			0
#define S_BUF_CTRL_FLAG_CALLBACK	1
#define S_BUF_CTRL_FLAG_INACTIVE	2
#define S_BUF_CTRL_FLAG_MUTE		4
#define S_BUF_CTRL_FLAG_PAUSE		8
	
#define BUF_CTRL_IS_ACTIVE(psBufCtrl)				(((psBufCtrl)->u8Flag & S_BUF_CTRL_FLAG_INACTIVE)==0)
#define BUF_CTRL_IS_INACTIVE(psBufCtrl)				(((psBufCtrl)->u8Flag & S_BUF_CTRL_FLAG_INACTIVE) != 0)	
#define BUF_CTRL_IS_MUTE_OR_INACTIVE(psBufCtrl)		(((psBufCtrl)->u8Flag & (S_BUF_CTRL_FLAG_INACTIVE|S_BUF_CTRL_FLAG_MUTE))!=0)
#define BUF_CTRL_IS_PAUSE_OR_INACTIVE(psBufCtrl)	(((psBufCtrl)->u8Flag & (S_BUF_CTRL_FLAG_INACTIVE|S_BUF_CTRL_FLAG_PAUSE))!=0)
#define BUF_CTRL_IS_PAUSE(psBufCtrl)				(((psBufCtrl)->u8Flag & S_BUF_CTRL_FLAG_PAUSE)!=0)
#define BUF_CTRL_IS_MUTE(psBufCtrl)					(((psBufCtrl)->u8Flag & S_BUF_CTRL_FLAG_MUTE)!=0)
#define BUF_CTRL_ISNOT_CALLBACK(psBufCtrl)			(((psBufCtrl)->u8Flag & S_BUF_CTRL_FLAG_CALLBACK)==0)
#define BUF_CTRL_SET_ACTIVE(psBufCtrl)				((psBufCtrl)->u8Flag &= (~S_BUF_CTRL_FLAG_INACTIVE))
#define BUF_CTRL_SET_INACTIVE(psBufCtrl)			((psBufCtrl)->u8Flag |= S_BUF_CTRL_FLAG_INACTIVE)
#define BUF_CTRL_SET_CALLBACK(psBufCtrl) 			((psBufCtrl)->u8Flag |= S_BUF_CTRL_FLAG_CALLBACK)

#define BUF_CTRL_SET_MUTE(psBufCtrl)				((psBufCtrl)->u8Flag |= S_BUF_CTRL_FLAG_MUTE)
#define BUF_CTRL_SET_UNMUTE(psBufCtrl)				((psBufCtrl)->u8Flag &= (~S_BUF_CTRL_FLAG_MUTE))
#define BUF_CTRL_SET_PAUSE(psBufCtrl)				((psBufCtrl)->u8Flag|=S_BUF_CTRL_FLAG_PAUSE)
#define BUF_CTRL_SET_RESUME(psBufCtrl)				((psBufCtrl)->u8Flag&=(~S_BUF_CTRL_FLAG_PAUSE))

typedef struct sBufCtrl
{
	UINT8 u8Flag;		// should not change this "u8Flag" order, to be the same order as S_CALLBACK_CTRL
	
	UINT16 u16BufCount;					// buffer total count
	INT16  *pi16Buf;					// buffer pointer.(Ring buffer)
	UINT16 u16BufReadIdx;				// buffer read index.
	UINT16 u16BufWriteIdx;				// buffer write index.
	
	UINT16 u16SampleRate;				// Playback sample rate
	UINT16 u16FrameSize;				// frame size that output samples by processing
	UINT16 u16ReSamplingCalculation;	// Meta data for re-sampling calculation
}S_BUF_CTRL;


typedef UINT8 (*PFN_DATA_REQUEST_CALLBACK)(void *pParam, INT16 i16DataBufCount, INT16 ai16DataBuf[]);
typedef struct sCallbackCtrl
{
	UINT8 u8Flag;		// should not change this "u8Flag" order, to be the same order as S_BUF_CTRL
	
	PFN_DATA_REQUEST_CALLBACK  pfnFunc;
	UINT8 *pu8Param;
} S_BUF_CTRL_CALLBACK;


void BufCtrl_ReadWithCount(S_BUF_CTRL *psSrc, UINT16 u16ConsumeCount, INT16 *pi16Des);
void BufCtrl_WriteWithCount(S_BUF_CTRL *psDes, UINT16 u16WriteCount, INT16 *pi16Src);

#endif
