// LrcBuf.h: interface for the CLrcBuf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LRCBUF_H__2B05391C_5EC7_4137_A3F8_7AEFB61A77DB__INCLUDED_)
#define AFX_LRCBUF_H__2B05391C_5EC7_4137_A3F8_7AEFB61A77DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IniFile.h"
class CLrcBuf  
{
public:
    int  GetMusicInfo(CString strFile, CString &strTitle, CString &strArtist);
	bool Changed();
	void SetSearchPath(CString path);
	void Del(CString song);
	void Add(CString song,CString lrcPath,int forward);
	bool Find(CString song,CString& lrcFilePath,int &forward);
	bool Store(CString strFile);
	bool Load(CString strFile);
	CLrcBuf();
	virtual ~CLrcBuf();
public:
	CString GetSearchPath();
    CString m_searchPath;
    CIniFile m_iniFile;
};

#endif // !defined(AFX_LRCBUF_H__2B05391C_5EC7_4137_A3F8_7AEFB61A77DB__INCLUDED_)
