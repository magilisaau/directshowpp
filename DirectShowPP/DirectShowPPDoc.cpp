// DirectShowPPDoc.cpp : implementation of the CDirectShowPPDoc class
//

#include "stdafx.h"
#include "DirectShowPP.h"

#include "DirectShowPPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPDoc

IMPLEMENT_DYNCREATE(CDirectShowPPDoc, CDocument)

BEGIN_MESSAGE_MAP(CDirectShowPPDoc, CDocument)
	//{{AFX_MSG_MAP(CDirectShowPPDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPDoc construction/destruction

CDirectShowPPDoc::CDirectShowPPDoc()
{
	// TODO: add one-time construction code here

}

CDirectShowPPDoc::~CDirectShowPPDoc()
{
}

BOOL CDirectShowPPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPDoc serialization

void CDirectShowPPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPDoc diagnostics

#ifdef _DEBUG
void CDirectShowPPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDirectShowPPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPDoc commands
