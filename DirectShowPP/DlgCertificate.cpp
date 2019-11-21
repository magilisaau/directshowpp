// DlgCertificate.cpp : implementation file
//

#include "stdafx.h"
#include "directshowpp.h"
#include "DlgCertificate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCertificate dialog


CDlgCertificate::CDlgCertificate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCertificate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCertificate)
	m_pwd = _T("");
	//}}AFX_DATA_INIT
}


void CDlgCertificate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCertificate)
	DDX_Text(pDX, IDC_EDIT1, m_pwd);
	DDV_MaxChars(pDX, m_pwd, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCertificate, CDialog)
	//{{AFX_MSG_MAP(CDlgCertificate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCertificate message handlers

void CDlgCertificate::getPwd(CString &strPwd)
{
    strPwd=m_pwd;
}

void CDlgCertificate::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}
