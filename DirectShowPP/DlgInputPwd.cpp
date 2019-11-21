// DlgInputPwd.cpp : implementation file
//

#include "stdafx.h"
#include "directshowpp.h"
#include "DlgInputPwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputPwd dialog


CDlgInputPwd::CDlgInputPwd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputPwd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputPwd)
	m_pwd1 = _T("");
	m_pwd2 = _T("");
	//}}AFX_DATA_INIT
}


void CDlgInputPwd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputPwd)
	DDX_Text(pDX, IDC_EDIT1, m_pwd1);
	DDV_MaxChars(pDX, m_pwd1, 8);
	DDX_Text(pDX, IDC_EDIT2, m_pwd2);
	DDV_MaxChars(pDX, m_pwd2, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputPwd, CDialog)
	//{{AFX_MSG_MAP(CDlgInputPwd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputPwd message handlers

void CDlgInputPwd::GetPwd(CString &strPwd)
{
   strPwd =m_pwd1;
}

void CDlgInputPwd::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}
