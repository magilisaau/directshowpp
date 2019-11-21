// LrcSearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "LrcSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLrcSearchDlg dialog


CLrcSearchDlg::CLrcSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLrcSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLrcSearchDlg)
	m_path = _T("");
	//}}AFX_DATA_INIT
}


void CLrcSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLrcSearchDlg)
	DDX_Text(pDX, IDC_EDIT1, m_path);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLrcSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CLrcSearchDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnSetSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLrcSearchDlg message handlers

void CLrcSearchDlg::OnSetSearch() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	char strpath[MAX_PATH];
	strpath[0]=0;
	BROWSEINFO bi;
	ITEMIDLIST * pidl;
	bi.hwndOwner =GetSafeHwnd();
	bi.iImage =0;
	bi.lParam =0;
	bi.lpfn =0;
	bi.lpszTitle ="设置歌词文件搜索目录";
	bi.pidlRoot =0;
	bi.pszDisplayName =0;
	bi.ulFlags =BIF_RETURNONLYFSDIRS;
	UpdateData(TRUE);
	pidl=SHBrowseForFolder(&bi);
	if(SHGetPathFromIDList(pidl,strpath)==TRUE)	
	{
		m_path =strpath;
		UpdateData(FALSE);
	}		
}
