#include "stdafx.h"
#include "Highlight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float fHIGHLIGHT_SIZE = 1.0;

// statics
CComPtr<IDCompositionSurface> CHighlight::m_pHighlightSurface;


CHighlight::CHighlight()
: m_bHighlighted(false)
{
}

CHighlight::~CHighlight()
{
    m_pHighlightVisual.Release();
}

bool CHighlight::IsHighlighted() const
{
    return m_bHighlighted;
}

IDCompositionVisual* CHighlight::GetVisual() const
{
    return m_pHighlightVisual;
}

HRESULT CHighlight::Initialize(IDCompositionDevice* pDevice)
{
    ASSERT(pDevice);

    HRESULT hr = pDevice ? S_OK : E_INVALIDARG;

    // Initialize highlight visual, if necessary
    // (not currently bothering with recomposing since highlight visuals are
    //  not expected to change or overlap)
    //
    if (SUCCEEDED(hr) && !m_pHighlightVisual)
    {
        if (!m_pHighlightSurface)
        {
            hr = CreateHighlight(pDevice);
        }

        if (SUCCEEDED(hr))
        {
            hr = DCompHelpers::CreateVisualForContent(pDevice, m_pHighlightSurface, &m_pHighlightVisual);
        }
    }

    return hr;
}

//static
void CHighlight::ReleaseStaticDeviceResources()
{
    m_pHighlightSurface.Release();
}

//static
HRESULT CHighlight::CreateHighlight(IDCompositionDevice* pDevice)
{
    ASSERT(pDevice);

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    if (m_pHighlightSurface)
    {
        return S_OK;
    }

    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
    {
        hr = pDevice->CreateSurface(static_cast<UINT>(fHIGHLIGHT_SIZE), static_cast<UINT>(fHIGHLIGHT_SIZE),
                                    DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ALPHA_MODE_IGNORE, &m_pHighlightSurface);
    }

    if (SUCCEEDED(hr))
    {
        hr = DrawHighlight();
    }

    return hr;
}

// static
HRESULT CHighlight::DrawHighlight()
{
    ASSERT(m_pHighlightSurface);

    if (!(m_pHighlightSurface))
    {
        return E_INVALIDARG;
    }

    CComPtr<ID2D1DeviceContext> d2dDeviceContext;
    POINT updateOffset = { 0 };
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
    {
        hr = m_pHighlightSurface->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void **>(&d2dDeviceContext), &updateOffset);
    }

    if (SUCCEEDED(hr))
    {
        d2dDeviceContext->Clear();

        // The brush can be stored and used across device contexts, it doesn't
        // have to be re-created on every draw. However, we only draw once.
        //
        CComPtr<ID2D1SolidColorBrush> d2dBackgroundBrush;

        if (SUCCEEDED(hr))
        {
            hr = d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &d2dBackgroundBrush);
        }

        if (SUCCEEDED(hr))
        {
            D2D1_RECT_F dcompTileSurfaceRect = D2D1::RectF(
                updateOffset.x + 0.0f,
                updateOffset.y + 0.0f,
                updateOffset.x + fHIGHLIGHT_SIZE,
                updateOffset.y + fHIGHLIGHT_SIZE
                );

            d2dDeviceContext->FillRectangle(
                dcompTileSurfaceRect,
                d2dBackgroundBrush);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pHighlightSurface->EndDraw();
    }

    return hr;
}

HRESULT CHighlight::PlaceHighlight(IDCompositionDevice* pDevice, const CRect& rcDest)
{
    ASSERT(pDevice);

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = DCompHelpers::OffsetVisual(m_pHighlightVisual, rcDest.TopLeft());

    if (SUCCEEDED(hr))
    {
        float fScaleX = (rcDest.Width()) / fHIGHLIGHT_SIZE;
        float fScaleY = (rcDest.Height()) / fHIGHLIGHT_SIZE;

        hr = DCompHelpers::SetScale(pDevice, m_pHighlightVisual, fScaleX, fScaleY, { 0, 0 });
    }

    return hr;
}
