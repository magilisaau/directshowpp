// Lrc.h: interface for the CLrc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LRC_H__4AE2993A_1708_4662_9CDD_0F3ABEB1ECC6__INCLUDED_)
#define AFX_LRC_H__4AE2993A_1708_4662_9CDD_0F3ABEB1ECC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxtempl.h"
struct TLrcRecord
{
    UINT m_time;
    CString m_str;
};

class CLrc  
{
public:
	CString GetNext(UINT time);
	CString GetTxt(UINT Time);
	void Free();
	BOOL Open(CString strFile);
	CLrc();
	virtual ~CLrc();
public:
	void Patch();
	void Modify(int forward);
	CArray<TLrcRecord*,TLrcRecord*> m_recordArray;
	CString ti;
	CString ar;
	CString al;
	CString by;
	int offset;
	CString m_file;
};

#endif // !defined(AFX_LRC_H__4AE2993A_1708_4662_9CDD_0F3ABEB1ECC6__INCLUDED_)
