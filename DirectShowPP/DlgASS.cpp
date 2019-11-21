// DlgASS.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "DlgASS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgASS dialog


CDlgASS::CDlgASS(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgASS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgASS)
	m_lrc = _T("");
	//}}AFX_DATA_INIT
}


void CDlgASS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgASS)
	DDX_Text(pDX, IDC_EDIT1, m_lrc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgASS, CDialog)
	//{{AFX_MSG_MAP(CDlgASS)
	ON_BN_CLICKED(IDC_BUTTON1, OnSelect)
	//}}AFX_MSG_MAP 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgASS message handlers

void CDlgASS::OnSelect() 
{
	CString strTitle;
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[MAX_PATH];               //   buffer   for   file   name   
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   this->GetSafeHwnd();   
	ofn.lpstrFile   =   szFile;   
	ofn.lpstrFile[0]   =   '\0';   
	ofn.nMaxFile   =   sizeof(szFile);   
	ofn.lpstrFilter   =   "¸è´ÊÎÄ¼þ(*.lrc)\0*.lrc\0";   
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   
	//   Display   the   Save   dialog   box.         
	GetOpenFileName(&ofn);     
	//   ofn.lpstrFile   Store   the   Filename.   
	if(strlen(ofn.lpstrFile))
	{
		m_lrc =ofn.lpstrFile;
		UpdateData(FALSE);
	}	
}
