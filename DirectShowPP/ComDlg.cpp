// ComDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "DlgShow.h"
#include "ComDlg.h"
#include "MyResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComDlg dialog
CString gstrSong;

CComDlg::CComDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComDlg::IDD, pParent)
{
//	m_LastDbClkFlag=0;
	//{{AFX_DATA_INIT(CComDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_parentWnd =pParent;
  m_list.SetParent(this);
}


void CComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComDlg)
//	DDX_Control(pDX, IDC_REMOVE, m_remove);
//	DDX_Control(pDX, IDC_ADD, m_add);
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComDlg, CDialog)
	//{{AFX_MSG_MAP(CComDlg)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCCALCSIZE()
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_RBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComDlg message handlers

void CComDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CRect rtWnd;
	GetWindowRect(&rtWnd);
	//mouse坐标转化为本窗口坐标 重要
	point.x = point.x - rtWnd.left;
	point.y = point.y - rtWnd.top;
	{ 
		if (m_rtButtHelp.PtInRect(point))
		{
			SendMessage(WM_HELP);
		}
		else 
		{
			if (m_rtButtExit.PtInRect(point))
			{
				::SendMessage(m_parentWnd->GetSafeHwnd(),WM_COMMAND,WM_MYMESSAGE_SHOW_CLICK,0);				
			}

		}
	}
	CDialog::OnNcLButtonDown(nHitTest,point);	
	
}

LRESULT CComDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT lrst=CDialog::DefWindowProc(message, wParam, lParam);
	
	if (!::IsWindow(m_hWnd))
		return lrst;
	
	if (/*message==WM_PAINT||message==WM_NCPAINT||*/message==WM_NCACTIVATE /*||message == WM_NOTIFY || message ==WM_SIZING*/)
	{
		CDC* pWinDC = GetWindowDC();
		if (pWinDC)
		{
			DrawTitleBar(pWinDC);
			ReleaseDC(pWinDC);
		}
	}

	return lrst;
}

void CComDlg::DrawTitleBar(CDC *pDC)
{
	if (m_hWnd)
	{
		int xFramesWidth,yFrameWidth;
		CBrush Brush(RGB(0,100,255));
		CBrush* pOldBrush = pDC->SelectObject(&Brush);
		CRect rtWnd, rtTitle, rtButtons;
		GetWindowRect(&rtWnd); 
		//取得标题栏的位置
		xFramesWidth=rtTitle.left = GetSystemMetrics(SM_CXFRAME);
		yFrameWidth=rtTitle.top = GetSystemMetrics(SM_CYFRAME);
		rtTitle.right = rtWnd.right - rtWnd.left - GetSystemMetrics(SM_CXFRAME);
		rtTitle.bottom = rtTitle.top + GetSystemMetrics(SM_CYSIZE);
		//计算最小化按钮的位置，位图大小为23*23
		rtButtons.left = rtTitle.right-23;
		rtButtons.top= rtTitle.top+2;
		rtButtons.right = rtTitle.right-4;
		rtButtons.bottom = rtTitle.top+27;
		
		m_rtButtExit = rtButtons;

		//准备画图
		CBitmap* pBitmap = new CBitmap;
		BITMAP BmpInfo;
		CBitmap* pOldBitmap;
		CDC* pDisplayMemDC=new CDC;
		pDisplayMemDC->CreateCompatibleDC(pDC);
		//重画最Caption
        int iwd =rtWnd.right -rtWnd.left;
		POINT  DrawPonit;
		DrawPonit.x =	0;
		DrawPonit.y =	0;
		pBitmap->LoadBitmap(IDB_LINE);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pBitmap->GetBitmap(&BmpInfo);
		int i=0;
		for(i=0; i< iwd; i++)
		{
		  pDC->BitBlt(DrawPonit.x+i, DrawPonit.y, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
        DrawPonit.x =DrawPonit.x +i;
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		
		//重画button
		rtButtons = m_rtButtExit;
		pBitmap->LoadBitmap(IDB_BUTTON_COMMON);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pDC->BitBlt(rtButtons.left, rtButtons.top, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();


		//准备画图
		pBitmap->LoadBitmap(IDB_011);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pBitmap->GetBitmap(&BmpInfo);
		int itop =39;
		
		for(i=4; i<rtWnd.right-rtWnd.left ; i+=2)
		{
			pDC->BitBlt(i, itop, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();	
		//重画左边框
		CBrush brush(RGB(0,0,0));
		int m_slideTop=39;
		int max_xFramesWidth=min(xFramesWidth,4);
		pBitmap->LoadBitmap(IDB_RIGHTDOWN);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		for (i= 0; i<=rtWnd.bottom-rtWnd.top-BmpInfo.bmHeight;i=i+BmpInfo.bmHeight-3)
		{
			pDC->StretchBlt(0,i,max_xFramesWidth,BmpInfo.bmHeight, pDisplayMemDC, 0, 0,BmpInfo.bmWidth,BmpInfo.bmHeight, SRCCOPY);
		}
		//
		if(xFramesWidth>4)
		{
			pDC->FillRect(CRect(max_xFramesWidth,m_slideTop+1,xFramesWidth,rtWnd.bottom-rtWnd.top),&brush);	
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		
		
		

		//重画右边框
		pBitmap->LoadBitmap(IDB_LEFTDOWN);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		for (i= 0;i<=rtWnd.bottom-rtWnd.top;i=i+BmpInfo.bmHeight-3)
		{
			pDC->BitBlt(rtWnd.right-rtWnd.left-max_xFramesWidth,  /*rtButtons.top+*/i, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
		if(xFramesWidth>4)
		{
			pDC->FillRect(CRect(rtWnd.right-rtWnd.left-xFramesWidth,m_slideTop+1,rtWnd.right-rtWnd.left-max_xFramesWidth,rtWnd.bottom-rtWnd.top),&brush);	
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		
		
		//重画底边框
		int max_yFrameWidth=min(4,yFrameWidth);
		pBitmap->LoadBitmap(IDB_DOWN1);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		GetClientRect(&rtWnd);
		GetWindowRect(&rtWnd);
		for (i= 0; i<rtWnd.right-rtWnd.left; i=i+BmpInfo.bmWidth)
		{
			pDC->StretchBlt(i,rtWnd.bottom-rtWnd.top-max_yFrameWidth, BmpInfo.bmWidth, max_yFrameWidth,pDisplayMemDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,SRCCOPY);			
		}
		if(yFrameWidth>4)
		{
			pDC->FillRect(CRect(max_xFramesWidth,rtWnd.bottom-rtWnd.top-yFrameWidth,rtWnd.right-rtWnd.left-max_xFramesWidth,rtWnd.bottom-rtWnd.top-max_yFrameWidth),&brush);				
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		brush.DeleteObject();
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();	
		ReleaseDC(pDisplayMemDC);
		delete pDisplayMemDC;
		delete pBitmap;
	}
}

void CComDlg::OnOK() 
{
 	//CDialog::OnOK();
}

void CComDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CComDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	lpncsp->rgrc[0].top +=(41-GetSystemMetrics(SM_CYSIZE)-GetSystemMetrics(SM_CYFRAME));
    lpncsp->rgrc[0].left +=0;
	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
	tagRECT rt;
	GetClientRect(&rt);
	if(rt.bottom-rt.top==0) return;
	rt.top +=8;
	rt.bottom -=0;
 	if(m_list.m_hWnd ) m_list.MoveWindow(&rt);
}

HBRUSH CComDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) { 		
	case CTLCOLOR_EDIT:	
	case CTLCOLOR_MSGBOX:
	case CTLCOLOR_DLG : 
    case CTLCOLOR_LISTBOX:
		
		{
			const COLORREF BGColor = RGB(0,0,40);			
			HBRUSH B = CreateSolidBrush(BGColor); //COLOR是你想设置的颜色 
			pDC->SetBkColor(RGB(0,0,32));
			pDC->SetTextColor(RGB(128,128,255));
			return (HBRUSH) B;

		}				
	default: ;//其他控件设置自己默认的颜色和背景刷.	
		
//		const COLORREF BGColor = RGB(0,0,0);			
//		HBRUSH B = CreateSolidBrush(BGColor); //COLOR是你想设置的颜色 
//		return (HBRUSH) B;		

//		int DlgID =pWnd->GetDlgCtrlID();
// 		if(DlgID==IDC_LIST1)
//		{
//			pDC->SetBkColor(RGB(0,0,0));
//			pDC->SetTextColor(RGB(0,0,255));
//		}	
	}
	return hbr;
}
extern int g_bStick;
void CComDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
    tagRECT rtthis,rtparent;
	GetWindowRect(&rtthis);
	m_parentWnd->GetWindowRect(&rtparent);
	if(rtparent.right-10< rtthis.left && rtparent.right+10 > rtthis.left)
	{
		rtthis.left -= (rtthis.left-rtparent.right);
		rtthis.right -=(rtthis.left-rtparent.right);
		MoveWindow(&rtthis);
		g_bStick =1;
	}
	else
	{
		g_bStick =0;
	}

}

int CComDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	tagRECT rct;
	this->GetWindowRect(&rct);
	return 0;
}

void CComDlg::OnDblclkList1() 
{
//双击
	int curSel =m_list.GetCurSel();
	if(curSel>=0)
	{
		if(CheckSong(curSel))
		{
		  gstrSong=m_songlist.GetAt(curSel);
		  gstrSong.MakeUpper();
          ::SendMessage(m_parentWnd->GetSafeHwnd(),WM_MYMESSAGE_NOTIFY_PLAY,0,0);

		}
		else
		{
			::AfxMessageBox("文件不存在");
			RemoveString(curSel);

		}
	}
	
}

void CComDlg::GetNextPlay()
{
   CString strNext;
   int inext=0;
   int totle =m_list.GetCount();
   int curSel =m_list.GetCurSel();
   if(curSel <0) 
   {
	   curSel =-1;
   }
   int i=0;
   for( i=0;i<totle;i++)
   {
	   inext=(++curSel)%totle;
	   if(CheckSong(inext))
	   {
          m_list.SetCurSel(inext);
	      gstrSong =m_songlist.GetAt(inext);
	      gstrSong.MakeUpper();
		  break;
	   }
   }

   if(i==totle)
   {
	   gstrSong.Empty();
   }
}

void CComDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnSysCommand(nID, lParam);
}

BOOL CComDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_add.DrawTransparent();
//	m_remove.DrawTransparent();	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CComDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	

}

BOOL CComDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |=WS_SYSMENU;
	return CDialog::PreCreateWindow(cs);
}

void CComDlg::ResetList()
{
    m_list.ResetContent();
	m_songlist.RemoveAll();
}

int CComDlg::InsertString(CString str)
{
	//插入一个歌曲，如果发现已经存在则删除之前的，并将最新的提升到最近，该函数返回插入的位置，这个位置可以重新set

	char filename[MAX_PATH];
	//m_songlist.Append(str);
	for(int i=0; i<m_songlist.GetCount(); i++)
	{
		if(m_songlist.GetAt(i)==str) 
		{
			return i;
		}
	}
	m_songlist.Add(str);
	_splitpath(str.GetBuffer(0),NULL,NULL,filename,NULL);
	CString strr="--- ";
	strr+=filename;
	m_list.InsertString(m_list.GetCount(),strr.GetBuffer(0));
	return m_list.GetCount()-1; //lxb 2011
	//m_list.InsertString(m_list.GetCount(),str);
}
void CComDlg::RemoveString(int index)
{
	m_songlist.RemoveAt(index);	
	m_list.DeleteString(index);
}
int CComDlg::GetCount()
{
   return m_list.GetCount();
}

CString CComDlg::GetString(int index)
{
  //CString strr;
  //m_list.GetText(index,strr);
  //return strr;
	return m_songlist.GetAt(index);
}

void CComDlg::OnDropFiles(HDROP hDropInfo) 
{
	char filename[MAX_PATH];
    int cnt =DragQueryFile(hDropInfo,-1,filename,MAX_PATH);
	for(int i=0; i<cnt; i++)
	{
		CString strTmp;		
		DragQueryFile(hDropInfo,i,filename,MAX_PATH);		
		strTmp =filename;
		strTmp.MakeUpper();
		strTmp =strTmp.Right(4);
		if( strTmp==".MP3" ||
			//strTmp==".WMA" ||
			strTmp==".WAV" 			
			)
		{
			InsertString(filename);
		}
		
	} 
	
	CDialog::OnDropFiles(hDropInfo);
}

void CComDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnRButtonDown(nFlags, point);
}

void CComDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.y   =   160;   	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CComDlg::SetSel(int index)
{
	m_list.SetCurSel(index);
   UpdateData(FALSE);
}

BOOL CComDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CComDlg::OnNcPaint() 
{
	// TODO: Add your message handler code here
	// TODO: Add your message handler code here
	CDC* pWinDC = GetWindowDC();
	if (pWinDC)
	{
	  DrawTitleBar(pWinDC);
	  ReleaseDC(pWinDC);	
	}
	// Do not call CDialog::OnNcPaint() for painting messages
}
	// Do not call CDialog::OnNcPaint() for painting messages




void CComDlg::RemoveString(CString str)
{
	int i;
	for( i=0; i<m_songlist.GetCount(); i++)
	{
		if(m_songlist.GetAt(i)==str) 
		{
			break;
		}
	}

	if(i!=m_songlist.GetCount())
	{
       RemoveString(i);
	}

}

void CComDlg::RemoveAll(void)
{
	m_songlist.RemoveAll();	
	m_list.ResetContent();
}



bool CComDlg::CheckSong(int index)
{
	    CString strr;
		strr=m_songlist.GetAt(index);
		strr.MakeUpper();
        CFileFind ff;
	    if(ff.FindFile(strr))
		{
          return true;
		}
		else
		{
          return false;
		}
		
}
