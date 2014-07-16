
// ChildFrm.h : interface of the CChildFrame class
//

#pragma once

//#include <dcomp.h>
//#include "DCompHelpers.h"

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:

// Generated message map functions
protected:
	int OnCreate(LPCREATESTRUCT);
	
	DECLARE_MESSAGE_MAP()
};
