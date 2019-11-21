// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_delayTime = 0;
	m_radioSpeed = -1;
	m_radioAutosize = -1;
	m_direct = -1;
	//}}AFX_DATA_INIT
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Text(pDX, IDC_EDIT1, m_delayTime);
	DDX_Radio(pDX, IDC_RADIO1, m_radioSpeed);
	DDX_Radio(pDX, IDC_RADIO4, m_radioAutosize);
	DDX_Radio(pDX, IDC_RADIO6, m_direct);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioFast)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioMiddle)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioSlow)
	ON_BN_CLICKED(IDC_RADIO4, OnRadioAutoSize)
	ON_BN_CLICKED(IDC_RADIO5, OnRadioNoAutoSize)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO9, &CConfigDlg::OnBnClickedRadio9)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

void CConfigDlg::OnRadioFast() 
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::OnRadioMiddle() 
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::OnRadioSlow() 
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::OnRadioAutoSize() 
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::OnRadioNoAutoSize() 
{
	// TODO: Add your control notification handler code here
}
 
void CConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}

BOOL CConfigDlg::OnInitDialog() 
{	
	POINT pt;
	tagRECT rtthis;
	CDialog::OnInitDialog();
	GetCursorPos(&pt);			
	GetWindowRect(&rtthis);
    int wi,hi;
	wi =rtthis.right-rtthis.left;
	hi =rtthis.bottom-rtthis.top;
	rtthis.left =pt.x;
	rtthis.top =pt.y;
	rtthis.right =pt.x+wi;
	rtthis.bottom =pt.y+hi;	
	this->MoveWindow(&rtthis);
	return TRUE;  // return TRUE unless you set the focus to a control
	
}

void CConfigDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CConfigDlg::OnBnClickedRadio9()
{
	// TODO: 在此添加控件通知处理程序代码
}
