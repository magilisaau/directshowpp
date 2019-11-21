// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CC7EB2B6_46E4_4315_8942_A3B2A27EC6F7__INCLUDED_)
#define AFX_STDAFX_H__CC7EB2B6_46E4_4315_8942_A3B2A27EC6F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef POINTER_64
#if !defined(_MAC) && (defined(_M_MRX000) || defined(_M_AMD64) || defined(_M_IA64)) && (_MSC_VER >= 1100) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#if defined(_WIN64)
#define POINTER_32 __ptr32
#else
#define POINTER_32
#endif
#else
#if defined(_MAC) && defined(_MAC_INT_64)
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#else
#if (_MSC_VER >= 1300) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
#else
#define POINTER_64
#endif
typedef unsigned long POINTER_64_INT;
#endif
#define POINTER_32
#endif
 #endif

#define _CRT_SECURE_NO_WARNINGS
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls 
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <windows.h>
#include <commdlg.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <dsound.h>
#include <strsafe.h>
#include <wmsdk.h>
#include <Wmsdkidl.h >
#include <msacm.h>
#include <windowsx.h>
/* libmad support */
#include "mad.h"

/* libogg support */
#include "ogg/ogg.h"

/* libvorbis support */
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "strsafe.lib")
#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "wmvcore.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "msacm32.lib")

#define USE_DAUDIO TRUE
#define USE_ERROR
#define USE_TRACE


#define   HAVE_BOOLEAN
#define  SHOW_TYPE_EXE    0
#define  SHOW_TYPE_SCR    1
#define  SHOW_TYPE_PRO    2

extern int g_magic_offset;
#define  MAGIC_NUM  (0x7364921+g_magic_offset)
#define  BMPTYPE 0x39528761
#define  JPGTYPE 0x39528762
#define  MP3TYPE 0x39528763
#define  WAVTYPE 0x39528764
#define  WMATYPE 0x39528765
#define  LRCTYPE 0x39528766
typedef struct
{
	int type;
	int autosize;
	int delaytime;
	int direct;
//	int sound;
	int speed;
	int forward;
	unsigned char pwd[8];
	int iEncyFlag;
} T_FILE_HEADER;
extern T_FILE_HEADER g_head;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CC7EB2B6_46E4_4315_8942_A3B2A27EC6F7__INCLUDED_)
