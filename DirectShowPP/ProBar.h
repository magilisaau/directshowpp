#if !defined(AFX_PROBAR_H__2C5D4B81_2DC2_47EF_B029_DE44B9690A5F__INCLUDED_)
#define AFX_PROBAR_H__2C5D4B81_2DC2_47EF_B029_DE44B9690A5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProBar dialog

class CProBar : public CDialog
{
// Construction
public:
	CProBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProBar)
	enum { IDD = IDD_DIALOG2 };
	CProgressCtrl	m_probar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProBar)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROBAR_H__2C5D4B81_2DC2_47EF_B029_DE44B9690A5F__INCLUDED_)
