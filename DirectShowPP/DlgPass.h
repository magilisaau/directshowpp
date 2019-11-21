#pragma once

#include "resource.h"
// CDlgPass 对话框

class CDlgPass : public CDialog
{
	DECLARE_DYNAMIC(CDlgPass)

public:
	CDlgPass(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPass();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
