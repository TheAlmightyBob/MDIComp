/*
A simple rect visual for highlighting hovered windows.
*/
#pragma once

#include "DCompHelpers.h"

class CHighlight
{
public:
    CHighlight();
    ~CHighlight();

    HRESULT Initialize(IDCompositionDevice* pDevice);

    HRESULT PlaceHighlight(IDCompositionDevice* pDevice, const CRect& rcDest);

    bool IsHighlighted() const;

    IDCompositionVisual* GetVisual() const;

    static void ReleaseStaticDeviceResources();

protected:
    static HRESULT CreateHighlight(IDCompositionDevice* pDevice);
    static HRESULT DrawHighlight();

protected:
    // As explained at BUILD 2013, reusing a shared surface for multiple
    // visuals saves memory
    static CComPtr<IDCompositionSurface> m_pHighlightSurface;

    CComPtr<IDCompositionVisual> m_pHighlightVisual;
    bool m_bHighlighted;
};
