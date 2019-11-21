// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AA6EC772_2D15_4561_ACD2_E2BDE0A96E1B__INCLUDED_)
#define AFX_MAINFRM_H__AA6EC772_2D15_4561_ACD2_E2BDE0A96E1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DlgShow.h"
#include "ConfigDlg.h"
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	LRESULT StartPlay(WPARAM wParam, LPARAM lParam);
	CDlgShow* GetShowWindow();
	LRESULT OnRevMsgExit(WPARAM wParam, LPARAM lPara);
 	void OnRun();
	LRESULT OnMsgNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnTuiChu(WPARAM wParam, LPARAM lParam);
	int InitMyMsg();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	CDlgShow* m_pShowDlg;
	int m_count;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AA6EC772_2D15_4561_ACD2_E2BDE0A96E1B__INCLUDED_)
