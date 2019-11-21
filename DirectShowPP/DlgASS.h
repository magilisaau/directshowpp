#if !defined(AFX_DLGASS_H__62D3EB1B_D2FE_4726_B9F5_D3CDEFA7745C__INCLUDED_)
#define AFX_DLGASS_H__62D3EB1B_D2FE_4726_B9F5_D3CDEFA7745C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgASS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgASS dialog

class CDlgASS : public CDialog
{
// Construction
public:
	CDlgASS(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgASS)
	enum { IDD = IDD_LRC_ASS };
	CString	m_lrc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgASS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgASS)
	afx_msg void OnSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGASS_H__62D3EB1B_D2FE_4726_B9F5_D3CDEFA7745C__INCLUDED_)
