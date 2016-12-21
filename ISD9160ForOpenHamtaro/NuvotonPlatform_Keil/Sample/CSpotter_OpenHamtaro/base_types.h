/*
	Copyright (c) 2002 Cyberon Corp.  All right reserved.
	File: base_types.h
	Desc: Define all base types
	Author: Calvin
	Date: 2013/05/02
	Version : 1.50.00
*/

#ifndef	__BASE_TYPES_H
#define	__BASE_TYPES_H

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)

	#include <windows.h>

	typedef signed char INT8;
	typedef unsigned short USHORT;
	typedef unsigned int UINT;
	typedef unsigned long ULONG;
	typedef unsigned short WCHAR;

#else

	typedef signed char CHAR;
	typedef unsigned char BYTE;
	typedef signed char INT8;
	typedef signed short SHORT;
	typedef unsigned short WORD;
	typedef unsigned short USHORT;
	typedef signed long LONG;
	typedef unsigned int DWORD;
	typedef unsigned long ULONG;
	typedef signed int INT;
	typedef unsigned int UINT;
//	typedef int BOOL;
	typedef void VOID;
	typedef unsigned short WCHAR;	/*typedef wchar_t WCHAR;*/

#endif

#ifdef UNICODE
#undef UNICODE
#endif

#define UNICODE WCHAR

#ifdef LPVOID
#undef LPVOID
#endif
#define LPVOID	void*

#ifndef TRUE
#define TRUE	(1 == 1)
#endif

#ifndef FALSE
#define FALSE	(1 == 0)
#endif

#ifndef HANDLE
#define HANDLE	VOID*
#endif

#ifndef NULL
#define NULL	((VOID*)0)
#endif

#define PNULL	((VOID*)0)

#ifndef STATIC
#define STATIC 	static
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#if defined(_WIN32)
	#define EXPAPI WINAPI
#else
	#define EXPAPI
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(a)	if ( (a) != NULL ) {delete (a); (a) = NULL;}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(a)	if ( (a) != NULL ) {free(a); (a) = NULL;}
#endif

#ifndef INLINE
#ifdef _WIN32
#define INLINE		_inline
#else
#define INLINE		__inline
#endif
#endif

#endif	/* __BASE_TYPES_H */
