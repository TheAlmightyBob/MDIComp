#pragma once

#include <dcomp.h>


namespace DCompHelpers
{
    HRESULT CreateDCompDevice(IDCompositionDevice** ppDevice, IUnknown* pRenderingDevice = nullptr);

    HRESULT CreateVisualForContent(IDCompositionDevice* pDevice, IUnknown* pContent, IDCompositionVisual** ppVisual);

    HRESULT CloakWindow(HWND hWnd, BOOL bCloak = TRUE);

    HRESULT OffsetVisual(IDCompositionVisual* pVisual, const CPoint& ptOffset);

    HRESULT OffsetVisual(IDCompositionVisual* pVisual, POINTF ptOffset);

    HRESULT AnimateLinear(IDCompositionAnimation* pAnimation, float from, float to, double duration);

    HRESULT AnimateLinear(IDCompositionAnimation* pAnimationX, IDCompositionAnimation* pAnimationY,
        POINTF ptFrom, POINTF ptTo, double duration);

    // These are all linear animations...

    HRESULT AnimatePosition(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
        const POINTF& ptFrom, const POINTF& ptTo, double dDuration);

    HRESULT AnimateScale(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
        float fFrom, float fTo, double dDuration, const CPoint& ptCenter);

    // No animation..

    HRESULT SetScale(IDCompositionDevice* pDevice, IDCompositionVisual* pVisual,
        float fX, float fY, const CPoint& ptCenter);
}