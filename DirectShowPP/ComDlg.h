#if !defined(AFX_COMDLG_H__2B25E3F9_AD0C_4D28_8959_99FC737AC9ED__INCLUDED_)
#define AFX_COMDLG_H__2B25E3F9_AD0C_4D28_8959_99FC737AC9ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComDlg.h : header file
//
#include "MyListBox.h"
/////////////////////////////////////////////////////////////////////////////
// CComDlg dialog
class CComDlg : public CDialog
{
// Construction
public:
	void SetSel(int index);
	CString GetString(int index);
	int GetCount();
	int InsertString(CString str);
	void ResetList();
	void GetNextPlay();
	void DrawTitleBar(CDC *pDC);
	CComDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComDlg)
	enum { IDD = IDD_COMDLG_DIALOG };

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComDlg)
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkList1();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRect m_rtButtExit;	//关闭按钮位置
	CRect m_rtButtHelp;	//帮助按钮位置
	CRect m_rtIcon;		//图标位置
	BOOL  m_ReSizeFlag;
	int	  m_OldWidth;
	int	  m_OldHeight;
	BOOL  m_ShowTitle;
	CWnd* m_parentWnd; 
	CMyListBox	m_list;	
	CArray<CString,CString> m_songlist;
public:
	void RemoveString(int index);
	void RemoveString(CString str);
	void RemoveAll(void);
	bool CheckSong(int index);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMDLG_H__2B25E3F9_AD0C_4D28_8959_99FC737AC9ED__INCLUDED_)
