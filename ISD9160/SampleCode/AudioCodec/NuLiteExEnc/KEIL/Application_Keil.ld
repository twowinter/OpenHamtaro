#!armcc -E --device=DARMCM1
#define ROM_START			0x00000000
#define SRAM_START			0x20000000
; The ROM_SIZE and SRAM_SIZE are defined in linker settings
; Open the linker setting and find the --pd="-DSRAM_SIZE=0xNNNN" --pd="-DROM_SIZE=0xMMMMM" in "misc controls"

_ROM					ROM_START	 ROM_SIZE
{
	_RESET		 0 OVERLAY
	{
		* (RESET, +FIRST)
	}
	_CODE		+0  ALIGN 4
	{
		* (+RO)
	}
	_STACK		SRAM_START UNINIT ALIGN 4
	{
		* (STACK)
	}

	_SRAM		+0 ALIGN 4
	{
		* (+RW, +ZI)
		* (HEAP)
	}
}

ScatterAssert(ImageLimit(_SRAM) < (SRAM_START+SRAM_SIZE))

; When the error happen:
;	"L6388E: ScatterAssert expression (ImageLimit(_SRAM) < (0xNNNNNNNN + 0xMMMM)) failed on line xx" 
; It means the size of variables+stack is over chip SRAM size.
; Please open map file to see the overed SRAM size and reduce variables or stack size to fit chip SRAM size.
; When the error happen:
;	"L6220E: Load region _ROM size (mmmmm bytes) exceeds limit (nnn bytes)."
; It means the size of programs is over chip ROM size.
; Please open map file to see the overed programs size and reduce programs size to fit chip ROM size.

