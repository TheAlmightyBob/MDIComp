#include "stdafx.h"

#include "DCompHelpers.h"
#include "MathHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace DCompHelpers
{

HRESULT CreateDCompDevice(IDCompositionDevice** ppDevice, IUnknown* pRenderingDevice /*=nullptr*/)
{
    HRESULT hr = ((ppDevice == nullptr) || (*ppDevice != nullptr)) ? E_UNEXPECTED : S_OK;

    if (SUCCEEDED(hr))
    {
        hr = ::DCompositionCreateDevice2(pRenderingDevice, __uuidof(IDCompositionDevice), reinterpret_cast<void **>(ppDevice));
    }

    return hr;
}

HRESULT CreateVisualForContent(IDCompositionDevice* pDevice, IUnknown* pContent, IDCompositionVisual** ppVisual)
{
    ASSERT(pDevice && pContent && ppVisual && !(*ppVisual));

    HRESULT hr = !(pDevice && pContent && ppVisual && !(*ppVisual)) ? E_UNEXPECTED : S_OK;

    if (SUCCEEDED(hr))
    {
        hr = pDevice->CreateVisual(ppVisual);
    }

    if (SUCCEEDED(hr))
    {
        hr = (*ppVisual)->SetContent(pContent);
    }

    return hr;
}

HRESULT CloakWindow(HWND hWnd, BOOL bCloak)
{
    return DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &bCloak, sizeof(bCloak));
}

HRESULT OffsetVisual(IDCompositionVisual* pVisual, const CPoint& ptOffset)
{
    ASSERT(pVisual);

    HRESULT hr = !pVisual ? E_UNEXPECTED : S_OK;

    if (SUCCEEDED(hr))
    {
        POINTF ptfOffset = { static_cast<float>(ptOffset.x), static_cast<float>(ptOffset.y) };

        hr = OffsetVisual(pVisual, ptfOffset);
    }

    return hr;
}

HRESULT OffsetVisual(IDCompositionVisual* pVisual, POINTF ptOffset)
{
    ASSERT(pVisual);

    HRESULT hr = !pVisual ? E_UNEXPECTED : S_OK;

    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetOffsetX(ptOffset.x);
    }

    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetOffsetY(ptOffset.y);
    }

    return hr;
}

HRESULT AnimateLinear(IDCompositionAnimation* pAnimation, float from, float to, double duration)
{
    HRESULT hr = S_OK;

    if (!pAnimation)
    {
        ASSERT(false);
        return E_FAIL;
    }

    hr = pAnimation->AddCubic(0.0f, from, (to - from) / (float) duration, 0.0f, 0.0f);

    if (SUCCEEDED(hr))
    {
        hr = pAnimation->End(duration, to);
    }

    return hr;
}

HRESULT AnimateLinear(IDCompositionAnimation* pAnimationX, IDCompositionAnimation* pAnimationY,
                      POINTF ptFrom, POINTF ptTo, double duration)
{
    HRESULT hr = S_OK;

    hr = AnimateLinear(pAnimationX, ptFrom.x, ptTo.x, duration);

    if (SUCCEEDED(hr))
    {
        hr = AnimateLinear(pAnimationY, ptFrom.y, ptTo.y, duration);
    }

    return hr;
}

HRESULT AnimatePosition(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
                        const POINTF& ptFrom, const POINTF& ptTo, double dDuration)
{
    HRESULT hr = S_OK;
    CComPtr<IDCompositionAnimation> pAnimateX;
    CComPtr<IDCompositionAnimation> pAnimateY;

    if (!(pDevice && pVisual))
    {
        ASSERT(false);
        return E_INVALIDARG;
    }

    // Create the animation objects.
    hr = pDevice->CreateAnimation(&pAnimateX);
    if (SUCCEEDED(hr))
    {
        hr = pDevice->CreateAnimation(&pAnimateY);
    }

    // Set up the animation
    if (SUCCEEDED(hr))
    {
        hr = DCompHelpers::AnimateLinear(pAnimateX, pAnimateY, ptFrom, ptTo, dDuration);
    }

    // Associate the animation objects with the offset properties of 
    // the visual.
    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetOffsetX(pAnimateX);
    }

    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetOffsetY(pAnimateY);
    }

    return hr;
}

HRESULT AnimateScale(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
                     float fFrom, float fTo, double dDuration, const CPoint& ptCenter)
{
    HRESULT hr = S_OK;
    CComPtr<IDCompositionAnimation> pAnimateScale;
    CComPtr<IDCompositionScaleTransform> pScale;

    if (!(pDevice && pVisual))
    {
        ASSERT(false);
        return E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        hr = pDevice->CreateAnimation(&pAnimateScale);
    }

    if (SUCCEEDED(hr))
    {
        // Create the scale transform object.
        hr = pDevice->CreateScaleTransform(&pScale);
    }

    if (SUCCEEDED(hr))
    {
        DCompHelpers::AnimateLinear(pAnimateScale, fFrom, fTo, dDuration);

        hr = pScale->SetScaleX(pAnimateScale);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetScaleY(pAnimateScale);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetCenterX((float) ptCenter.x);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetCenterY((float) ptCenter.y);
    }

    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetTransform(pScale);
    }

    return hr;
}

HRESULT SetScale(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
    float fX, float fY, const CPoint& ptCenter)
{
    HRESULT hr = S_OK;
    CComPtr<IDCompositionScaleTransform> pScale;

    if (!(pDevice && pVisual))
    {
        ASSERT(false);
        return E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        // Create the scale transform object.
        hr = pDevice->CreateScaleTransform(&pScale);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetScaleX(fX);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetScaleY(fY);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetCenterX((float) ptCenter.x);
    }

    if (SUCCEEDED(hr))
    {
        hr = pScale->SetCenterY((float) ptCenter.y);
    }

    if (SUCCEEDED(hr))
    {
        hr = pVisual->SetTransform(pScale);
    }

    return hr;
}
}