// DlgPass.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgPass.h"


// CDlgPass 对话框

IMPLEMENT_DYNAMIC(CDlgPass, CDialog)

CDlgPass::CDlgPass(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPass::IDD, pParent)
{

}

CDlgPass::~CDlgPass()
{
}

void CDlgPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPass, CDialog)
END_MESSAGE_MAP()


// CDlgPass 消息处理程序
