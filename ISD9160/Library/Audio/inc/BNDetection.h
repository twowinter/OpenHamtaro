#ifndef __BNDETECTION_H__
#define __BNDETECTION_H__

// --------------------------------------------------------------
// Include Header Files
// --------------------------------------------------------------
#include "MidiSynthesizer.h"
#include "VoiceChange.h"

#ifdef  __cplusplus
extern "C"
{
#endif
	 
//defined in "AudioCommon.h"
typedef UINT32 (*PFN_AUDIO_DATAREQUEST)(void * pDesAddr, UINT32 u32Position, UINT32 u32ByteNum);
typedef UINT32 (*PFN_AUDIO_USREVENT)(UINT16 u16EventIndex, UINT16 u16EventSubIndex);
typedef INT32 (*PFN_AUDIO_DECODEPROCESS)( UINT8 *pu8DecodeWorkBuf, UINT8 *pu8DecodeTempBuf, INT16 *pi16DecodedPcmBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback, PFN_AUDIO_USREVENT PFN_AUDIO_USREVENT);

// --------------------------------------------------------------
// Version Definition and Error Code
// --------------------------------------------------------------
#define	BNDET_MAJOR_NUM 3
#define	BNDET_MINOR_NUM 10
#define	BNDET_BUILD_NUM 006
#define BNDET_VERSION_NUM	_SYSINFRA_VERSION(BNDET_MAJOR_NUM, BNDET_MINOR_NUM, BNDET_BUILD_NUM)


#define BNDETECTION_OUTPUT_MIDI			0x1
#define BNDETECTION_OUTPUT_PCM			0x2
#define BNDETECTION_OUTPUT_BEAT			0x4
#define BNDETECTION_OUTPUT_NOTE_BEAT	0x8
#define BNDETECTION_OUTPUT_FREQ			0x10

#define ROBOTSOUND_OPTION_REDUCE_LIGHT			0x1
#define ROBOTSOUND_OPTION_TRIANGLE_ENVELOPE		0x2
//#define ROBOTSOUND_OPTION_RECTANGLE_SYNTH		0x4
#define ROBOTSOUND_OPTION_SINE_ENVELOPE			0x8

#define PITCHCHANGELFO_OPTION_TRIANGLE_ENVELOPE		0x2
//#define PITCHCHANGELFO_OPTION_RECTANGLE_SYNTH		0x4
#define PITCHCHANGELFO_OPTION_SINE_ENVELOPE			0x8

#define TALKTOSING_AUTOTUNE_PITCH				0x0001
#define	TALKTOSING_ONE2ONE_SYLB					0x0002
//#define TALKTOSING_DOUBLE_OUTPUT_SAMPLE_RATE	0x0004		
#define TALKTOSING_TUNE_SYLB_AVG_PITCH			0x0008
#define TALKTOSING_CLASSIC_AUTOTUNE				0x0010
//#define TALKTOSING_ONE2ONE_PHRASE				0x0020	// for midi format only
#define TALKTOSING_SYLB_NOTE_BEST_MATCH			0x0040
#define TALKTOSING_SUPRESS_SILENCE				0x0080
#define TALKTOSING_SUPRESS_UNVOICE				0x0100
#define TALKTOSING_SUPRESS_LOWVOICE				0x0200
//#define TALKTOSING_BLUR_SND						0x0400
//#define TALKTOSING_SYLB_NOTE_BEST_MATCH_PHRASE	0x80

//#define TALKTOSING_LOOP_IN_PHRASE_NORMAL		0x0
//#define TALKTOSING_LOOP_IN_PHRASE_RESTART		0x01
//#define TALKTOSING_LOOP_IN_PHRASE_ONETIME		0x02


typedef enum
{
	eBNDETECTION_MODE_CHILD = 0,
	eBNDETECTION_MODE_WOMAN = 1,
	eBNDETECTION_MODE_MAN = 2,
	eBNDETECTION_MODE_INSTRUMENT = 3
} E_BNDETECTION_MODE;

typedef enum 
{
	eBNDETECTION_BEAT_NO 		= 0,
	eBNDETECTION_BEAT_START_HI 	= 1,
	eBNDETECTION_BEAT_HI 		= 2,
	eBNDETECTION_BEAT_LO 		= 3,
	eBNDETECTION_BEAT_START_NO 	= 4,
	eBNDETECTION_BEAT_THD_HI 	= 5,
	eBNDETECTION_BEAT_THD_NO 	= 6,
	eBNDETECTION_NONE		 	= 7
} E_BNDETECTION_RESULT;

typedef enum
{
	eBNDETECTION_PITCHBEND_NONE = 0,
	eBNDETECTION_PITCHBEND_SINGLE = 1,
	eBNDETECTION_PITCHBEND_MULTIPLE = 2
} E_BNDETECTION_PITCHBEND_MODE;


typedef enum
{
	eBNDETECTION_FILTER_NONE = 0,
	eBNDETECTION_FILTER_LOW = 1,
	eBNDETECTION_FILTER_HIGH = 2,
	eBNDETECTION_FILTER_LOW_APPLY = 3,
	eBNDETECTION_FILTER_HIGH_APPLY = 4,
} E_BNDETECTION_FILTER_MODE;


#define AUTOTUNE_PITCH_TABLE_SIZE		378

typedef enum
{
	eBNDETECTION_AUTOTUNE_MODE_NONE = 0,
	eBNDETECTION_AUTOTUNE_MODE_MONO = 1,
	eBNDETECTION_AUTOTUNE_MODE_SEMI_NOTE = 2
//	eBNDETECTION_AUTOTUNE_MODE_REF_PITCH = 3,
//	eBNDETECTION_AUTOTUNE_MODE_REF_PITCH_BEAT = 4
} E_BNDETECTION_AUTOTUNE_MODE;


typedef enum
{ 
	eBNDETECTION_BEAT_NONE = 0,
	eBNDETECTION_BEAT_SOUND = 1,
	eBNDETECTION_BEAT_PERCUSSION = 2
} E_BNDETECTION_BEAT_DETECT_MODE;

#define E_BNDETECTION_BEAT_PERCUSSION	eBNDETECTION_BEAT_PERCUSSION
#define E_BNDETECTION_BEAT_SOUND		eBNDETECTION_BEAT_SOUND
#define E_BNDETECTION_BEAT_NONE			eBNDETECTION_BEAT_NONE

typedef enum
{
	eBNDETECTION_ROBOTSOUND_MODE_NONE = 0,
	eBNDETECTION_ROBOTSOUND_MODE_LIGHT_X = 1,
	eBNDETECTION_ROBOTSOUND_MODE_LIGHT = 2,
	eBNDETECTION_ROBOTSOUND_MODE_MIDDLE = 3,
	eBNDETECTION_ROBOTSOUND_MODE_HEAVY = 4,
	eBNDETECTION_ROBOTSOUND_MODE_HEAVY_X = 5
} E_BNDETECTION_ROBOTSOUND_MODE;

typedef enum
{
	eBNDETECTION_ROBOTSOUND_FILTER_NONE = 0,
	eBNDETECTION_ROBOTSOUND_FILTER_LIGHT_X = 1,
	eBNDETECTION_ROBOTSOUND_FILTER_LIGHT = 2,
	eBNDETECTION_ROBOTSOUND_FILTER_MIDDLE = 3,
	eBNDETECTION_ROBOTSOUND_FILTER_HEAVY = 4,
	eBNDETECTION_ROBOTSOUND_FILTER_HEAVY_X = 5,
	eBNDETECTION_ROBOTSOUND_FILTER_HEAVY_XX = 6,
	eBNDETECTION_ROBOTSOUND_FILTER_HEAVY_XXX = 7
} E_BNDETECTION_ROBOTSOUND_FILTER;

typedef enum
{
	eBNDETECTION_SINGALONG_MODE_NONE = 0,
	eBNDETECTION_SINGALONG_MODE_NORMAL = 1,		// use frame energy peak to split beats.
	eBNDETECTION_SINGALONG_MODE_PITCHBEND = 2	// use silence frame to split beats. better to use timbre with loop.
} E_BNDETECTION_SINGALONG_MODE;


typedef enum  {
	ePITCHCHANGE_8K = 8000,
	ePITCHCHANGE_12K = 12000,
	ePITCHCHANGE_15K = 15000,
	ePITCHCHANGE_16K = 16000,
	ePITCHCHANGE_20K = 20000,
	ePITCHCHANGE_24K = 24000,
	ePITCHCHANGE_48K = 48000
} E_PITCHCHANGE_MODE;

typedef enum  {
	eHARMONYSND_8K = 8000,
	eHARMONYSND_12K = 12000,
	eHARMONYSND_15K = 15000,
	eHARMONYSND_16K = 16000,
	eHARMONYSND_20K = 20000,
	eHARMONYSND_24K = 24000,
	eHARMONYSND_48K = 48000
} E_HARMONYSND_MODE;

typedef enum {
	eBNDETECTION_LFO_SINE = 0,
   	eBNDETECTION_LFO_RECTANGLE = 1
} E_BNDETECTION_LFO_TYPE;

// sturcture to store data for call back function to read the recorded PCM samples 
typedef struct tagSpiRecordSnd
{
	UINT32	m_u32RecAddr;	 	// sound start addr
	INT32 	m_i32RecSmplCnt;	// sound total samples count
	INT32 	m_i32RWSmplCnt;		// read samples count
} S_SPI_READ_SND;


typedef UINT32 (*PFN_BNDETECTION_WRITE)(void * pBuf, UINT32 u32ByteNum);
typedef UINT32 (*PFN_BNDETECTION_SEEK)(UINT32 u32Position);
typedef UINT32 (*PFN_BNDETECTION_READ)(void * pBuf, UINT32 u32ByteNum);
typedef UINT32 (*PFN_AUTOTUNE_INPUT_DATA) (void* psAT,UINT32 u32ByteNum);
typedef void (*PFN_PITCHCHANGE_SETDIVIDER)(INT16 i16Timer0Divider, INT16 i16ADCDivider);
typedef void (*PFN_BNDETECTION_START_TIMBRE)(
	UINT8 u8TimbreIdx, 
	UINT8 u8NoteIdx,
//	void** g_ppDscptrAddr,
	UINT32* pu32TimbreFreq,
	INT16* pi16TimbreSampleRate,
	UINT8* pu8VolReleaseStep,
	INT8* pi8TimbrePitchShift,	
	UINT16* pu16TimbreVol);
typedef BOOL (*PFN_MIDISYNC_START_TIMBRE)(
	UINT8 u8Note,
	INT16* pi16NoteLength,			// msec
	INT16 i16DeltaTime,
//	void** g_ppDscptrAddr,
	UINT32* pu32TimbreFreq,			// Q8, reserved for futhure 
	INT16* pi16TimbreSampleRate,	// reserved for futhure 
	UINT8* pu8VolReleaseStep,
	INT8* pi8TimbrePitchShift,	
	UINT16* pu16TimbreVol);			// Q10
typedef BOOL (*PFN_MIDISYNC_START_TIMBRE_EX)(
	UINT8 u8Ch,
	UINT8 u8Note,
	INT16* pi16NoteLength,			// msec
	INT16 i16DeltaTime,
//	void** g_ppDscptrAddr,
	UINT32* pu32TimbreFreq,			// Q8, reserved for futhure 
	INT16* pi16TimbreSampleRate,	// reserved for futhure 
	UINT8* pu8VolReleaseStep,
	INT8* pi8TimbrePitchShift,	
	UINT16* pu16TimbreVol);			// Q10
typedef BOOL (*PFN_MIDISYNC_DECODE_PCM)(
	INT16* pi16PcmBuf,
	INT16 i16PcmNum);
typedef BOOL (*PFN_MIDISYNC_DECODE_PCM_EX)(
	UINT8 u8Ch,
	INT16* pi16PcmBuf,
	INT16 i16PcmNum);
typedef void (*PFN_BNDETECTION_END_TIMBRE)(UINT8 u8TimbreIdx, BOOL bPlayEnd);
typedef BOOL (*PFN_BNDETECTION_GET_EXT_NOTE)(UINT8* pu8Note);
typedef BOOL (*PFN_BNDETECTION_GET_SHIFT_SEMITONE)(INT8* pi8ShiftTone);

typedef void (*PFN_BNDETECTION_START_BEAT)(void);
typedef void (*PFN_BNDETECTION_SOUND_INFO)(UINT16 u16Energy,UINT16 u16Freq);

typedef INT32 (*PFN_PITCHZONE_CAL_DIFF_SUM)(INT16* pi16DataSrc,INT16* pi16DataDest);

typedef void (*PFN_BNDETECTION_GET_DYNAMIC_PITCH)(INT16 i16Freq, UINT8 u8Note, UINT8 u8Cent);
typedef void (*PFN_BNDETECTION_GET_DYNAMIC_NOTE)(INT16 i16Freq, UINT8 u8Note, UINT8 u8Cent);

typedef UINT32 (*PFN_MIDI_PROC_WRITE)(void * pBuf, UINT32 u32ByteNum);
typedef UINT32 (*PFN_MIDI_PROC_READ)(void * pBuf, UINT32 u32Addr, UINT32 u32ByteNum);

typedef BOOL (*PFN_TALKTOSING_GET_EXT_PHRASE)(INT16* pi16NoteNum, INT32* pi32SmplCntPhraseRef);
typedef BOOL (*PFN_TALKTOSING_GET_EXT_NOTE)(INT8* pi8Note, INT32* pi32SmplCntNoteRef);
typedef BOOL (*PFN_PLAY_DRUM)(UINT8 u8Drum);

typedef void (*PFN_GET_STUTTER_PARAMETER)(BOOL* pbFadeIn, UINT16* pu16SylbRatio/*,BOOL* pbFadeOut*/,UINT16* pu16MuteTime);

typedef void (*FPN_GET_PHRASE_NOTE_NUM)(INT8 i8NoteNum);
#define FPN_GET_PHRASE_SYLB_NUM		FPN_GET_PHRASE_NOTE_NUM

typedef void (*PFN_IMITATETALK_INIT_VOICE)(INT32 i32SylbSmplCnt,UINT16 i16Freq, INT32* pi32PcmSmplNum,UINT32* pu32ExtensionRatioMax);

//----------beat note detection--------------------------------------------------------------
UINT32
BNDetection_Config(
	UINT8 u8Support,
	UINT16 u16SampleRate,
	UINT8 u8BeatSmoothNum,				
	UINT8 u8WaveSmoothNum,				
	UINT16* pu16FramePcmCount
);				 

void
BNDetection_InitBuffer(
	void* pBNDBuf
);

void 
BNDetection_RegisterOutputSmfFormat(
	void
);

void 
BNDetection_RegisterOutputSmfFormatEx(
	void* pBNDBuf
);

void 
BNDetection_RegisterOutputImfFormat(	// deprecated
	void
);

void 
BNDetection_RegisterOutputImfFormatEx(
	void* pBNDBuf
);

void
BNDetection_SuppportPitchDetection(		// deprecated
	void
);

void
BNDetection_SuppportPitchDetectionEx(
	void* pBNDBuf
);

void
BNDetection_SuppportDynamicPitchDetection(		// deprecated
	PFN_BNDETECTION_GET_DYNAMIC_PITCH pfnGetDynamicPitch,
	PFN_BNDETECTION_GET_DYNAMIC_NOTE pfnGetDynamicNote
	);

void
BNDetection_SuppportDynamicPitchDetectionEx(
	void* pBNDBuf,
	PFN_BNDETECTION_GET_DYNAMIC_PITCH pfnGetDynamicPitch,
	PFN_BNDETECTION_GET_DYNAMIC_NOTE pfnGetDynamicNote
	);

void
BNDetection_SuppportSingAlongMono(		// deprecated
	E_BNDETECTION_SINGALONG_MODE eSingAlongMode,
	PFN_BNDETECTION_START_TIMBRE pfnStartTimbre,
	PFN_BNDETECTION_END_TIMBRE pfnEndTimbre,	   	// can be NULL
	PFN_MIDISYNC_DECODE_PCM pfnDecodePcm,
	INT16 i16OutputSampleRate,
	UINT8 u8TimbreNum
	);

void
BNDetection_SuppportSingAlongMonoEx(
	void* pBNDBuf,
	E_BNDETECTION_SINGALONG_MODE eSingAlongMode,
	PFN_BNDETECTION_START_TIMBRE pfnStartTimbre,
	PFN_BNDETECTION_END_TIMBRE pfnEndTimbre,	   	// can be NULL
	PFN_MIDISYNC_DECODE_PCM pfnDecodePcm,
	INT16 i16OutputSampleRate,
	UINT8 u8TimbreNum
	);

void
BNDetection_SetSingAlongMode(		// deprecated
	E_BNDETECTION_SINGALONG_MODE eSingAlongMode
	);

void
BNDetection_SetSingAlongModeEx(
	void* pBNDBuf,
	E_BNDETECTION_SINGALONG_MODE eSingAlongMode
	);

void
BNDetection_WriteMidiHeaderSmf(		// deprecated
	UINT8 u8Inst,
	UINT8 u8Vol
);

void
BNDetection_WriteMidiHeaderSmfEx(
	void* pBNDBuf,
	UINT8 u8Inst,
	UINT8 u8Vol
);


void
BNDetection_WriteMidiHeaderImf(		// deprecated
	UINT8 u8Inst,
	UINT8 u8Vol
);

void
BNDetection_WriteMidiHeaderImfEx(
	void* pBNDBuf,
	UINT8 u8Inst,
	UINT8 u8Vol
);

void
BNDetection_InitNoteDetect(			// deprecated
	E_BNDETECTION_MODE eBNDetectionMode,
	E_BNDETECTION_PITCHBEND_MODE eBNDPitchBendMode,
	UINT16 u16FrameEngyThreshold,
	UINT16 u16BeatEngyThreshold,	
	INT8 i8PitchShift,
	BOOL bAlignBeat,
	PFN_BNDETECTION_WRITE g_pfnBNDetection_Write,
	PFN_BNDETECTION_SEEK g_pfnBNDetection_Seek
);

void 
BNDetection_RegsterSinglePitchBend(
	void* pBNDBuf
);

void 
BNDetection_RegsterMultiplePitchBend(
	void* pBNDBuf
);

void
BNDetection_InitNoteDetectEx(
	void* pBNDBuf,
	E_BNDETECTION_MODE eBNDetectionMode,
	E_BNDETECTION_PITCHBEND_MODE eBNDPitchBendMode,
	UINT16 u16FrameEngyThreshold,
	UINT16 u16BeatEngyThreshold,	
	INT8 i8PitchShift,
	BOOL bAlignBeat,
	PFN_BNDETECTION_WRITE g_pfnBNDetection_Write,
	PFN_BNDETECTION_SEEK g_pfnBNDetection_Seek
);


BOOL
BNDetection_BeatNoteDetect(		// deprecated
	E_BNDETECTION_RESULT *peResult
);

BOOL
BNDetection_BeatNoteDetectEx(
	void* pBNDBuf,
	E_BNDETECTION_RESULT *peResult
);

BOOL
BNDetection_BeatNoteDetectExx(
	void* pBNDBuf,
	E_BNDETECTION_RESULT *peResult,
	INT16* pi16FrameEngy
);

E_BNDETECTION_RESULT
BNDetection_EnergyDetect(		// deprecated
	INT16* pi16FrameEngy
);

E_BNDETECTION_RESULT
BNDetection_EnergyDetectEx(
	void* pBNDBuf,
	INT16* pi16FrameEngy
);

BOOL
BNDetection_StopDetect(			// deprecated
	void
);

BOOL
BNDetection_StopDetectEx(
	void* pBNDBuf
);

BOOL
BNDetection_NeedDetect(			// deprecated
	void
);

BOOL
BNDetection_NeedDetectEx(
	void* pBNDBuf
);

UINT8 
BNDetection_SetInputData_2(		// deprecated
	INT16 ai16InputBuf[]);


UINT8 
BNDetection_SetInputDataEx_2(
	void* pBNDBuf,
	INT16 ai16InputBuf[]);

UINT8 
BNDetection_SetInputData(		// deprecated
	INT16 i16InputBufCount,		// i16ADBufCount must be 1/2/4/8
	INT16 ai16InputBuf[]);

UINT8 
BNDetection_SetInputDataEx(
	void* pBNDBuf,
	INT16 i16InputBufCount,		// i16ADBufCount must be 1/2/4/8
	INT16 ai16InputBuf[]);

BOOL
BNDetection_BeatPitchDetect(	// deprecated
	E_BNDETECTION_RESULT *peResult
);

BOOL
BNDetection_BeatPitchDetectEx(
	void* pBNDBuf,
	E_BNDETECTION_RESULT *peResult
);


INT16 
BNDetection_FillPcmBuf(			// deprecated
	INT16 i16PcmNum,
	INT16* pi16PcmBuf);

INT16 
BNDetection_FillPcmBufEx(
	void* pBNDBuf,
	INT16 i16PcmNum,
	INT16* pi16PcmBuf);

void
BNDetection_SetPitchShift(		// deprecated
	INT8 i8PitchShift
	);

void
BNDetection_SetPitchShiftEx(
	void* pBNDBuf,
	INT8 i8PitchShift
	);

INT8 
BNDetction_MatchCommand3Sylb(
	void* pBNDBuf,
	UINT8* pu8MidiBuf,
	INT8 i8DiffThd,
	INT8 i8LenThd		// 100 ms unit
	);

UINT16
BNDetection_StartPlay(
	void* pBND,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	INT16 i16TimbreID,
	PFN_BNDETECTION_WRITE App_Write_Callback,
	PFN_BNDETECTION_SEEK App_Seek_Callback);

//----------beat detection--------------------------------------------------------------

void
BNDetection_InitBeatDetect(
	E_BNDETECTION_BEAT_DETECT_MODE eDetectMode,
	UINT16 u16FrameEngyThreshold,	
	INT16 i16HiEngyThd,
	INT16 i16LoEngyThd,
	UINT8 u8HiFraction,
	UINT8 u8LoFraction
);

void
BNDetection_InitBeatDetectEx(
	void* pBNDBuf,
	E_BNDETECTION_BEAT_DETECT_MODE eDetectMode,
	UINT16 u16FrameEngyThreshold,	
	INT16 i16HiEngyThd,
	INT16 i16LoEngyThd,
	UINT8 u8HiFraction,
	UINT8 u8LoFraction
);

E_BNDETECTION_RESULT
BNDetection_BeatDetect(
	INT16* pi16Engy
);


E_BNDETECTION_RESULT
BNDetection_BeatDetectEx(
	void* pBNDBuf,
	INT16* pi16Engy
);

UINT16
BNDetection_BeatDetect_StartRec(
	void* pBD,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	E_BNDETECTION_BEAT_DETECT_MODE 	eDetectMode);

//-------------- midi proc-------------------------------------------------------------
INT16
MidiProc_GetBufSize(
	INT8 i8ChnlNum
);

void 
MidiProc_Init(
	void* pBuf,
	INT8 i8ChnlNum,
	PFN_MIDI_PROC_READ	pfnRead,
	PFN_MIDI_PROC_WRITE pfnWrite
);

BOOL
MidiProc_InitMidiChnl(
	void* pBuf,
	INT8 i8ChnlIdx,
	INT32 i32MidiAddr
);

BOOL
MidiProc_Merge(
	void* pBuf
);

BOOL
MidiProc_Duplicate(
	void* pMidiProcBuf
);

void
MidiProc_RemoveHeadSil(
	UINT8* pu8MidiData);

void 
MidiProc_AlignTempo(
	UINT8* pu8MidiData,
	INT16 i16Tempo);

INT16
MidiProc_GetBufSize_MidiSync(
	void);

BOOL
MidiProc_InitMidiSync(
	void* pMidiSyncBuf,
	INT16 i16OutputSampleRate,
	UINT8* pu8MidiData,
	PFN_MIDISYNC_START_TIMBRE pfnStartTimbre,
	PFN_MIDISYNC_DECODE_PCM pfnDecodePcm
	);

BOOL
MidiProc_InitMidiSyncEx(
	void* pMidiSyncBuf,
	INT16 i16OutputSampleRate,
	UINT8* pu8MidiData,
	PFN_MIDISYNC_START_TIMBRE_EX pfnStartTimbreEx,
	PFN_MIDISYNC_DECODE_PCM_EX pfnDecodePcmEx,
	UINT8 u8Ch
	);

BOOL
MidiProc_SynthesizeData(
	void* pMidiSyncBuf,
	INT16* ai16PcmBuf,
	INT16 i16PcmNum);

INT16
MidiProc_ReverseMidi(
	UINT8* pu8MidiBuf,
	UINT8* pu8DataBuf,
	INT16 i16TailSil);

//-------------- midi match-------------------------------------------------------------
INT16
MidiMatch_GetBufSize(
	INT8 i8ChnlNum
);

BOOL
MidiMatch_Compare(
	void* pBuf,
	UINT8* pu8MidiData,
	UINT16 u16MidiId,
	UINT8 u8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	INT16* pi16Diff,
	UINT8 u8Threshold
);

// ---------detect 8KHz-----------------------------------------------------------------

UINT32
Detect8K_Config(
	UINT8 u8DetectFrameCnt
);				 

void
Detect8K_Init(
	void* pBNDBuf,
	UINT16 u16FrameEngyThreshold,
	UINT8 u8DetectFrameCnt
);

UINT8 
Detect8K_SetInputData(
	void* pD8KBuf,
	INT16 i16InputBufCount,		// i16ADBufCount must be 1/2/4/8
	INT16 ai16InputBuf[]
);

BOOL
Detect8K_NeedDetect(
	void* pD8KBuf
);

INT8
Detect8K_Detect(
	void* pBNDBuf
);

//----------Pitch Zone--------------------------------------------------------------

#define PITCHZONE_CORR_WIND_SIZE_XXXX1			72	// 72 / 1 ==> 72
#define PITCHZONE_CORR_WIND_SIZE_XXXX			144	// 144 / 2 ==> 72
#define PITCHZONE_CORR_WIND_SIZE_XXXX4			288	// 144 / 4 ==> 72

#define PITCHZONE_CORR_WIND_SIZE_XXX1			64	// 64 / 1 ==> 64
#define PITCHZONE_CORR_WIND_SIZE_XXX			128	// 128 / 2 ==> 64
#define PITCHZONE_CORR_WIND_SIZE_XXX3			192	// 192 / 3 ==> 64

#define PITCHZONE_CORR_WIND_SIZE_XX1			56	// 56 / 1 ==> 56
#define PITCHZONE_CORR_WIND_SIZE_XX 			112	// 112 / 2 ==> 56
#define PITCHZONE_CORR_WIND_SIZE_XX3			168	// 168 / 3 ==> 56

#define PITCHZONE_CORR_WIND_SIZE_EXTRA1			48	// 48 / 1 ==> 48
#define PITCHZONE_CORR_WIND_SIZE_EXTRA			96	// 96 / 2 ==> 48
#define PITCHZONE_CORR_WIND_SIZE_EXTRA3			144	// 144 / 3 ==> 48
#define PITCHZONE_CORR_WIND_SIZE_EXTRA4			192	// 192 / 4 ==> 48

#define PITCHZONE_CORR_WIND_SIZE_ULTRA5			200	// 200 / 5 ==> 40

#define PITCHZONE_CORR_WIND_SIZE_HUGE1			32	// 32 / 1 ==> 32
#define PITCHZONE_CORR_WIND_SIZE_HUGE			64	// 64 / 2 ==> 32
#define PITCHZONE_CORR_WIND_SIZE_HUGE3			96	// 96 / 3 ==> 32
#define PITCHZONE_CORR_WIND_SIZE_HUGE4			128	// 128 / 4 ==> 32

#define PITCHZONE_CORR_WIND_SIZE_HEAVY1			24	// 24 / 1 ==> 24
#define PITCHZONE_CORR_WIND_SIZE_HEAVY			48	// 48 / 2 ==> 24
#define PITCHZONE_CORR_WIND_SIZE_HEAVY3			72	// 72 / 3 ==> 24
#define PITCHZONE_CORR_WIND_SIZE_HEAVY4			96	// 96 / 4 ==> 24
#define PITCHZONE_CORR_WIND_SIZE_HEAVY6			144	// 144 / 6 ==> 24

#define PITCHZONE_CORR_WIND_SIZE_LARGE1			21	// 21 / 1 ==> 21
#define PITCHZONE_CORR_WIND_SIZE_LARGE			42	// 42 / 2 ==> 21
#define PITCHZONE_CORR_WIND_SIZE_LARGE3			63	// 63 / 3 ==> 21
#define PITCHZONE_CORR_WIND_SIZE_LARGE4			84	// 84 / 4 ==> 21
#define PITCHZONE_CORR_WIND_SIZE_LARGE6			126	// 126 / 6 ==> 21

#define PITCHZONE_CORR_WIND_SIZE_MIDDLE1		18	// 18 / 1 ==> 18
#define PITCHZONE_CORR_WIND_SIZE_MIDDLE			36	// 36 / 2 ==> 18
#define PITCHZONE_CORR_WIND_SIZE_MIDDLE3		54	// 54 / 3 ==> 18
#define PITCHZONE_CORR_WIND_SIZE_MIDDLE4		72	// 72 / 4 ==> 18
#define PITCHZONE_CORR_WIND_SIZE_MIDDLE6		108	// 108 / 6 ==> 18

#define PITCHZONE_CORR_WIND_SIZE_SMALL1			15	// 15 / 1 ==> 15
#define PITCHZONE_CORR_WIND_SIZE_SMALL			30	// 30 / 2 ==> 15
#define PITCHZONE_CORR_WIND_SIZE_SMALL3			45	// 45 / 3 ==> 15
#define PITCHZONE_CORR_WIND_SIZE_SMALL4			60	// 60 / 4 ==> 15

#define PITCHZONE_CORR_WIND_SIZE_LIGHT1			12	// 12 / 1 ==> 12
#define PITCHZONE_CORR_WIND_SIZE_LIGHT			24	// 24 / 2 ==> 12
#define PITCHZONE_CORR_WIND_SIZE_LIGHT3			36	// 36 / 3 ==> 12

#define PITCHZONE_COOR_WIND_SIZE_EXTRA1 		PITCHZONE_CORR_WIND_SIZE_EXTRA1	// for backward compatible

#if (defined(__N571_MAC__))
#define PitchZone_CalDiffSumXXXX4	NULL
#define PitchZone_CalDiffSumXXXX	NULL
#define PitchZone_CalDiffSumXXXX1	NULL
#define PitchZone_CalDiffSumXXX3	NULL
#define PitchZone_CalDiffSumXXX		NULL
#define PitchZone_CalDiffSumXXX1	NULL
#define PitchZone_CalDiffSumXX3		NULL
#define PitchZone_CalDiffSumXX		NULL
#define PitchZone_CalDiffSumExtra4	NULL
#define PitchZone_CalDiffSumExtra3	NULL
#define PitchZone_CalDiffSumExtra	NULL
#define PitchZone_CalDiffSumExtra1	NULL
#define PitchZone_CalDiffSumUltra5	NULL
#define PitchZone_CalDiffSumHuge4	NULL
#define PitchZone_CalDiffSumHuge3	NULL
#define PitchZone_CalDiffSumHuge	NULL
#define PitchZone_CalDiffSumHuge1	NULL
#define PitchZone_CalDiffSumHeavy6	NULL
#define PitchZone_CalDiffSumHeavy4	NULL
#define PitchZone_CalDiffSumHeavy3	NULL
#define PitchZone_CalDiffSumHeavy	NULL
#define PitchZone_CalDiffSumHeavy1	NULL
#define PitchZone_CalDiffSumLarge6	NULL
#define PitchZone_CalDiffSumLarge4	NULL
#define PitchZone_CalDiffSumLarge3	NULL
#define PitchZone_CalDiffSumLarge	NULL
#define PitchZone_CalDiffSumLarge1	NULL
#define PitchZone_CalDiffSumMiddle6	NULL
#define PitchZone_CalDiffSumMiddle5	NULL
#define PitchZone_CalDiffSumMiddle4	NULL
#define PitchZone_CalDiffSumMiddle3	NULL
#define PitchZone_CalDiffSumMiddle	NULL
#define PitchZone_CalDiffSumMiddle1	NULL
#define PitchZone_CalDiffSumSmall4	NULL
#define PitchZone_CalDiffSumSmall3	NULL
#define PitchZone_CalDiffSumSmall	NULL
#define PitchZone_CalDiffSumSmall1	NULL
#define PitchZone_CalDiffSumLight3	NULL
#define PitchZone_CalDiffSumLight	NULL
#define PitchZone_CalDiffSumLight1	NULL
#else
INT32
PitchZone_CalDiffSumLight1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLight(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLight3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumSmall1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumSmall(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumSmall3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumSmall4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle5(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumMiddle6(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge5(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumLarge6(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy5(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHeavy6(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHuge1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHuge(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHuge3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumHuge4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumUltra5(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumExtra1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumExtra(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumExtra3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumExtra4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXX(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXX3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXX1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXX(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXX3(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXXX1(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXXX(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);

INT32
PitchZone_CalDiffSumXXXX4(
	INT16* pi16DataSrc,
	INT16* pi16DataDest
	);
#endif

void 
PitchZone_SetCalDiffInfo_24K_Extra(void);

void 
PitchZone_SetCalDiffInfo_24K_Huge(void);

void 
PitchZone_SetCalDiffInfo_24K_Heavy(void);

void 
PitchZone_SetCalDiffInfo_16K_Extra(void);

void 
PitchZone_SetCalDiffInfo_16K_Huge(void);

void 
PitchZone_SetCalDiffInfo_16K_Heavy(void);

void 
PitchZone_SetCalDiffInfo_12K_Extra(void);

void 
PitchZone_SetCalDiffInfo_12K_Huge(void);

void 
PitchZone_SetCalDiffInfo_12K_Heavy(void);

void 
PitchZone_SetCalDiffInfo_12K_Large(void);

void 
PitchZone_SetCalDiffInfo_12K_Middle(void);

void
PitchZone_SetPitchDetectFunc(
	void* pPZ,
	PFN_PITCHZONE_CAL_DIFF_SUM	pfnCalDiffSum,
	INT16 i16CorrWinSize
	);

void
PitchZone_SetPitchDetectFuncEx(
	PFN_PITCHZONE_CAL_DIFF_SUM	pfnCalDiffSum,
	INT16 i16CorrWinSize
	);

void 
PitchZone_SetPitchRange12K(
	INT16 i16MinPitch,
	INT16 i16MaxPitch
	);

void
PitchZone_SetLowEngyThd(
	INT16 i16LowEngyThd
	);

void
PitchZone_SM0SP0(
	void* pPZ
	);

void
PitchZone_SM0SP1(
	void* pPZ
	);

void
PitchZone_SM1SP0(
	void* pPZ
	);

void
PitchZone_SM1SP1(
	void* pPZ
	);

INT16
PitchZone_GetInitDataNum(
	void* pPZ
	);

BOOL
PitchZone_InitInputData(
	void* pPZ,
	INT16 i16InputBufCount,		// i16ADBufCount must be 1/2/4/8
	INT16 ai16InputBuf[],
	INT16 i16StartIdx
	);

INT16
PitchZone_GetRestDataNum(
	void* pPZ
	);

void
PitchZone_SetInterpoFactor(
	void* pPZ,
	UINT32 u32InterpoFactor
);

INT16 
PitchZone_GetLowEngyWidth(
	void* pPZ);

void
PitchZone_SetOutputSampleRate(
	void *pPZ,
	UINT16 u16OutputSampleRate);

void
PitchZone_SetPitchShift(
	void *pPZ,
	INT8 i8PitchShift);

UINT8 
PitchZone_SetInputData(
	void* pPZ,
	INT16 i16InputBufCount,		// i16ADBufCount must be 1/2/4/8
	INT16 ai16InputBuf[]
);

BOOL 
PitchZone_FillPcmBuf(
	void* pStruct,
	PINT16 pi16PCMBuf
);

BOOL
PitchZone_RemoveHeadTailNoise(
	void* pPZ,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm,
	INT32 i32TotalSmplCnt,
	UINT16 u16HeadThdRatio,		// Q8, > 0x100
	UINT16 u16TailThdRatio,		// Q8, > 0x100
	UINT16 u16RewindTime,		// Q8, second
	INT32 *pi32HeadNoiseSmplCnt,
	INT32 *pi32TailNoiseCmplCnt);

void
PitchZone_SetTuneVolThd(
	void *pPZ,
	INT16 i16TuneVolThd
	);

void
PitchZone_TuneVolume(
	void* pPZ);

UINT16 
PitchZone_TuneVolumeEx(
	void* pPZ);

void*
PitchZone_GetPitchZone(
	void* pPZ);

// -----AutoTune--------------------------------------------------------------------------

#define AUTOTUNE_ON							0x01
#define OVERTUNE_ON							0x02
#define OVERTUNE_PITCH_TURN_UP				0x04
#define OVERTUNE_PITCH_TURN_DOWN			0x08
#define OVERTUNE_PITCH_TURN_DOWN_UP			0x10
#define OVERTUNE_ON_STARTING				0x20
#define OVERTUNE_FIX_LENGTH					0x40
//#define AUTOTUNE_DOUBLE_OUTPUT_SAMPLE_RATE 	0x80

typedef enum
{
	eMAJOR_SCALE_NA = -1,
	eMAJOR_SCALE_C = 0,
	eMAJOR_SCALE_Db = 1,
	eMAJOR_SCALE_D = 2,
	eMAJOR_SCALE_Eb = 3,
	eMAJOR_SCALE_E = 4,
	eMAJOR_SCALE_F = 5,
	eMAJOR_SCALE_Gb = 6,
	eMAJOR_SCALE_G = 7,
	eMAJOR_SCALE_Ab = 8,
	eMAJOR_SCALE_A = 9,
	eMAJOR_SCALE_Bb = 10,
	eMAJOR_SCALE_B = 11,
} E_AUTOTUNE_MAJOR_SCALE;

#define E_MAJOR_SCALE	E_AUTOTUNE_MAJOR_SCALE


UINT16								// return buffer size
AutoTune_Config(
	E_BNDETECTION_FILTER_MODE eFilterMode,
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
AutoTune_Init(
	E_BNDETECTION_AUTOTUNE_MODE eAutoTuneMode,
	void *pAutoTuneBuf,
	void *pRefAutoTuneBuf,
	PFN_AUTOTUNE_INPUT_DATA pfnInputData,
	INT16 i16MonoPitch
);

void
AutoTune_SetMonoPitch(
	void *pAutoTuneBuf,
	INT16 i16MonoPitch
	);

void
AutoTune_SetAutoTuneParam(
	void *pAutoTuneBuf,
	UINT8 u8AutoTuneOtion,
	INT8 i8OverTuneDepth,
	INT8 i8OverTuneWidth,
	INT8 i8OverTuneContiCountThd
	);

void
AutoTune_SetOverTuneSteadyParam(
	void *pAutoTuneBuf,
	INT8 i8OverTuneSteadyCountThd
	);

void
AutoTune_SetPitchTable(
	void *pAutoTuneBuf,
	UINT16* pu16PitchTable,
	INT8 i8Scale	// 1~8-- 1/8, 2/8, 3/8, 4/8, 5/8, 6/8, 7/8, 8/8 full-tone
   );

void
AutoTune_SetPitchShift(
	void *pAutoTuneBuf,
	INT8 i8PitchShift
	);

void
AutoTune_EnableExtRefPitch(
	void* pAutoTuneBuf,
	BOOL bEnable);

void
AutoTune_EnableExtRefPitchBeat(
	void* pAutoTuneBuf,
	PFN_BNDETECTION_GET_EXT_NOTE pfnGetExtNote,
	UINT16 u16BeatEngyThreshold,
	UINT8 u8BeatDetectBufNum,
	void* pBeatDetectBuf);

void
AutoTune_EnableSoundInfo(
	void* pBuf,
	PFN_BNDETECTION_START_BEAT pfnStartBeat,
	PFN_BNDETECTION_SOUND_INFO pfnSoundInfo,
	UINT16 u16BeatEngyThreshold,
	UINT8 u8BeatDetectBufNum,
	void* pBeatDetectBuf);

void
AutoTune_DisableGetExtNote(
	void* pAutoTuneBuf);

void
AutoTune_SetExtNoteOn(
	void* pAutoTuneBuf,
	UINT8 u8Note);

void
AutoTune_SetExtNoteOff(
	void* pAutoTuneBuf,
	UINT8 u8Note);

void
AutoTune_SetExtPitchbend(
	void* pAutoTuneBuf,
	UINT16 u16Pitchbend);

void
AutoTune_SetExtPitchbendSensitivity(
	void* pAutoTuneBuf,
	UINT16 u16PitchbendSensitivity);

BOOL	
AutoTune_MidiGetNextNote(
	UINT8* pu8Note);

void
AutoTune_SetChromaticScale(
	void* pAutoTuneBuf);

void
AutoTune_SetMajorScale(
	void* pAutoTuneBuf,
	E_AUTOTUNE_MAJOR_SCALE eMajorScale
	);

void
AutoTune_SetExtRefPitch(
	void* pBuf,
	INT16 i16ExtRefPitch);

void
AutoTune_EnableAutoTune(
	void* pAT,
	BOOL bEnable);

UINT16
AutoTune_StartPlay(
	void* pAT,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	UINT16 u16OutSampleRate);

#define AutoTune_FillPcmBuf				PitchZone_FillPcmBuf	
#define AutoTune_SetInputData			PitchZone_SetInputData
#define AutoTune_GetPitchZone			PitchZone_GetPitchZone
#define AutoTune_SetOutputSampleRate	PitchZone_SetOutputSampleRate


// -----AutoTune LFO--------------------------------------------------------------------------

UINT16								// return buffer size
AutoTuneLfo_Config(
	E_BNDETECTION_FILTER_MODE eFilterMode,
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
AutoTuneLfo_Init(
	E_BNDETECTION_AUTOTUNE_MODE eAutoTuneMode,
	void *pAutoTuneLfoBuf
);

void
AutoTuneLfo_SetPitchTableEx(
	void *pAutoTuneBuf,
	const UINT16* pu16PitchTable,
	UINT8 u8TableEntries
);

void
AutoTuneLfo_SetIntensity(
	void *pAutoTuneLfoBuf,
	UINT16 u16Intensity	
	);

void
AutoTuneLfo_SetLfoType(
	void *pAutoTuneLfoBuf,
	E_BNDETECTION_LFO_TYPE eLfoType
);

void
AutoTuneLfo_SetVolLfo(
	void *pAutoTuneLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
);

void
AutoTuneLfo_SetFreqLfo(
	void *pAutoTuneLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
);

void
AutoTuneLfo_SetIntensityLfo(
	void *pAutoTuneLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
);

#define AutoTuneLfo_GetAutoTuneBuf	PitchZone_GetPitchZone
#define AutoTuneLfo_FillPcmBuf		PitchZone_FillPcmBuf

//------------------------------------------------------------------------------------------
UINT16		
TalkToSing_Config(
	UINT16 u16SampleRate,		
	UINT8 u8BeatDetectBufNum,
	INT16 i16ExtraInBufSize			
);

BOOL
TalkToSing_Init(			// deprecated
	void *pTalkToSingBuf,
	UINT16 u16BeatEngyThreshold,
	UINT8 u8BeatDetectBufNum,
	PFN_BNDETECTION_WRITE pfnWrite,
	PFN_BNDETECTION_SEEK pfnSeek,
	PFN_BNDETECTION_READ pfnRead,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm
);

BOOL
TalkToSing_InitEx(
	void *pTalkToSingBuf,
	UINT16 u16BeatEngyThreshold,
	UINT8 u8BeatDetectBufNum,
	UINT8* pu8SylbPitchInfoBuf,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm);

void 
TalkToSing_SetFillPcmNum(
	void *pTalkToSingBuf,
	INT8 i8FillPcmNum);

void 
TalkToSing_SetSilResvCnt(
	void *pTalkToSingBuf,
	INT8 i8SilResvCnt);

INT16
TalkToSing_Analysis(
	void* pTalkToSingBuf,
	UINT32* pu32HeadSilSmplCnt,
	UINT32* pu32EffectiveSmplCnt
);

INT16
TalkToSing_AnalysisEx(
	void* pTalkToSingBuf,
	UINT32* pu32HeadSilSmplCnt,
	UINT32* pu32EffectiveSmplCnt,
	UINT16* pu16UsedSylbPitchInfoBufSize
	);

void
TalkToSing_SetNormalizedBuf(
	void *pTalkToSingBuf,
	UINT16* pu16Buf);

BOOL
TalkToSing_Normalize(
	void *pTalkToSingBuf,
	INT8 i8BufNum,
	UINT16* pu16Buf);

void
TalkToSing_InitPlayback(		// deprecated
	void *pTalkToSingBuf,
	UINT16 u16Option,
	UINT8 u8AutotuneIntensity,
	INT32 i32EndSilSmplNum
	);

void
TalkToSing_InitPlaybackEx(
	void *pTalkToSingBuf,
	UINT16 u16Option,
	UINT8 u8AutotuneIntensity
	);

void
TalkToSing_SetSyllableTuneRatio(
	void *pTalkToSingBuf,
	UINT16 u16Ratio);	// Q12

void
TalkToSing_EnableMidiPhrase(
	void *pTalkToSingBuf,
	BOOL bMidiPhrase);

UINT16 
TalkToSing_GetRepeatInPhraseBufSize(void);

void
TalkToSing_EnableRepeatInPhrase(
	void *pTalkToSingBuf,
	void *pTTSRIPBuf,
	BOOL bWithEcho);

void 
TalkToSing_SetSyncModeBestMatch(
	void *pTalkToSingBuf
	);

void 
TalkToSing_SetSyncModeOneToOne(
	void *pTalkToSingBuf
	);

void 
TalkToSing_SetSyncModeSpeech(
	void *pTalkToSingBuf
	);

void 
TalkToSing_InitExtNote(
	void *pTalkToSingBuf,
	PFN_TALKTOSING_GET_EXT_PHRASE pfnGetExtPhrase,
	PFN_TALKTOSING_GET_EXT_NOTE pfnGetExtNote);


void
TalkToSing_SetExtNote(
	void* pTTS,
	INT8 i8Note,
	INT32 i32SmplCntNoteRef
);

void
TalkToSing_SetRefPitch(			// deprcated
	void *pTalkToSingBuf,
	INT16 i16PitchSylbRef,
	INT32 i32SmplCntSylbRef
	);

void
TalkToSing_SetRefFreq(			// deprecated
	void *pTalkToSingBuf,
	UINT32 u32FreqSylbRef,			// 8-bit fix point
	UINT32 u32TimeLenSylbRef		// msec
	);

BOOL
TalkToSing_InitRefMidiId(
	void *pTalkToSingBuf,
	INT8 i8MidiID,
	INT8 i8MidiChn,
	INT8 i8NoteShift,
	UINT32 u32HeadSilSmplCnt,
	PFN_MIDISYN_GETDATA pfnGetDataCallback);

BOOL
TalkToSing_ChangeRefMidiId(
	void *pTalkToSingBuf,
	INT8 i8MidiID,
	INT8 i8MidiChn);

BOOL
TalkToSing_InitRefMidiAddr(
	void *pTalkToSingBuf,
	UINT32 u32MidiBuf,
	INT8 i8MidiChn,
	INT8 i8NoteShift,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
	);

BOOL 
TalkToSing_FillPcmBuf(
	void *pTalkToSingBuf,
	PINT16 pi16PCMBuf
);

BOOL 
TalkToSing_FillPcmBufEx(
	void *pTalkToSingBuf,
	PINT16 pi16PCMBuf
);

void*
TalkToSing_GetFreeBufAsPlaying(		// deprecated
	void *pTalkToSingBuf,
	UINT16* pu16BufSize
);

void
TalkToSing_SetPitchShift(
	void *pTalkToSingBuf,
	INT8 i8PitchShift,	  	// semitone
	INT8 i8SemiToneScale
	);

void
TalkToSing_SetPitchShiftByRefPitchTable(		// deprecated
	void *pTalkToSingBuf,
	UINT16 u16ItemNum,
	const INT16* pi16PitchRef
	);

INT8
TalkToSing_GetPitchShiftByRefMidiId_Average(
	void *pTalkToSingBuf,
	UINT8 u8MidiID,
	UINT8 u8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
);

INT8
TalkToSing_GetPitchShiftByRefMidiId_High(
	void *pTalkToSingBuf,
	UINT8 u8MidiID,
	UINT8 u8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
);

INT16
TalkToSing_CalAvgSylbPitchEx(
	void *pTalkToSingBuf
);

void
TalkToSing_SetPitchLevel(
	void *pTalkToSingBuf,
	INT8 i8PitchLevel,	  	// semitone, ex: 60, 72...
	INT8 i8SemoToneScale
);

BOOL
TalkToSing_ChangeSylbNum(		// deprecated
	void *pTalkToSingBuf,
	INT16 i16SylbNum
	);

BOOL
TalkToSing_ChangeSylbNumEx(
	void *pTalkToSingBuf,
	INT16 i16SylbNum
	);

BOOL
TalkToSing_IsSndEnd(
	void *pTalkToSingBuf
	);

BOOL
TalkToSing_ChangeSnd(
	void *pTalkToSingBuf
	);

UINT16
TalkToSing_GetSylbInfoSize(
	void *pTalkToSingBuf
	);

void
TalkToSing_SetSylbPitchInfoBuf(
	void *pTalkToSingBuf,
	UINT8* pu8SylbPitchInfoBuf);

void*
TalkToSing_GetPitchZone(		// deprecated
	void *pTalkToSingBuf
	);

/*
void
TalkToSing_SyncBeat(
	void* pTalkToSingBuf,
	INT16 i16i16Bpm);

UINT16 
TalkToSing_GetGenerativeModeBufSize(void);

void TalkToSing_SetSyncModeGenerative(
	void *pTalkToSingBuf,
	void *pTTSGBuf,
	UINT8 u8RandomSeed);

void
TalkToSing_SetMajorScale(
	void *pTalkToSingBuf,
	E_AUTOTUNE_MAJOR_SCALE eMajorScale,
	BOOL bPentatonic);

INT8
TalkToSing_GetPitchShiftByRefMidiIdArray_High(
	void *pTalkToSingBuf,
	INT8 i8Num,
	const INT8* pi8MidiID,
	UINT8 u8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
);

INT8
TalkToSing_GetPitchShiftByRefMidiIdArray_Average(
	void *pTalkToSingBuf,				
	INT8 i8Num,
	const INT8* pi8MidiID,
	UINT8 u8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
);

void
TalkToSing_SpecialEffect(
	void *pTalkToSingBuf,
	UINT8 u8SylbStutter,
	UINT8 u8SylbRepeat,
	BOOL bEcho);

void
TalkToSing_SetFreqLfo(
	void *pTalkToSingBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
);
*/

UINT16 
TalkToSing_GetStutterEndModeBufSize(void);

void 
TalkToSing_SetSyncModeStutterEnd(
	void *pTalkToSingBuf,
	void *pTTSSBuf,
	INT8 i8StutterMode,
	PFN_GET_STUTTER_PARAMETER pfnGetStutterParameter
	);

void
TalkToSing_SetLengthRatio(	// only workable for pitch change appliation, not for talk to sing
	void *pTalkToSingBuf,
	UINT16 u16Ratio			// 0x1000 original length
);

void
TalkToSing_SetPitchShiftRange(
	void *pTalkToSingBuf,
	INT16 i16PitchRange
);

void
TalkToSing_SetPitchShiftRangeEx(
	void *pTalkToSingBuf,
	INT8 i8PitchRangeUp,
	INT8 i8PitchRangeDn,
	INT8 i8PitchHighest,
	INT8 i8PitchLowest
); 

UINT16 
TalkToSing_GetEchoModeBufSize(void);

void 
TalkToSing_SetSyncModeEcho(
	void *pTalkToSingBuf,
	void *pTTSEBuf,
	BOOL bRepeatSnd);

void
TalkToSing_SetMaxLoopNum(
	void *pTalkToSingBuf,
	INT8 i8UnvoiceMaxLoopNum,
	INT8 i8LowVolMaxLoopNum,
	INT8 i8VoiceMaxLoopNum);

void 
TalkToSing_SetOutputSampleRate(
	void *pTalkToSingBuf,
	UINT16 u16OutputSampleRate
	);

UINT16
TalkToSing_GetOutputSampleRate(
	void *pTalkToSingBuf
	);

INT16
TalkToSing_GetMaxPitchVariance(
	void *pTalkToSingBuf
	);

void
TalkToSing_SetPhraseCallbackFunc(
	void *pTalkToSingBuf,
	FPN_GET_PHRASE_SYLB_NUM pfnGetPhraseSylbNum
	);

BOOL
TalkToSing_RemoveHeadTailNoise(
	void *pTalkToSingBuf,
	INT32 i32TotalSmplCnt,
	UINT16 u16HeadThdRatio,		// Q8, > 0x100
	UINT16 u16TailThdRatio,		// Q8, > 0x100
	UINT16 u16RewindTime,		// Q8, second
	INT32 *pi32HeadNoiseSmplCnt,
	INT32 *pi32TailNoiseCmplCnt);

BOOL
TalkToSing_ReCalculateSylbStartPitch(
	void *pTalkToSingBuf
	);

BOOL
TalkToSing_CalVolume(
	void *pTalkToSingBuf,
	INT16* pi16MaxVol,
	INT16* pi16AvgVol);

void
TalkToSing_GetMelodyNoteInfo(
	void *pTTS,
	INT8 i8MidiID,
	INT8 i8MidiChn,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	INT8* pi8LowestNote,
	INT8* pi8AvgNote,
	INT8* pi8HighestNote
);

BOOL
TalkToSing_GetSylbNoteInfo(
	void *pTTS,
	INT8* pi8LowestNote,
	INT8* pi8AvgNote,
	INT8* pi8HighestNote
);

/*
UINT16
TalkToSing_GetShiftPitchExBufSize(
	void *pTalkToSingBuf,
	BOOL bTwoOctaves
	);

void
TalkToSing_InitShiftPitchEx(
	void *pTalkToSingBuf,
	void *pShiftPitchExBuf,
	BOOL bTwoOctaves,
	INT16 i16ShiftPitch
	);

BOOL 
TalkToSing_FillPcmBufExx(
	void *pTalkToSingBuf,
	PINT16 pi16PCMBuf
);

BOOL 
TalkToSing_FillPcmBufSP(
	void *pTalkToSingBuf,
	PINT16 pi16PCMBuf
);
*/

UINT16
TalkToSing_StartPlay(
	void* pTTS,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	UINT16 u16OutSampleRate,
	UINT8* au8SylbInfoBuf,
	UINT16* au16NormalizeBuf,
	UINT8* au8RepeatInPhraseBuf,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	UINT8 u8TalkToSingOption,			// TalkToSing option
	BOOL bTalkToSingEcho,				// enable echo effect or not?
	INT16 i16PlayMidiID,				// The MIDI file played as backgound music
	INT8 i8MidiChRefByTalkToSing);		// MIDI channel referenced By TalkToSing as singing melody

UINT16
TalkToSing_StartAnalysis(
	void* pTTS,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
//	UINT16 u16OutSampleRate,
	UINT8* au8SylbInfoBuf,
	UINT16* pu16UsedSylbPitchInfoBufSize,
	UINT16* au16NormalizeBuf,
	UINT16 u16NormalizeBufSize,
//	UINT8* au8RepeatInPhraseBuf,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm,
//	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	S_SPI_READ_SND* psSpiRecSnd,
	UINT32* pu32SkipHeadSmplCnt,
//	UINT32* pu32EffectiveSmplCnt,
	INT16 *pi16SylbNum,
	BOOL b8BitFormat);

UINT16
TalkToSing_GetRobotEffectBufSize(
	void
	);
	
void
TalkToSing_InitRobotEffect(
	void *pTalkToSingBuf,
	void *pRobotEffectBuf,
	E_BNDETECTION_ROBOTSOUND_MODE eRobotSoundMode,
	E_BNDETECTION_ROBOTSOUND_FILTER eRobotSoundFilter,
	INT16 i16MonoPitch,
//	E_DECAY_TYPE eEchoDecayType,		// deprecated
	UINT16 u16Options
);
	
BOOL 
TalkToSing_FillPcmBufRobot(
	void *pTalkToSingBuf,
	PINT16 pi16PCMBuf
);

//-------------------------------------Pitch Change----------------------------------------
UINT16								// return buffer size
PitchChange_ConfigEx(
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
PitchChange_InitEx(
	void *pPitchChangeBuf
);

UINT16
PitchChange_StartPlay(
	void* pPC,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	UINT16 u16OutSampleRate);

#define PitchChange_SetInputData 			PitchZone_SetInputData
#define PitchChange_SetOutputSampleRate		PitchZone_SetOutputSampleRate
#define PitchChange_SetPitchShift			PitchZone_SetPitchShift
#define PitchChange_FillPcmBuf				PitchZone_FillPcmBuf
#define	PitchChange_GetPitchZone			PitchZone_GetPitchZone

//--------------------------------------------------------

UINT16								
PitchChange_Config(					// deprecated
	E_BNDETECTION_FILTER_MODE eFilterMode,
	E_PITCHCHANGE_MODE eMode,		
	UINT8 u8LowPitchIndex, 			// 30~100	; u8LowPitchIndex <= u8HighPitchIndex
	UINT8 u8HighPitchIndex,			// 100~190	; u8LowPitchIndex <= u8HighPitchIndex
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
PitchChange_Init(					// deprecated
	void *pPitchChangeBuf,
	UINT8 u8PitchIdx,
	PFN_PITCHCHANGE_SETDIVIDER pfnSetDivider_Callback,
	INT16 *pi16DacDivider,
	INT16 *pi16AdcDivider
);

BOOL 								// deprecated
PitchChange_ChangePitchIdx(
	void *pPitchChangeBuf,
	UINT8 u8PitchIdx
);

//-------------------------------------Pitch Change LFO----------------------------------------

UINT16								// return buffer size
PitchChangeLfo_ConfigEx(
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
PitchChangeLfo_InitEx(
	void *pPCL
);

void
PitchChangeLfo_InitBeatNoteDetect(
	void *pPitchChangeLfoBuf,
	UINT16 u16BeatEngyThreshold);

void
PitchChangeLfo_InitPitchBend(
	void *pPitchChangeLfoBuf,
	UINT32 u32InterpoFactorDest,
	UINT16 u16Duration);	// msec

void
PitchChangeLfo_SetVolLfo(
	void *pPitchChangeLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
	);

void
PitchChangeLfo_SetFreqLfo(
	void *pPitchChangeLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
	);

void
PitchChangeLfo_SetPitchShift(
	void *pPCL,
	INT8 i8PitchShift);
	
void
PitchChangeLfo_InitEnvelope(
	void *pPCL,
	UINT16 u16Option,
	UINT16 u16Freq,
	UINT8 u8RatioMin);
	
#define PitchChangeLfo_InitPitchBand		PitchChangeLfo_InitPitchBend
#define	PitchChangeLfo_FillPcmBuf			PitchZone_FillPcmBuf
#define PitchChangeLfo_GetPitchChangeBuf	PitchZone_GetPitchZone
#define PitchChangeLfo_SetInputData 		PitchZone_SetInputData
#define PitchChangeLfo_SetOutputSampleRate	PitchZone_SetOutputSampleRate

//--------------------------------------------------------

UINT16								
PitchChangeLfo_Config(				// this API is deprecated
	E_BNDETECTION_FILTER_MODE eFilterMode,
	E_PITCHCHANGE_MODE eMode,		
	UINT8 u8LowPitchIndex, 			
	UINT8 u8HighPitchIndex,			
	INT16 i16ExtraInBufSize			
	);

BOOL
PitchChangeLfo_Init(				// this API is deprecated
	void *pPitchChangeLfoBuf,
	UINT8 u8PitchIdx,
	PFN_PITCHCHANGE_SETDIVIDER pfnSetDivider_Callback,
	INT16 *pi16DacDivider,
	INT16 *pi16AdcDivider
	);

BOOL 
PitchChangeLfo_ChangePitchIdx(		// this API is deprecated
	void *pPitchChangeLfoBuf,
	UINT8 u8PitchIdx
	);

//-------------------------------------Pitch Change----------------------------------------

UINT16								// return buffer size
PitchChangeNRT_Config(
	E_PITCHCHANGE_MODE eMode
);

BOOL
PitchChangeNRT_Init(				// deprecated
	void *pPitchChangeBuf,
	UINT32 u32InterpoFactor,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm
);

BOOL
PitchChangeNRT_InitEx(
	void *pPitchChangeBuf,
	INT8 i8PitchShift,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm
);

UINT16
PitchChangeNRT_GetDecoderBufSize(
	INT16 i16FrameSize);
	
void
PitchChangeNRT_InitDecoder(
	void* pPitchChangeBuf,
	void* pPcnrtDecoderBuf,
	INT16 i16FrameLen,
	UINT8* pu8WorkBuf,
	UINT8* pu8TempBuf,
	PFN_AUDIO_DATAREQUEST pfnReadDataCallback,
	PFN_AUDIO_DECODEPROCESS pfnDecodeProcess);

BOOL 
PitchChangeNRT_FillPcmBuf(
	void *pPitchChangeBuf,
	PINT16 pi16PCMBuf
);

UINT8
PitchChangeNRT_CalAvgPitch(
	void *pPitchChangeBuf,
	UINT8* pu8Buf,
	INT16 i16BufSize);

void
PitchChangeNRT_SetSpeedRatio(
	void *pPitchChangeBuf,
	UINT32 u32Ratio);			// Q12

void
PitchChangeNRT_SetPitchRatio(
	void *pPitchChangeBuf,
	UINT32 u32Ratio);			// Q13

void
PitchChangeNRT_SetVol(
	void *pPitchChangeBuf,
	UINT16 u16Vol);	// Q12

INT16
PitchChangeNRT_AnalyzePitch(
	void *pPitchChangeBuf,
	UINT8* pu8Buf,
	INT16 i16BufSize);

void
PitchChangeNRT_SetPitchBuf(
	void *pPitchChangeBuf,
	UINT8* pu8Buf,
	INT16 i16BufSize);

void
PitchChangeNRT_SetPitchCallbackFunc(
	void *pPitchChangeBuf,
	PFN_BNDETECTION_SEEK pfnSeekSylbPitchInfo,
	PFN_BNDETECTION_READ pfnReadSylbPitchInfo);
	
BOOL
PitchChangeNRT_RemoveHeadTailNoise(
	void *pPitchChangeBuf,
	INT32 i32TotalSmplCnt,
	UINT16 u16HeadThdRatio,		// Q8, > 0x100
	UINT16 u16TailThdRatio,		// Q8, > 0x100
	UINT16 u16RewindTime,		// Q8, second
	INT32 *pi32HeadNoiseSmplCnt,
	INT32 *pi32TailNoiseCmplCnt);

UINT16
PitchChangeNRT_CalNormalizeVol(
	void *pPitchChangeBuf,
	INT32 i32TotalSmplCnt);

#define PitchChangeNRT_SetOutputSampleRate	PitchZone_SetOutputSampleRate
#define PitchChangeNRT_SetPitchShift		PitchZone_SetPitchShift
#define PitchChangeNRT_GetPitchZone			PitchZone_GetPitchZone


//---Pitch Shift---------------------------------------------------------------------------

UINT16							
PitchShift_Config(
	UINT16 u16SampleRate,		
	UINT8 u8BeatDetectBufNum,
	INT16 i16ExtraInBufSize		
);

BOOL
PitchShift_Init(
	void *pPitchShiftBuf,
	UINT8 u8Option,
	UINT16 u16BeatEngyThreshold,
	UINT8 u8BeatDetectBufNum,
	PFN_BNDETECTION_GET_SHIFT_SEMITONE pfnGetShiftSemiTone,
	INT8 i8SemitoneShift
);

#define	PitchShift_SetOutputSampleRate	PitchZone_SetOutputSampleRate
#define PitchShift_FillPcmBuf			PitchZone_FillPcmBuf
#define PitchShift_SetInputData			PitchZone_SetInputData
#define PitchShift_GetPitchZone			PitchZone_GetPitchZone


//----------robot sound--------------------------------------------------------------

UINT16								// return buffer size
RobotSound_Config(
	E_BNDETECTION_FILTER_MODE eFilterMode,
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize,			// >= 32, larger if CPU is busy
	INT16 i16MonoPitch,
	UINT8 u8EchoNum
	);

BOOL
RobotSound_Init(
	E_BNDETECTION_ROBOTSOUND_MODE eRobotSoundMode,
	E_BNDETECTION_ROBOTSOUND_FILTER eRobotSoundFilter,
	void *pRobotSoundBuf,
	INT16 i16MonoPitch,
	UINT8 u8EchoNum,
	E_DECAY_TYPE eEchoDecayType,
	UINT16 u16Options
);

void
RobotSound_SetMonoPitch(
	void *pRobotSoundBuf,
	INT16 i16MonoPitch
	);

UINT16
RobotSound_StartPlay(
	void* pRS,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	UINT16 u16OutSampleRate);

#define RobotSound_FillPcmBuf		PitchZone_FillPcmBuf
#define RobotSound_SetInputData		PitchZone_SetInputData
#define RobotSound_GetPitchZone		PitchZone_GetPitchZone


//----------robot sound LFO--------------------------------------------------------------

UINT16								// return buffer size
RobotSoundLfo_Config(
	E_BNDETECTION_FILTER_MODE eFilterMode,
	UINT16 u16SampleRate,		
	INT16 i16ExtraInBufSize,			// larger if CPU is busy
	INT16 i16MonoPitch,
	UINT8 u8EchoNum
	);

BOOL
RobotSoundLfo_Init(
	E_BNDETECTION_ROBOTSOUND_MODE eRobotSoundMode,
	E_BNDETECTION_ROBOTSOUND_FILTER eRobotSoundFilter,
	void *pRobotSoundLfoBuf,
	INT16 i16MonoPitch,
	UINT8 u8EchoNum,
	E_DECAY_TYPE eEchoDecayType,
	UINT16 u16Options
	);

void
RobotSoundLfo_SetVolLfo(
	void *pRobotSoundLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
	);

void
RobotSoundLfo_SetFreqLfo(
	void *pRobotSoundLfoBuf,
	UINT8 u8ModDepth,					// ratio of range, u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
	);

#define RobotSoundLfo_GetRobotSoundBuf	PitchZone_GetPitchZone	
#define RobotSoundLfo_FillPcmBuf		PitchZone_FillPcmBuf


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
UINT16								// return buffer size
HarmonySnd_Config(
	E_BNDETECTION_FILTER_MODE eFilterMode,
	E_HARMONYSND_MODE eMode,
	INT8 i8ChnlNum,
	UINT8 u8BeatDetectBufNum,
	INT16 i16ExtraInBufSize			// >= 32, larger if CPU is busy
);

BOOL
HarmonySnd_Init(
	void *pHarmonySndBuf,
	UINT16 u16BeatEngyThreshold,
	BOOL bMixOrgSnd
);

void
HarmonySnd_SetPitchShiftSemiTone(
	void *pHarmonySndBuf,
	INT8 i8ChnlIdx,
	INT8 i8ShiftPitch,					// in semi-tone
	UINT8 u8Vol,						// 0~255
	INT8 i8ModRange,					// semi-tone 1~3, DEFAULT: 1
	UINT8 u8ModDepth,					// u8ModDepth/255 of 1 semi-tone
	UINT8 u8Modfreq,					// Q4.4, 0 < u8Modfreq <= 10
	UINT8 u8ModDelayTime				// time unit: 5ms
	);

void
HarmonySnd_Enable(
	void *pHarmonySndBuf,
	INT8 i8ChnlIdx
	);

void
HarmonySnd_Disable(
	void *pHarmonySndBuf,
	INT8 i8ChnlIdx
	);

BOOL 
HarmonySnd_FillPcmBuf(
	void *pHarmonySndBuf,
	PINT16 pi16PCMBuf
);

UINT16
HarmonySound_StartPlay(
	void* pHS,
	UINT16 u16BufSize,
	UINT16 u16InSampleRate,
	UINT16 u16OutSampleRate,
	UINT8 u8ChNum);

#define HarmonySnd_SetInputData	PitchZone_SetInputData
#define HarmonySnd_GetPitchZone	PitchZone_GetPitchZone


//------------------------------------------------------------------------------------------
void
Resample_Init(
	INT16* pi16PcmBuf,
	INT16 i16PcmNum);

void
Resample_16To24_Linear(
	INT16* pi16PcmBuf,
	INT16* pi16PcmRaw,
	INT16* pi16PcmOut);

void
Resample_16To24(
	INT16* pi16PcmBuf,
	INT16* pi16PcmRaw,
	INT16* pi16PcmOut);

//------------------------------------------------------------------------------------------
/*
void
FreqShift_Init(
	UINT16 u16SampleRate,
	UINT32 u32ShiftFreq);

INT16
FreqShift_Convert(
	INT16 i16Pcm);
*/
UINT16
FreqShift_Config(
	BOOL bBPF);

void
FreqShift_InitEx(
	void *pFSBuf,
	UINT16 u16SampleRate,
	UINT32 u32ShiftFreq,
	BOOL bBPF);

void
FreqShift_ChangeFreq(
	void *pFSBuf,
	UINT32 u32ShiftFreq);

INT16
FreqShift_ConvertEx(
	void *pFSBuf,
	INT16 i16Pcm);

INT16
FreqShift_Filter(
	void *pFSBuf,
	INT16 i16Pcm);

void
FreqShift_SetBPFCoef64_0_1200(
	void *pFSBuf
);

void
FreqShift_SetBPFCoef64_600_6000(
	void *pFSBuf
);

void
FreqShift_SetBPFCoef64_800_6000(
	void *pFSBuf
);

void
FreqShift_SetBPFCoef64_1000_6000(
	void *pFSBuf
);

//------------------------------------------------------------------------------------------
typedef void (*PFN_USER_EVENT_HANDLER)(void * pBuf, UINT8 u8Idx, UINT8 u8SubIdx);


UINT16
MidiRead_Config(void);

BOOL
MidiRead_InitMidiAddr(
	void* pMR,
	UINT32 u32MidiDataAddr,
	UINT8 u8MidiChn,
	INT8 i8NoteShift,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	PFN_USER_EVENT_HANDLER pfnUserEventHandlerCallback,
	void* pUserEventCallbackBuf
	);

BOOL
MidiRead_InitMidiId(
	void* pMR,
	UINT16 u16MidiID,
	UINT8 u8MidiChn,
	INT8 i8NoteShift,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	PFN_USER_EVENT_HANDLER pfnUserEventHandlerCallback,
	void* pUserEventCallbackBuf
	);

BOOL 
MidiRead_GetHeadSilenceLength(
	void* pMR,
	UINT32* pu32DeltaTime,
	BOOL bCallUserEventHandler);

BOOL
MidiRead_GetNextNoteAndLength(
	void* pMR,
	INT8* pi8Note,
	UINT32* pu32DeltaTime,
	UINT32* pu32NoteDeltaTime,
	BOOL bCallUserEventHandler);

INT8
MidiRead_GetAvgNote(
	void* pMR
);

INT8
MidiRead_GetHighestNote(
	void* pMR
);

INT8
MidiRead_GetLowestNote(
	void* pMR
);

void
MidiRead_GetNoteInfo(
	void* pMR,
	INT8* pi8LowestNote,
	INT8* pi8AvgNote,
	INT8* pi8HighestNote
);

//------------------------------------------------------------------------------------------
UINT16 
PlayDrum_Config(void);

BOOL 
PlayDrum_Init(
	void* pPlayDrumBuf, 
	UINT16 u16MidiId,
	UINT16 i16OutputSampleRate,
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	PFN_PLAY_DRUM	pfnPlayDrum
	);

BOOL PlayDrum_CheckPlay(
	void* pPlayDrumBuf);

//------------------------------------------------------------------------------------------
UINT16 
ImitateTalk_Config(
	UINT16 u16SampleRate		
	);

void
ImitateTalk_Init(
	void* pImitateTalkBuf,
	INT16 i16SndAvgPitch,
	INT16 i16LowEngyWidth,
	PFN_BNDETECTION_SEEK pfnSeekPcm,
	PFN_BNDETECTION_READ pfnReadPcm,
	PFN_BNDETECTION_SEEK pfnSeekSylbPitchInfo,
	PFN_BNDETECTION_READ pfnReadSylbPitchInfo,
	PFN_IMITATETALK_INIT_VOICE pfnGetSylbInfo
	);

void
ImitateTalk_SetNormalizedBuf(
	void *pImitateTalkBuf,
	UINT16* pu16Buf);

BOOL 
ImitateTalk_FillPcmBuf(
	void* pImitateTalkBuf,
	PINT16 pi16PCMBuf
);

void
ImitateTalk_TuneSylbAvgPitch(
	void *pImitateTalkBuf);

#define ImitateTalk_SetOutputSampleRate		PitchChangeNRT_SetOutputSampleRate
#define ImitateTalk_AnalyzePitch			PitchChangeNRT_AnalyzePitch
#define ImitateTalk_SetPitchBuf				PitchChangeNRT_SetPitchBuf
#define ImitateTalk_SetPitchCallbackFunc	PitchChangeNRT_SetPitchCallbackFunc
#define ImitateTalk_GetDecoderBufSize		PitchChangeNRT_GetDecoderBufSize
#define ImitateTalk_InitDecoder				PitchChangeNRT_InitDecoder


//------------------------------------------------------------------------------------------
UINT32
BNDetection_GetVersion(void);


#ifdef  __cplusplus
}
#endif

#endif // __BNDETECTION_H__

