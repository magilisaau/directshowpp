#if !defined(AFX_MYLISTBOX_H__FFBC9235_FFED_4E34_940D_5D93BC043A4A__INCLUDED_)
#define AFX_MYLISTBOX_H__FFBC9235_FFED_4E34_940D_5D93BC043A4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyListBox window
#include "myresource.h"
class CComDlg;
class CMyListBox : public CListBox
{
// Construction
public:
	CMyListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnClear();
	void onremove();
	void onadd();
	virtual ~CMyListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListBox)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	CComDlg* m_pParent;
public:
	void SetParent(CComDlg* pWin);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTBOX_H__FFBC9235_FFED_4E34_940D_5D93BC043A4A__INCLUDED_)
