// MusicSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "MusicSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMusicSetDlg dialog


CMusicSetDlg::CMusicSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMusicSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMusicSetDlg)
	m_sound = _T("");
	//}}AFX_DATA_INIT
}


void CMusicSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMusicSetDlg)
	DDX_Text(pDX, IDC_EDIT_SOUND, m_sound);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMusicSetDlg, CDialog)
	//{{AFX_MSG_MAP(CMusicSetDlg)
	ON_BN_CLICKED(IDC_BUTTONSOUND, OnButtonsound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMusicSetDlg message handlers

void CMusicSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CMusicSetDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CMusicSetDlg::OnButtonsound() 
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
	ofn.lpstrFilter   =   "ÉùÒôÎÄ¼þ(*.mp3;*.wma;*.wav)\0*.mp3;*.wma;*.wav\0";   
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   
	GetOpenFileName(&ofn);     
	if(strlen(ofn.lpstrFile))
	{
      m_sound =ofn.lpstrFile;
	  UpdateData(FALSE);
	}	

}
