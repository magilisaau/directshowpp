#pragma once

#include "resource.h"
// CDlgPass �Ի���

class CDlgPass : public CDialog
{
	DECLARE_DYNAMIC(CDlgPass)

public:
	CDlgPass(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPass();

// �Ի�������
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
