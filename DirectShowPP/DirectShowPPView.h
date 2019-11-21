// DirectShowPPView.h : interface of the CDirectShowPPView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSHOWVIEW_H__BFE15E9F_27DA_40EC_9D90_DE8B080CAC9D__INCLUDED_)
#define AFX_DIRECTSHOWVIEW_H__BFE15E9F_27DA_40EC_9D90_DE8B080CAC9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDirectShowPPView : public CView
{
protected: // create from serialization only
	CDirectShowPPView();
	DECLARE_DYNCREATE(CDirectShowPPView)

// Attributes
public:
	CDirectShowPPDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectShowPPView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirectShowPPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirectShowPPView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DirectShowPPView.cpp
inline CDirectShowPPDoc* CDirectShowPPView::GetDocument()
   { return (CDirectShowPPDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTSHOWVIEW_H__BFE15E9F_27DA_40EC_9D90_DE8B080CAC9D__INCLUDED_)
