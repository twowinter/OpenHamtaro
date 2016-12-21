/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
---------------------------------------------------------------------------------------------------------
Purpose:
---------------------------------------------------------------------------------------------------------
	1. 	Record encoded data with NuLiteEx encoder to SPIFlash .
	2.	Play decoded data with NuLiteEx decoder from SPIFlash.


---------------------------------------------------------------------------------------------------------
Operation:
---------------------------------------------------------------------------------------------------------
	LED:
		1. 	GPA14: show system standby
		2.	GPA15: light is start to playback; dark is stop to playback. 
		3. 	GPB4: light is start to record; dark is stop to record.

	Key:
		1. 	GPB0: start or stop record.
		2. 	GPB1: start or stop playback.
		3.	GPB3: power down(keep pressing than 4 second)/wake up


---------------------------------------------------------------------------------------------------------
Note:
---------------------------------------------------------------------------------------------------------
	1.	This sample code used 4k section partial erase, programmer make sure SPIFlash support
		4k erase command.
	2. 	The playback sampling rate is read from NuLite data.
	3. 	Playback can be with/without N-times up-sampling to reduce "metal sound"
	   	which is controlled by APU_UPSAMPLE defined in "ConfigApp.h"
	4.	ADC sampling rate is 8000Hz which is defined by ADC_SAMPLE_RATE "ConfigApp.h"
	5.	ADC record can be with/without N-times over-sampling to improve recording quality
		which is controlled by ADC_DOWNSAMPLE defined in "ConfigApp.h"
	6.	SPIFlash performance will decrease as frequency of use. programmer can set PCM_RING_BUF_SIZE
		,ring buffer size, in "ConfigApp.h" to avoid audio sample loss.


---------------------------------------------------------------------------------------------------------
Must Know:
---------------------------------------------------------------------------------------------------------
	1.	The stack size is defined bye "Stack_Size" in "startup_N572.s". Can change the stack size
		by modifing the content of "Stack_Size".
	2.	The chip ROM size and SRAM size is specified by "ROM_SIZE" and "SRAM_SIZE"in  linker settings
		Open the linker setting and find the --pd="-DSRAM_SIZE=0xNNNN" --pd="-DROM_SIZE=0xMMMMM" in "misc controls" 
	3.	When the error happen:
			"L6388E: ScatterAssert expression (ImageLimit(_SRAM) < (0xNNNNNNNN + 0xMMMM)) failed on line xx" 
		It means the size of variables+stack is over chip SRAM size.
		Please open map file to see the overed SRAM size and reduce variables or stack size to fit chip SRAM size.
	4.	When the error happen:
			"L6220E: Load region _ROM size (NNNNN bytes) exceeds limit (MMM bytes)."
		It means the size of programs is over chip ROM size.
		Please open map file to see the overed programs size and reduce programs size to fit chip ROM size.
