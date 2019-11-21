// ProBar.cpp : implementation file
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "ProBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProBar dialog


CProBar::CProBar(CWnd* pParent /*=NULL*/)
	: CDialog(CProBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProBar)
	//}}AFX_DATA_INIT
}


void CProBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProBar)
	DDX_Control(pDX, IDC_PROGRESS1, m_probar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProBar, CDialog)
	//{{AFX_MSG_MAP(CProBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProBar message handlers




BOOL CProBar::OnInitDialog() 
{
	POINT pt;
	tagRECT rtthis;
	CDialog::OnInitDialog();
	GetCursorPos(&pt);			
	GetWindowRect(&rtthis);
    int wi,hi;
	wi =rtthis.right-rtthis.left;
	hi =rtthis.bottom-rtthis.top;
	rtthis.left =pt.x-wi/2;
	rtthis.top =pt.y;
	rtthis.right =rtthis.left+wi;
	rtthis.bottom =rtthis.top+hi;	
	this->MoveWindow(&rtthis);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
