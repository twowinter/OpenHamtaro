#ifndef _AUDIOMIXER_H_
#define _AUDIOMIXER_H_

#include "ConfigApp.h"
#include "PlaybackRecord.h"


#define AUDIOMIXER_SAME_SAMPLE_RATE		0

#define AUDIOMIXER_BUL_CTRL_END		((S_BUF_CTRL*)0xffffffff)

UINT8 AudioMixer_MixProcess(S_BUF_CTRL **pParam, INT16 i16DataBufCount, INT16 ai16DataBuf[]);
// Return the largest sample rate from a lot of S_BUF_CTRL
UINT32 AudioMixer_Initiate(S_BUF_CTRL_CALLBACK *psMixerCtl, S_BUF_CTRL **ppsInBufCtrlList);
// Return the largest sample rate from a lot of S_BUF_CTRL
UINT32 AudioMixer_ChangeSR(S_BUF_CTRL **ppsInBufCtrlList);

#endif
