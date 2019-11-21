// EFile.h: interface for the CEFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFILE_H__70CBE803_37DE_4ECA_B082_ED9E36C4BA09__INCLUDED_)
#define AFX_EFILE_H__70CBE803_37DE_4ECA_B082_ED9E36C4BA09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEFile :public CFile 
{
public:
	BOOL isOpen();
	virtual void Close();
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,CFileException* pError = NULL);
	int ReadOneLine(CString& strline);
	CEFile();
	virtual ~CEFile();
private:
	BOOL m_open;
	CString m_file;
};

#endif // !defined(AFX_EFILE_H__70CBE803_37DE_4ECA_B082_ED9E36C4BA09__INCLUDED_)
