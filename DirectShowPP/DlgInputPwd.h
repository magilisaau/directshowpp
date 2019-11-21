#if !defined(AFX_DLGINPUTPWD_H__AD036C49_FE01_43AA_836F_5DB379FFBA12__INCLUDED_)
#define AFX_DLGINPUTPWD_H__AD036C49_FE01_43AA_836F_5DB379FFBA12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputPwd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputPwd dialog

class CDlgInputPwd : public CDialog
{
// Construction
public:
	void GetPwd(CString &strPwd);
	CDlgInputPwd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputPwd)
	enum { IDD = IDD_DLG_INPUT_PASS };
	CString	m_pwd1;
	CString	m_pwd2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputPwd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputPwd)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTPWD_H__AD036C49_FE01_43AA_836F_5DB379FFBA12__INCLUDED_)
