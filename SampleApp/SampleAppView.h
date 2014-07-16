
// SampleAppView.h : interface of the CSampleAppView class
//

#pragma once

//#include <dcomp.h>
//#include "DCompHelpers.h"

class CSampleAppView : public CView
{
protected: // create from serialization only
	CSampleAppView();
	DECLARE_DYNCREATE(CSampleAppView)

// Attributes
public:
	CSampleAppDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSampleAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    COLORREF m_color;

	//CComPtr<IDCompositionVisual> m_pVisual;
	//CComPtr<IUnknown> m_pSurface;

// Generated message map functions
protected:
	int OnCreate(LPCREATESTRUCT);
	void OnLButtonDown(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SampleAppView.cpp
inline CSampleAppDoc* CSampleAppView::GetDocument() const
   { return reinterpret_cast<CSampleAppDoc*>(m_pDocument); }
#endif

