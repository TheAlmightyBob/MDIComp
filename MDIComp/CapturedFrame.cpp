#include "stdafx.h"
#include "CapturedFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCapturedFrame::~CCapturedFrame()
{
    m_pVisual.Release();
    m_pSurface.Release();
}

HRESULT CCapturedFrame::Initialize(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer, 
                                   CWnd* pSource)
{
    ASSERT(pDevice && pContainer && pSource);

    HRESULT hr = !(pDevice && pContainer && pSource) ? E_INVALIDARG : S_OK;

    HDC hSurfaceDC = NULL;

    CComPtr<IDXGISurface1> pDXGISurface;
    POINT pointOffset = {};

    CRect rcClient;
    pSource->GetClientRect(&rcClient);

    if (SUCCEEDED(hr))
    {
        // Create a DirectComposition-compatible surface that is the same size 
        // as the window.
        hr = pDevice->CreateSurface(rcClient.Width(), rcClient.Height(),
                                      DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_IGNORE, &m_pSurface);
    }

    ASSERT(SUCCEEDED(hr));

    if (SUCCEEDED(hr))
    {
        hr = m_pSurface->BeginDraw(NULL, __uuidof(IDXGISurface1),
                                   reinterpret_cast<void**>(&pDXGISurface), &pointOffset);
    }

    ASSERT(SUCCEEDED(hr));

    if (SUCCEEDED(hr))
    {
        pDXGISurface->GetDC(FALSE, &hSurfaceDC);

        hr = hSurfaceDC ? S_OK : E_FAIL;
    }

    ASSERT(SUCCEEDED(hr));

    if (SUCCEEDED(hr))
    {
        CDC* pDC = pSource->GetDC();
        ASSERT(pDC);

        BitBlt(hSurfaceDC, pointOffset.x, pointOffset.y,
               rcClient.Width(), rcClient.Height(), pDC->GetSafeHdc(), 0, 0, SRCCOPY);

        pDXGISurface->ReleaseDC(NULL);
    }

    if (m_pSurface)
    {
        m_pSurface->EndDraw();
    }

    if (SUCCEEDED(hr))
    {
        hr = DCompHelpers::CreateVisualForContent(pDevice, m_pSurface, &m_pVisual);
    }

    ASSERT(SUCCEEDED(hr));

    if (SUCCEEDED(hr))
    {
        hr = pContainer->AddVisual(m_pVisual, FALSE, nullptr);
    }

    return hr;
}