/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __MIDI_SYNTHESIZER_H__
#define __MIDI_SYNTHESIZER_H__

//#ifndef WIN32
#include "Platform.h"
#include "SysInfra.h"
//#endif

#ifdef	__cplusplus
extern "C"
{
#endif
//--------------------------------------------------------------------------
#define MIDISYN_MAJOR_NUM		2
#define MIDISYN_MINOR_NUM		00
#define MIDISYN_BUILD_NUM		1

//#define AUDIOSYN_POLY_INBUFSIZE		(0x333) // the more the better
//#define AUDIOSYN_POLY_INBUFSIZE		(0x20) // the more the better
#define AUDIOSYN_POLY_INBUFSIZE		(0x38) // the more the better
//#define AUDIOSYN_POLY_INBUFSIZE		(0x4) // the more the better
//#define AUDIOSYN_POLY_INBUFSIZE		(0x3) // the more the better


#define MIDISYN_VERSION_NUM _SYSINFRA_VERSION(MIDISYN_MAJOR_NUM, MIDISYN_MINOR_NUM, MIDISYN_BUILD_NUM)


// E_MIDISYN_INCORRECTDATAID          	Input data ID error
// E_MIDISYN_INCORRECTCHANNELID       	Input channel ID error
// E_MIDISYN_NOTMIDIFORMAT       		Target data is not midi format
// E_MIDISYN_NOTSUPPORTMIDIEVENT		Midi event not support
#define E_MIDISYN_INCORRECTDATAID		_SYSINFRA_ERRCODE(TRUE, MODULE_ID_MIDISYN, 1)
#define E_MIDISYN_INCORRECTCHANNELID	_SYSINFRA_ERRCODE(TRUE, MODULE_ID_MIDISYN, 2)
#define E_MIDISYN_NOTMIDIFORMAT			_SYSINFRA_ERRCODE(TRUE, MODULE_ID_MIDISYN, 3)
#define E_MIDISYN_NOTSUPPORTMIDIEVENT	_SYSINFRA_ERRCODE(TRUE, MODULE_ID_MIDISYN, 4)

typedef UINT32 (*PFN_MIDISYN_GETDATA)(void * pDesAddr, UINT32 u32Position, UINT32 u32ByteNum);
typedef UINT32 (*PFN_MIDISYN_USREVENT)(UINT16 u16Index, UINT16 u16SubIndex);

//20110517@Callback function for 
typedef UINT32 (*PFN_MIDISYN_NOTEONINFO)(UINT16 u16MidiID, UINT16 u16MidiChannel, UINT16 u16NoteNum);
typedef UINT32 (*PFN_MIDISYN_NOTEOFFINFO)(UINT16 u16MidiID, UINT16 u16MidiChannel, UINT16 u16NoteNum);
typedef UINT32 (*PFN_MIDISYN_PITCHBENDINFO)(UINT16 u16MidiID, UINT16 u16MidiChannel, UINT16 u16PitchBend);
typedef UINT32 (*PFN_MIDISYN_PITCHSENINFO)(UINT16 u16MidiID, UINT16 u16MidiChannel, UINT16 u16Sensitivity);
typedef UINT32 (*PFN_MIDISYN_ENDOFTRACK)(UINT16 u16MidiID);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Initiate midi synthesizer to get required memory.
// Arguments:
//  u16MidiChannelNum [in]
//		Number of active midi channels
//	u16PolyphonyNum [in]
//		Maximum number of notes playing at the same time
// Return:
//	Total memory required to run midi synthesizer.
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32	
MidiSyn_NeedVariableMemSize(
	UINT16 u16MidiChannelNum,
	UINT16 u16PolyphonyNum
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Initiate variables memory mapping for midi synthesizer.
// Arguments:
//  u32AudioBase [in]
//		The address of variables memory mapping for midi synthesizer
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_InitAllocateMem(
	UINT32 u32AudioBase
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Monitor following midi events, library will call back to AP if found these events:
//		1. Note on
//		2. Note off
//		3. Pitch bend change
//		4. Pitch bend sensitivity
//		5. All note off
//		6. Reset all
// Arguments:
//  
// Return:
//	
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32
MidiSyn_RegPitchInfoCallback(
	PFN_MIDISYN_NOTEONINFO pfnNoteOnInfoCallback,
	PFN_MIDISYN_NOTEOFFINFO pfnNoteOffInfoCallback,
	PFN_MIDISYN_PITCHBENDINFO fpnPitchBendInfoCallback,
	PFN_MIDISYN_PITCHSENINFO fpnPitchSensitivityCallback
	);

UINT32
MidiSyn_RegMidiCallback(
	PFN_MIDISYN_ENDOFTRACK fpnEndOfTrack
);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Set playing audio sample rate
// Arguments:
//  pu32OutputSampleRate [in]
//		Play sample rate 8000, 12000, 16000.
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_SampleRateSetting(
	UINT32 u32OutputSampleRate
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Initiate audio resource and wave-table resource
// Arguments:
//  pfnGetDataCallback [in]
//		Callback function to get midi data.
//	pfnUsrEventCallback [in]
//		Callback function for user event message in midi.
//	u32WTBTableAddr [in]
//		Wave table address in ROM file that is defined in header (MIDISYN_WTBOFFSET)
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_ResourceSetting(
	PFN_MIDISYN_GETDATA pfnGetDataCallback,
	PFN_MIDISYN_USREVENT pfnUsrEventCallback,
	UINT32	u32WTBTableAddr
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Output PCM data after mixed all polyphonies.	
// Arguments:
//  pu16OutputBufPtr [in]
//		Buffer to keep mixed PCM data.
//	u16OutPutPCMSize [in]
//		The sample number will be put into PCM buffer.
// Return:
//	Real sample number is put into PCM buffer.
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 
MidiSyn_SynthesizeData(
	void* pi16OutputBufPtr, 
	UINT16 u16OutPutPCMSize
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Check whether the specified midi channel is playing a midi or not
// Arguments:
//  u16ChannelID [in]
//		MIDI channel ID
// Return:
//	TRUE: The given channel ID has been assigned to play a midi.
//	FALSE: The given channel ID is idle now.
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL MidiSyn_MidiBusy(
	UINT16 u16ChannelID
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Check if any playing midi, or even midi note
// Arguments:
//  None.
// Return:
//	TRUE: At least one playing audio.
//	FALSE: All MIDI channels are idle now.
///////////////////////////////////////////////////////////////////////////////////////////////////////	
BOOL 
MidiSyn_PlayBusy(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Stop playback all audio that is playing.
// Arguments:
//  None.
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_StopAll(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Stop playback a midi that is playing on a specific midi channel.
// Arguments:
//  u16ChannelID [in]:
//		MIDI Channel ID
// Return:
//	TRUE: Successfully stop playback a playing midi
//	FALSE: Fail to stop a playing midi.
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	
MidiSyn_StopMidi(
	UINT16 u16ChannelID
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Recess a midi that is playing on a specific midi channel
// Arguments:
//  u16ChannelID [in]
//		MIDI Channel ID
// Return:
//	TRUE: Pause a playing midi successfully.
//	FALSE: Fail to pause.
///////////////////////////////////////////////////////////////////////////////////////////////////////	
BOOL	
MidiSyn_PauseMidi(
	UINT16 u16ChannelID
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Resume a paused midi that is playing on a specific midi channel	
// Arguments:
//  u16ChannelID [in]:
//		MIDI Channel ID
// Return:
//	TRUE: Successfully resume a paused midi.
//	FALSE: Resume a midi fail after pause.
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	
MidiSyn_ResumeMidi(
	UINT16 u16ChannelID
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Set the midi volume that is playing on a specific midi channel.
// Arguments:
//  u16ChannelID [in]:
//		MIDI channel ID.
//	u16MidiVol [in]:
//		Volume (0-127) of playing MIDI
// Return:
//	TRUE: Successfully set midi volume.
//	FALSE: Fail to set midi volume.
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	
MidiSyn_SetVolume(
	UINT16 u16ChannelID, 
	UINT16 u16MidiVol
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
//  Change the midi tempo ratio that is playing on a specific midi channel.
// Arguments:
//  u16ChannelID [in]:
//		MIDI Channel ID
//	u16TempoRatio [in]:
//		Tempo ratio value
// Return:
//	TRUE: Successfully set midi tempo ratio.
//	FALSE: Fail to set midi tempo ratio.
/////////////////////////////////////////////////////////////////////////////////////////////////////// 	
BOOL 
MidiSyn_SetTempoRatio(
	UINT16 u16ChannelID, 
	UINT16 u16TempoRatio
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Change the midi pitch offset that is playing on a specific midi channel
// Arguments:
//  u16ChannelID [in]:
//		MIDI Channel ID
//	i16PitchOffset [in]:
//		Offset value (-n ~+n)
// Return:
//	TRUE: Successfully set midi pitch offset.
//	FALSE: Fail to set midi pitch offset.
/////////////////////////////////////////////////////////////////////////////////////////////////////// 	
BOOL	
MidiSyn_SetPitchOffset(
	UINT16 u16ChannelID, 
	INT16 i16PitchOffset
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Change the midi channel volume that is playing on a specific midi channel.
// Arguments:
//  u16ChannelID [in]:
//		MIDI Channel ID
//	u16MidiChannel [in]:
//		MIDI channel number index (1-16)
//	u16MidiChannelVol [in]:
//		Volume of MIDI channel (0-127).
// Return:
//	TRUE: Successfully set midi channel volume.
//	FALSE: Fail to set midi channel volume.
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	
MidiSyn_SetChannelVolume(
	UINT16 u16ChannelID, 
	UINT16 u16MidiChannel, 
	UINT16 u16MidiChannelVol
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	For dynamic and partial loading midi data from a packet of audio data. To load an audio 
//	data from external storage to a specified RAM buffer and playback by giving audio info 
//	address, audio index, RAM address of FIFO, FIFO size, ChannelID used to play the audio, 
//	play volume, and play count.
// Arguments:
//  u32AudioInfoAddr [in]:
//		Audio data address in external storage
//	i16AudioID [in]:
//		Audio index from the file xxxxx.ROM
//	pu8FIFOBuf [in]:
//		Memory address of FIFO buffer for partial loading
//	u32FIFOSize [in]:
//		Size of FIFO buffer for partial loading
//	u16ChannelID [in]:
//		Channel ID (1~Maxium channel ID), but 0 will be also regarded as channel 1. 
//	u16Vol [in]:
//		Volume value (0~127)
//	u16Repeat [in]:
//		Play count, 0 means endlessly repeat.
// Return:
//	Error code or E_SUCCESS.
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32	
MidiSyn_PlayDataIDByPartLdr(
	UINT32 u32AudioInfoAddr, 
	INT16 i16AudioID, 
	PUINT8 pu8FIFOBuf, 
	UINT32 u32FIFOSize, 
	UINT16 u16ChannelID, 
	UINT16 u16Vol, 
	UINT16 u16Repeat
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	For dynamic and partial loading midi data from a packet of audio data. Load audio data 
//	to specified RAM buffer by giving audio data address, data size, RAM address of FIFO, 
//	FIFO size, ChannelID, volume, and play count.
// Arguments:
//  u32AudioDataAddr [in]:
//		Audio data address in external storage.
//	u32AudioDataSize [in]:
//		Audio data size.
//	pu8FIFOBuf [in]:
//		Memory address of FIFO buffer for partial loading
//	u32FIFOSize [in]
//		Size of FIFO buffer for partial loading
//	u16ChannelID [in]
//		Channel ID (1~Maxium channel ID), but 0 will be also regarded as channel 1. 
//	u16Vol [in]
//		Volume value (0~127)
//	u16Repeat [in]
//		Play count, 0 means endlessly repeat.
// Return:
//	Error code or E_SUCCESS.
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32	
MidiSyn_PlayExternDataByPartLdr(
	UINT32 u32AudioDataAddr,
	UINT32 u32AudioDataSize,
	PUINT8 pu8FIFOBuf,
	UINT32 u32FIFOSize,
	UINT16 u16ChannelID,
	UINT16 u16Vol,
	UINT16 u16Repeat,
	PFN_MIDISYN_GETDATA pfnGetDataCallback
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	For dynamic and partial loading audio, check and fill the partial loading buffer.
//  When the solution of audio data partial loading is applied, check all playing channels 
//	and fill-up audio data automatically.
// Arguments:
//  None.
// Return:
//	None.
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
void MidiSyn_CheckFillAudioBufferByPartLdr(void);
#define MidiSyn_CheckFillAudioBufferByPartLdr() \
	;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Get version inforamtion.
// Arguments:
//  
// Return:
//	
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 
MidiSyn_GetVersion(void);


//================================================================================================
//External Note control APIs
//================================================================================================

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Play a midi note
// Arguments:
//  u16InsID [in]
//		Instrument patch number (0-127)
//	u16NoteNum [in]
//		Midi note number (0-127), Middle C (C4) is 60
//	u16MidiChannel [in]
//		Midi channel number (1-16)
//	u16Vol [in]
//		Volume value (0-127) for the playing note
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_ExtNoteOn(
	UINT16 u16InsID, 
	UINT16 u16NoteNum,
	UINT16 u16MidiChannel, 
	UINT16 u16Vol
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Release/End a plying midi note
// Arguments:
//  u16InsID [in]
//		Instrument patch number (0-127)
//	u16NoteNum [in]
//		Midi note number (0-127), Middle C (C4) is 60
//	u16MidiChannel [in]
//		Midi channel number (1-16)
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////	 
void MidiSyn_ExtNoteOff(
	UINT16 u16InsID, 
	UINT16 u16NoteNum, 
	UINT16 u16MidiChannel
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Set external note pitch offset.
// Arguments:
//  i16PitchOffset [in]
//		Pitch offset value
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_SetExtPitchOffset(
	INT16 i16PitchOffset
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Set external note pitch bend value.
// Arguments:
//  u8PitchBend [in]:
//		External note pitch bend value
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_SetExtPitchBend(
	UINT8 u8PitchBend
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
//  Set external note modulation depth value
// Arguments:
//  u8ModulationDepth [in]:
//		External note modulation depth value
// Return:
//	None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void MidiSyn_SetExtModulationDepth(
	UINT8 u8ModulationDepth
	);
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
// 	Set external note volume.
// Arguments:
//  u8Vol [in]:
//		External note volume
// Return:
//	None.
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
void MidiSyn_SetExtVolume(
	UINT8 u8Vol
	);
	
//================================================================================================
//Sound module control APIs
//================================================================================================

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Description:
//		Before parsed UAC's midi command, audio synthesizer must initialize a midi structure to keep 
//		all information came from UAC's midi commands.
//	Argument:
//		None.
//	Return:
//		None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	MidiSyn_SoundModuleInitialize(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Description:
//		Unitialize about sound module.
//	Argument:
//		None.
//	Return:
//		None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	MidiSyn_SoundModuleUninitialize(void);


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Description:
//		To parse Midi event codes which come from UAC midi class. 
//	Argument:
//		pu8MIDIBuffer [in]:
//			To point to the buffer kept midi event codes, following talbe is it's format:
//		u8MIDICmdLen[in]:
//			Buffer length.
//	Return:
//		Error code or E_SUCCESS
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32
MidiSyn_SoundModuleEventDecoder(
	PUINT8 pu8MIDIBuffer,
	UINT8 u8MIDICmdLen
	);

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Description:
//		Output PCM data after mixed all polyphonies without including external polyphonies.	
//	Argument:
//		pu16OutputBufPtr [in]:
//			Buffer to keep mixed PCM data.
//		u16OutPutPCMSize [in]:
//			The sample number will be put into PCM buffer.
//	Return:
//		None.
///////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 
MidiSyn_SoundModuleSynData(
	void* pi16OutputBufPtr, 
	UINT16 u16OutPutPCMSize
	);

#ifdef	__cplusplus
}
#endif

#endif	//#ifndef __MIDI_SYNTHESIZER_H__









