/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/* Copyright(c) Nuvoton Technology Corp. All rights reserved.                                     */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

#ifndef _INSTATE_H_
#define _INSTATE_H_

//%{CodeGen_Block_Start}
extern void InputKey_Change(UINT32 u32InStateIndex);

enum E_INSTATE
{
	E_Default = 0,	//1

	E_INSTATE_END
};

#define Enable_Default	InputKey_Change(E_Default)

//%{CodeGen_Block_End}
#endif //_INSTATE_H_
