/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
---------------------------------------------------------------------------------------------------------
File Information:
---------------------------------------------------------------------------------------------------------
	[Source]
		+ main.c
			- System clock configuration.
			- Keypad configuration.
			- SPI Flash configuration.
			- Speaker configuration.
			- MIC configuration.
			- Output pin configuration.
			- UltraIO configuration.
			- Application Initiation.
			- Processing loop:
				* Codec processing(use functions in "AppFunctions.c").
				* Voice effect processing(use functions in "AppFunctions.c").
				* Keypad handling(use functions in "AppFunctions.c").
				* Etc.
		
		+ AppFunction.c
			- Functions to handle main operations:
				* Initiate application.
				* Start audio playback.
				* Stop  audio playback.
				* Produce PCM data for audio playback.
				* Start audio recording.
				* Stop  audio recording.
				* Use recorded data to do:
					a. encoding
					b. doing voice effect
					c. write to storage
					d. etc.
			- The above functions use codes in "xxxApp" folder and "Framework" folder to complete necessary operations.
		
		+ AppCallback.c
			- Implement these functions and used in "AppInfo.c":
				* Read storage funciton.
				* Write storage function.
				* Read MIDI wave table function.
				* Process user event function.
		
		+ InputKeyActions.c
			- Direct trigger key and matrix key falling, rising, long pressing handling functions.
				* Direct trigger key handling functions are referenced in a lookup table "g_asTriggerKeyHandler" in ConfigIO.h.
				* Matrix key handling functions are reference in a lookup talbe "g_asMatrixKeyHandler" in ConfigIO.h.
				* Both lookup tables are used by keypad library to call at key action being identified.
			- Default trigger key and matrix key handler is "Default_KeyHandler()"
		
		+ InterruptHandler.c
			- ADC ISR: handle input audio raw data and put into intranl buffer.
			- APU ISR: handle ouput audio data and put into APU buffer.
			- TimeF ISR: handle de-bounce time for direct trigger key and matrix key.
			- Time2 ISR: handle UltraIO curve output with F/W PWM mode.
			- GPIOAB ISR: handle wake up by GPIOA or GPIO B.
			- Other interrupt ISRs.
		
		+ Powerdown.c
			- Close using IPs, storage device and clock source before entering power down.
			- Handle particulars to avoid unstable power down:
				* Disable multi-function of GPIO pins and driver QUASI mode to avoid floating.
				* Change system clock source from PLL to RC.
				* Process debounce issue if using GPIO wake up.
			- Implement three wake up function:
				* GPIO.
				* RTC.
				* WDT.
			- Open using IPs, storage device and clock source after exiting power down.

	[Include]
		+ SPIFlashMap.h
			- Provide whole view on SPI flash space usage.
		
		+ App.h
			- Memory Allocation by data structure "S_APP"
		
		+ ConfigApp.h
			- APU
				* function enable/disable
				* up sampling enable/disable
				* Clip PCM or not during output to APU
				* ramp up/down step value
				* default gain setting at APU startup
			- ADC
				* function enable/disable
				* down sampling enable/disable
				* default gain setting at ADC startup
			- Select mixer operation mode
			- UltraIO used timer and timer clock source
			- UltraIO curve
				* enable/diable ultraio curve output by FW PWM
				* enable/diable ultraio curve output by HW PWM
				* can be configurated from keil IDE menu: "[Tools]->[NuVoice Library Setting]"
			- SPI Flash
				* SPI interface
				* SPI operation clock
			- input/output pin
				* enable/disable direct trigger key functions
				* enable/disable matrix key functions
				* enable/disable output pin functions
				* can be configurated from keil IDE menu: "[Tools]->[NuVoice Library Setting]"
			- Powerdown
				* function enable/disable
				* enable/disable GPIO wake up trigger
				* enable/disable RTC wake up trigger
				* enable/disable WDT wake up trigger
				* define port and pin for GPIO wake up trigger
				* define trigger time for RTC wake up
				* define trigger time for WDT wake up
			- Etc.
		
		+ ConfigSysClk.h
			- Configure CPU clock
				* main clock source: internal RC or exteranl 32K+PLL
				* CPU operation frequency
		
		+ ConfigUltraIO.h
			- Configurations and lookup table for UltraIO Curve output handling.
			- can be configurated from keil IDE menu: "[Tools]->[NuVoice Library Setting]"
		
		+ ConfigIP.h
			- Define IP clocks for different NuVoice chips
		
		+ ConfigIO.h
			- Define matrix key.
			- Define direct trigger key.
			- Define output key.
			- Key de-bounce time.
			- Define matrix key actions:
				* Key falling/rising/long press action functions implemented in "InpuKeytActions.c".
			- Define direct trigger key actions:
				* Key falling/rising/long press action functions implemented in "InpuKeytActions.c".
			- can be configurated from keil IDE menu: "[Tools]->[NuVoice Library Setting]".
		
		+ InState.h
			- Define keypad group state for further using. User can change keypad group
			  via E_INSTATE definition.
			- can be configurated from keil IDE menu: "[Tools]->[NuVoice Library Setting]"
			  
	[AudioRes]
		+ AudioRes.uid
			- Define the encoding format used to encode WAV, WIO, MIDI files.
			- Define play equations.
			- Used by "WindowsPlatform\Tool\AudioTool\AudioTool.exe" to generate.
				a. standalone file:
					*.lp8(LP8 format).
					*.mp4(MD4 format).
					*.p16(P16 format).
					*.nus(NuSound format).
					*.nul(NuLite format).
					*.nuv53(NuVox53 format).
					*.nuv63(NuVox63 format).
					*.bin(MIDI format).
				b. pack all encoded sound files and MIDI wavetable file("AudioRes\Output\AudioRes_WTBInfoTableMerge.ROM") to a ROM file("AudioRes\Output\AudioRes_AudioInfoMerge.ROM").

		+ WAVTable_Middle.idt
			- Define the timbres used in MIDI playing.
			- The wavetable includes instruments and drums' information.
			- Used by "WindowsPlatform\Tool\AudioTool\AudioTool.exe" to generate:
				a. wavetable file("AudioRes\Output\AudioRes_WTBInfoTableMerge.ROM")
				b. pack with all encoded sound files and MIDI file to a ROM file("AudioRes\Output\AudioRes_AudioInfoMerge.ROM")
	
	[CMSIS]
		+ startup_ISD9100.s
			- Startup file to
				* configure stack point.
				* initiate SRAM with initial value.
				* IRQ function table.
				* run to main().
		
		+ system_ISD9100.c
			- Define global variable,SystemCoreClock, to save current core clock. User can 
			  call this variable to view current core clock.
		
	[xxxApp]
		+ ConfigxxxApp.h(in "xxxApp" folder)
			-  configuration and data structure for this xxxApp.

		+ xxxApp.c(in "xxxApp" folder)
			-  main process functions for this xxxApp.

	[Framework]
		+ AudioMixer.c
			- Mix multiple channel PCMs with same or different sample rates.
			- Calculate the maxium sample rate based on mixing channels.
		
		+ AudioRom.c
			- Functions to lookup ROM file header.
			- The ROM file is generated by "WindowsPlatform\Tool\AudioTool\AudioTool.exe".
		
		+ BufCtrl.c
			- Functions to duplicate audio data from decoder or encoder for further using.
			
		+ ConfigIO.c
			- Functions to configure IO pin modes according to definitions in "ConfigIOPin.h".
		
		+ MicSpk.c
			- MIC functions:
				* Open MIC.
				* Close MIC.
				* Pause MIC recording.
				* Resume MIC recording.
				* Change MIC recording sampling rate.
			- SPK functions:
				* Open speaker.
				* Close speaker.
				* Pause speaker playback.
				* Resume speaker playback.
				* Change speaker playback sampling rate.
				
		+ Playback.c
			- APU ISR to handle:
				* Get PCM data from:
					a. Ring buffer.
					b. Callback for special usage(example: call audio mixer function).
				* Up sampling(1x, 2x, 4x) to reduce "metal sound".
				* Clipping at PCM value > 4095 or < -4096.
				* Ramp down(if force to end playback).
			- Playback start/stop.
			- Playback pause/resume.
			- Playback mute on/off.
			- Playback volume control.

		+ Record.c
			- ADC ISR to do:
				* Down sampling(1x, 2x, 4x).
				* Noise filtering.
				* Move PCM data to:
					a. ring buffer.
					b. Callback for special usage(example: call voice effect function).
			- Recording start/stop
		
		+ SPIFlashUtil.c
			- SPIFlash utility is used to encoding for partial erase partial write based on 4k bytes block.
		
		+ SysClk.c
			- Functions to configure system clock divisor and source:
				* Internal RC.
				* External 32k XTL.
			- Functions to wait system clock stable if using PLL from external 32k XTL.
			
		+ UltraIO.c
			- Functions to configure UltraIO and do output Ultraio Curve according the settings in ConfigUltraIO.h.
	
	[Storage]
		+ SPIFlash.c
			- Implement SPIFlash read, write and erase functions.
	[IO]
		+ Keypad.c
			- Implement direct and matrix functions.
	[Driver]
		+ XXX.c(in "..\Library\StdDriver" folder)
			- Contain all driver source files.
	
	[Library]
		+ XXX.lib(in "..\Library\Audio\arm" folder)
			- Contain all audio codec libraries.