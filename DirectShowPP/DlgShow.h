#if !defined(AFX_ShowDlg2_H__34966777_DF06_4934_BA35_F0426BECBB4E__INCLUDED_)
#define AFX_ShowDlg2_H__34966777_DF06_4934_BA35_F0426BECBB4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowDlg2.h : header file
//
#include "afxtempl.h"
//#include "MusicPlay.h"
#include "BasicPlayer.h"
#include "EFile.h"
#include "lrc.h"
#include "lrcBuf.h"
#include "ComDlg.h"
#include "afxtempl.h"
#include "MyListBox.h"
#include <direct.h>
#include <ddraw.h>
/////////////////////////////////////////////////////////////////////////////
// CDlgShow dialog

#define  REV_CLIENT_TOP  20
#define  REV_CLIENT_BOTTOM 40 
struct	_button
{
	UINT	uID;	//command ID
	int     x;
	int     y;
	int		cx;		//width
	int		cy;		//height
	HIMAGELIST	himl;	//image list
	UINT	uStatus;	//status (image index)
	char	szHint[80];	//tooltip text
};
enum
{
	//Button status /image index
	CAPTION_BTN_NORMAL		= 0,
	CAPTION_BTN_PUSHED		= 2,
	CAPTION_BTN_HOVER		= 1,
	CAPTION_BTN_CHECKED		= 3,
	CAPTION_BTN_DISABLED	= 4,		
	//Interval between buttons
	CAPTION_BTN_INTERVAL	= 0
};
class CDlgShow : public CDialog
{
// Construction
public:
	void InsertMusic(CString str);
	void clearScreen();
	void ShowCfgInfo();
	LRESULT OnClickTray(WPARAM wparam,LPARAM lparam);
	void DelTray();
	void HideInTray();
	void ShowLRC();
	void DrawSlider();
	void OnSetTop();
	void OnButtonShow();
	int GetButtonState(int index);
	void OnButtonStop();
	bool IsButtonChecked(int index);
	void ShowButtons();
	void OnPushButton();
	void ChangeButtonState(int index,int state);
	void UpdateButtonStatus();
	int AddButton(UINT uID, int x,int y,int cx, int cy, HIMAGELIST himl,char * pHint);
	void PlayMusic();
	void OnSetMusic();
	void OnAbout();
	void OnRestore();
	void OnForward();
	void OnBack();
	void OnExitTray();
	void AdjustListPos();
	LRESULT OnNotifyPlay(WPARAM wparam,LPARAM lparam);
	void OnSetLrcPath();
	void OnAssociateLrc();
	void OnLinkFile();
	void OnUnLinkFile();
	void ReShow();
	void LoadLrcArray();
	void FillRevClient(int flag);
	void DrawText(CDC* pDC,CString strTxt,UINT  posx,UINT posy,COLORREF color);
	LRESULT OnEndPlay(WPARAM wParam,LPARAM lParam);
	void MySleep(UINT interval);
	void DeleteTmpFile();
	void OnMakeFile(int type);
	void SaveSldFile(char *lpstrFile);
	void OnOK();
	void OnSetPath();
	void SlideShow();
	void OnSaveSlide();
	bool LoadSldFile(char *lpstrFile);
	void OnOpenSlide();
	void OnMakeScr();
	void CountFile();
	unsigned int m_count;
	void OnMakeExe();
	void OnSetting();
	CRect m_rtButtExit;	//关闭按钮位置
	CRect m_rtButtMax;	//最大化按钮位置
	CRect m_rtButtMin;	//最小化按钮位置
	CRect m_rtButtHelp;	//帮助按钮位置
	CRect m_rtIcon;		//图标位置
	BOOL  m_ReSizeFlag;
	int	  m_OldWidth;
	int   m_OldHeight;
	BOOL  m_ShowTitle;
	int   m_forwardflag;
	void DrawTitleBar(CDC *pDC);
	void Warning();
	void OnExit();
	BOOL LoadBMP(CString fileName);
	BOOL LoadJPG(CString fileName);
	afx_msg void OnClose();	
	HRESULT ShowPic();
	CDlgShow(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgShow)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgShow)
	virtual void OnCancel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcPaint();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void OnOpenMusic(char* song);
	BOOL LoadPicFile(CString filename);
	void ModifyLrc();
	CComDlg *m_pListWindow;
private:
// 	int m_LastDbClkFlag;
	BYTE *m_buf;
	UINT m_width;
	UINT m_height;
	UINT m_widthDW;
	UINT window_width,window_height;
	UINT m_pause;
	UINT m_time;
	CString m_curPic,m_nextPic;
	CString m_curLycris;
	CString m_nextLycris;
	UINT m_iPos;
	int m_playFlag;
	CLrc m_lrc;
	CLrcBuf m_lrcBuf;
	int m_bListShow;
	int m_ButtonPosx, m_ButtonPosy;
	CArray<_button*,_button*> m_buttons;
	int m_Pressed;
	int m_bTop;
	int m_flagDestroy;
	int m_slideTop;
	double m_slideRatio;
	int m_abc;
private:
	void UpdateForward();
	BOOL InitDDraw();
	void Flip();	
	LPDIRECTDRAW            lpDD;       //DirectDraw对象
	LPDIRECTDRAWCLIPPER		lpClipper;	//DirectDrawClipper对象
	LPDIRECTDRAWSURFACE     lpDDSFront; //DirectDrawSurface对象
	LPDIRECTDRAWSURFACE lpDDSBack;
	LPDIRECTDRAWSURFACE lpDDPic;
	LPDIRECTDRAWSURFACE lpDDClear;	
	int bytes_per_pixel,bytes_per_line;
	void* lpBackSurface;
public:
	int ShowLrc2(void);
	int updateSpectrum(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ShowDlg2_H__34966777_DF06_4934_BA35_F0426BECBB4E__INCLUDED_)
