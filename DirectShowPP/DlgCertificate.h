#if !defined(AFX_DLGCERTIFICATE_H__5098AC3D_B395_4FCE_B73A_B83E2FA99A99__INCLUDED_)
#define AFX_DLGCERTIFICATE_H__5098AC3D_B395_4FCE_B73A_B83E2FA99A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCertificate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCertificate dialog

class CDlgCertificate : public CDialog
{
// Construction
public:
	void getPwd(CString &strPwd);
	CDlgCertificate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCertificate)
	enum { IDD = IDD_DLG_CERT };
	CString	m_pwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCertificate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCertificate)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCERTIFICATE_H__5098AC3D_B395_4FCE_B73A_B83E2FA99A99__INCLUDED_)
