// DirectShowPPDoc.h : interface of the CDirectShowPPDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSHOWDOC_H__11D47B32_299E_4BA2_B4A6_39600DC0396B__INCLUDED_)
#define AFX_DIRECTSHOWDOC_H__11D47B32_299E_4BA2_B4A6_39600DC0396B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDirectShowPPDoc : public CDocument
{
protected: // create from serialization only
	CDirectShowPPDoc();
	DECLARE_DYNCREATE(CDirectShowPPDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectShowPPDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirectShowPPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirectShowPPDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTSHOWDOC_H__11D47B32_299E_4BA2_B4A6_39600DC0396B__INCLUDED_)
