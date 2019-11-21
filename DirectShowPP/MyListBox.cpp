// MyListBox.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "MyListBox.h"
#include "ComDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListBox

CMyListBox::CMyListBox()
{
	m_pParent=NULL;
}

CMyListBox::~CMyListBox()
{
}


BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
	//{{AFX_MSG_MAP(CMyListBox)
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(WM_MYMESSAGE_ADD,onadd)
    ON_COMMAND(WM_MYMESSAGE_REMOVE,onremove)
	ON_COMMAND(WM_MYMESSAGE_CLEARLIST,OnClear)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListBox message handlers

void CMyListBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	POINT pt;
	GetCursorPos(&pt);		
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_ADD,"Add Music"); 
    menu.AppendMenu(MF_SEPARATOR);								
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_REMOVE,"Remove Music");
    menu.AppendMenu(MF_SEPARATOR);							
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_CLEARLIST,"Clear Playing List"); 
	
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	HMENU hmenu=menu.Detach();
	menu.DestroyMenu();	CListBox::OnRButtonDown(nFlags, point);
}
/*#define _OPENTYPE "Music Files(*.mp3;*.wma)|*.mp3;*.wma||"*/

#define _OPENTYPE "All supported files|*.mp3;|" \
					  "MP3 files(*.mp3)|*.mp3|" 
void CMyListBox::onadd()
{
	char  sbuf[4000];
	memset(sbuf,0,4000);
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR,
		_OPENTYPE);
	FileDlg.m_ofn.lpstrFile =sbuf;
	FileDlg.m_ofn.nMaxFile =4000;
	if(FileDlg.DoModal()==IDCANCEL)
		return ;

	int cnt =this->GetCount();
	POSITION pos=FileDlg.GetStartPosition();
	CString lpszFile;
	lpszFile=FileDlg.GetNextPathName(pos);
	while(1)
	{
		//InsertString(cnt,lpszFile);
		m_pParent->InsertString(lpszFile);
		if(!pos)	break;
		lpszFile=FileDlg.GetNextPathName(pos);
		cnt++;
	}	  
}

void CMyListBox::onremove()
{
	int index =GetCurSel();
	if(index>=0)
	{
	   //DeleteString(index);	
		m_pParent->RemoveString(index);
	   int cnt =GetCount();
	   if(cnt>0)
	   {
		   index =index%cnt;
		   SetCurSel(index);
	   }	   
	}
}

void CMyListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
    if(nChar==0x2e)
	{
		onremove();
/*
		int cursel =GetCurSel();
		if(cursel!=-1)
		{
			DeleteString(cursel);
			int cnt =GetCount();
			if(cnt>0)
			{
			  cursel =cursel%cnt;
			  SetCurSel(cursel);
			}
		}
*/
	}
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyListBox::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListBox::OnGetMinMaxInfo(lpMMI);
}

void CMyListBox::OnClear()
{
   m_pParent->RemoveAll();
}

void CMyListBox::SetParent(CComDlg* pWin)
{
	m_pParent=pWin;
}
