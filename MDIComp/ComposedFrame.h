/*
This manages a single child window in expose-mode (scoped to a single invocation).
Assists with enabling child window layering, creating/animating DirectComposition
visuals, selection, etc.
*/
#pragma once

#include "DCompHelpers.h"
#include "Highlight.h"

class CComposedFrame
{
public:
    CComposedFrame(CWnd* pFrame);
    ~CComposedFrame();

    const CRect& GetOriginalRect() const;
    const CPoint& GetOriginalCenter() const;
    double GetAspectRatio() const;

    bool IsAssigned() const;
    bool IsHighlighted() const;
    const CRect& GetDCRegion() const;

    void SetDCRegion(const CRect& rect); 

    HRESULT Cloak(BOOL bCloak = TRUE) const;

    void Compose(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer,
		BOOL bInsertAbove = TRUE, IDCompositionVisual* pReferenceVisual = nullptr);

    void AnimateToRegion(IDCompositionDevice* pDevice, const CRect& rcBounds, double dDuration);
    void AnimateBack(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer, double dDuration);

    BOOL HitTest(const CPoint& pt) const;

    void Activate(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer);

    HRESULT ShowHighlight(IDCompositionVisual* pContainer, bool bShow = true);

protected:
    // Animates position & scale
    // rcTo is a bounding rect. rcFrom will be scaled to fit and centered in it
    HRESULT AnimateToRect(IDCompositionDevice* pDevice, const CRect& rcFrom, const CRect& rcTo,
        double dDuration);

	void Recompose(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer);

    static HRESULT CreateHighlight(IDCompositionDevice* pDevice);
    static HRESULT DrawHighlight(IDCompositionDevice* pDevice);

    HRESULT PlaceHighlight(IDCompositionDevice* pDevice);

protected:
    CWnd* m_pFrame;
    CRect m_rcOriginal;
    CRect m_rcNormalPosition;
    CPoint m_ptOriginalCenter;
    double m_dAspectRatio;
    bool m_bMaximized;

    CRect m_rcDCRegion;

    CRect m_rcCurrent;

	CRect m_rcBounds; // parent window rect, which clips our window rect

    CRect m_rcOriginalClipped;

    // Animation destination values, kept for animating back from
    POINTF m_ptTopLeftUnscaled;
    float m_fScaleFactor; // not certain if we need this... or should keep the clipped rect instead...

    CComPtr<IDCompositionVisual> m_pVisual;
    CComPtr<IUnknown> m_pSurface;

    CHighlight m_highlight;
    bool m_bHighlighted;
};
