#if !defined(AFX_LRCSEARCHDLG_H__14A4F225_0ED0_4E38_8067_A3832D35EBFA__INCLUDED_)
#define AFX_LRCSEARCHDLG_H__14A4F225_0ED0_4E38_8067_A3832D35EBFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LrcSearchDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLrcSearchDlg dialog

class CLrcSearchDlg : public CDialog
{
// Construction
public:
	CLrcSearchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLrcSearchDlg)
	enum { IDD = IDD_LRC_PATH };
	CString	m_path;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLrcSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLrcSearchDlg)
	afx_msg void OnSetSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LRCSEARCHDLG_H__14A4F225_0ED0_4E38_8067_A3832D35EBFA__INCLUDED_)
