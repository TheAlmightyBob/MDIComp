#include "stdafx.h"
#include "CompositionSurface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#include "D2DHelpers.h"
#endif

CDeviceResources CCompositionRoot::m_devResources;

HRESULT CCompositionRoot::Initialize(HWND hWndTarget)
{
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
    {
        hr = GetDevice()->CreateTargetForHwnd(hWndTarget, TRUE, &m_pHwndRenderTarget);
    }

    // Create/set root visual

    if (SUCCEEDED(hr))
    {
        hr = GetDevice()->CreateVisual(&m_pRootVisual);
    }

    if (SUCCEEDED(hr))
    {
        m_pRootVisual->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
        m_pRootVisual->SetBorderMode(DCOMPOSITION_BORDER_MODE_SOFT);
        
        hr = m_pHwndRenderTarget->SetRoot(m_pRootVisual);
    }

    m_hwndTarget = hWndTarget;

    return hr;
}

/*static*/
bool CCompositionRoot::AreDeviceResourcesValid()
{
    return m_devResources.HasValidDevices();
}

void CCompositionRoot::RecreateDeviceResources()
{
    m_pRootVisual.Release();
    m_pHwndRenderTarget.Release();
    m_devResources.RecreateDeviceResources();
    Initialize(m_hwndTarget);
}