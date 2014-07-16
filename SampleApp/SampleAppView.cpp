
// SampleAppView.cpp : implementation of the CSampleAppView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SampleApp.h"
#endif

#include "SampleAppDoc.h"
#include "SampleAppView.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSampleAppView

IMPLEMENT_DYNCREATE(CSampleAppView, CView)

BEGIN_MESSAGE_MAP(CSampleAppView, CView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSampleAppView construction/destruction

CSampleAppView::CSampleAppView()
{
	// TODO: add construction code here
    m_color = RGB(rand() % 256, rand() % 256, rand() % 256);

}

CSampleAppView::~CSampleAppView()
{
}

BOOL CSampleAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//cs.dwExStyle |= WS_EX_LAYERED;
	//EnableD2DSupport();
	AfxGetD2DState();
	return CView::PreCreateWindow(cs);
}

// CSampleAppView drawing

void CSampleAppView::OnDraw(CDC* pDC)
{
	CSampleAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	//pDC->FillSolidRect(0, 0, 1000, 1000, RGB(0, 0, 200));
    pDC->FillSolidRect(0, 0, 1000, 1000, m_color);
}

void CSampleAppView::OnLButtonDown(UINT, CPoint)
{
	//AfxMessageBox(CString("hi"));
	//Invalidate();
	//CRect bob(0, 0, 1000, 1000);
	//
	//RedrawWindow(bob, nullptr, RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);

	//CChildFrame* pFrame = static_cast<CChildFrame*>(GetParentFrame());
	//if (pFrame)
	//{
	//	pFrame->Compose();
	//}
}

int CSampleAppView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	//CMDIChildWnd* pParentFrame = static_cast<CMDIChildWnd*>(GetParentFrame());

	//CMainFrame* pMainFrame = static_cast<CMainFrame*>(pParentFrame->GetMDIFrame());

	//auto pDevice = pMainFrame->GetDevice();
	//auto pContainer = pMainFrame->GetContainer();

	//if (pDevice && pContainer)
	//{
	//	HRESULT hr = pDevice->CreateVisual(&m_pVisual);

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pDevice->CreateSurfaceFromHwnd(m_hWnd, &m_pSurface);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = m_pVisual->SetContent(m_pSurface);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		//m_pVisual->

	//		hr = pContainer->AddVisual(m_pVisual, TRUE, nullptr);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pDevice->Commit();
	//	}

	//	//if (SUCCEEDED(hr))
	//	//{
	//	//	BOOL bCloak = TRUE;
	//	//	hr = DwmSetWindowAttribute(m_hWnd, DWMWA_CLOAK, &bCloak, sizeof(bCloak));
	//	//}
	//}

	return 0;
}


// CSampleAppView printing

BOOL CSampleAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSampleAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSampleAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CSampleAppView diagnostics

#ifdef _DEBUG
void CSampleAppView::AssertValid() const
{
	CView::AssertValid();
}

void CSampleAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSampleAppDoc* CSampleAppView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleAppDoc)));
	return (CSampleAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CSampleAppView message handlers
