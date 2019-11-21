// DirectShowPPView.cpp : implementation of the CDirectShowPPView class
//

#include "stdafx.h"
#include "DirectShowPP.h"

#include "DirectShowPPDoc.h"
#include "DirectShowPPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPView

IMPLEMENT_DYNCREATE(CDirectShowPPView, CView)

BEGIN_MESSAGE_MAP(CDirectShowPPView, CView)
	//{{AFX_MSG_MAP(CDirectShowPPView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPView construction/destruction

CDirectShowPPView::CDirectShowPPView()
{
	// TODO: add construction code here

}

CDirectShowPPView::~CDirectShowPPView()
{
}

BOOL CDirectShowPPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPView drawing

void CDirectShowPPView::OnDraw(CDC* pDC)
{
	CDirectShowPPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPView diagnostics

#ifdef _DEBUG
void CDirectShowPPView::AssertValid() const
{
	CView::AssertValid();
}

void CDirectShowPPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDirectShowPPDoc* CDirectShowPPView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirectShowPPDoc)));
	return (CDirectShowPPDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPView message handlers
