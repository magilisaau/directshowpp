#if !defined(AFX_CONFIGDLG_H__AF3D44B9_50E1_4449_B7F7_985A216A8B9E__INCLUDED_)
#define AFX_CONFIGDLG_H__AF3D44B9_50E1_4449_B7F7_985A216A8B9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_PARAM_CONFIG };
	UINT	m_delayTime;
	int		m_radioSpeed;
	int		m_radioAutosize;
	int		m_direct;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	afx_msg void OnRadioFast();
	afx_msg void OnRadioMiddle();
	afx_msg void OnRadioSlow();
	afx_msg void OnRadioAutoSize();
	afx_msg void OnRadioNoAutoSize();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio9();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__AF3D44B9_50E1_4449_B7F7_985A216A8B9E__INCLUDED_)
