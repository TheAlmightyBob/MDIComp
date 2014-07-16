
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "SampleApp.h"

#include "ChildFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_CREATE()
	//ON_WM_PAINT()
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
	//SafeRelease(&m_pVisual);
	//SafeRelease(&m_pSurface);
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	//cs.dwExStyle |= WS_EX_LAYERED;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;


	return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

    //SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	return 0;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

// CChildFrame message handlers
