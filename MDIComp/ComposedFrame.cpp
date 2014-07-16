#include "stdafx.h"
#include "ComposedFrame.h"

#include "ExposeHelpers.h"
#include "MathHelpers.h"
#include "WindowHelpers.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int nHIGHLIGHT_MARGIN = 10;


CComposedFrame::CComposedFrame(CWnd* pF)
{
    m_pFrame = pF;

    m_bMaximized = false;
    
    // Get original (un-maximized) window rect
    //
    if (m_pFrame)
    {
        WINDOWPLACEMENT placement = WINDOWPLACEMENT();

        if (m_pFrame->GetWindowPlacement(&placement))
        {
            // TODO: Handle minimized windows somehow.
            //       Maybe still compose them, but then just leave them where
            //       they are so they can still be seen and activated but not
            //       directly manipulated as live windows?
            //       - Actually, what I'd really like to do is leverage this
            //         to provide a new minimized window behavior...

            m_rcNormalPosition = placement.rcNormalPosition;

            if (placement.showCmd != SW_MAXIMIZE)
            {
                m_rcOriginal = placement.rcNormalPosition;
            }
            else
            {
                // TODO: Animation offsets are still a bit funky with maximized windows?

                m_bMaximized = true;

				// we want no frame, just get client rect
                m_pFrame->GetClientRect(&m_rcOriginal);
            }
        }
    }

    m_ptOriginalCenter = m_rcOriginal.CenterPoint();
    m_dAspectRatio = (double) m_rcOriginal.Width() / m_rcOriginal.Height();

    m_rcCurrent = m_rcOriginal;
    m_fScaleFactor = 1.0;
    m_ptTopLeftUnscaled = { static_cast<float>(m_rcCurrent.left), static_cast<float>(m_rcCurrent.right) };

    m_bHighlighted = false;
}

CComposedFrame::~CComposedFrame()
{
    m_pVisual.Release();
    m_pSurface.Release();

    Cloak(FALSE);
}

#pragma region Accessors

const CRect& CComposedFrame::GetOriginalRect() const 
{ 
    return m_rcOriginal; 
}

const CPoint& CComposedFrame::GetOriginalCenter() const 
{
    return m_ptOriginalCenter; 
}

double CComposedFrame::GetAspectRatio() const 
{
    return m_dAspectRatio; 
}

bool CComposedFrame::IsAssigned() const 
{ 
    return !m_rcDCRegion.IsRectNull(); 
}

bool CComposedFrame::IsHighlighted() const
{
    return m_bHighlighted;
}

const CRect& CComposedFrame::GetDCRegion() const 
{ 
    return m_rcDCRegion; 
}

#pragma endregion

void CComposedFrame::SetDCRegion(const CRect& rect) 
{ 
    m_rcDCRegion = rect; 
}

HRESULT CComposedFrame::Cloak(BOOL bCloak) const
{
    ASSERT(m_pFrame);

    if (!m_pFrame)
    {
        return E_FAIL;
    }

    HRESULT hr = DCompHelpers::CloakWindow(m_pFrame->m_hWnd, bCloak);

    if (SUCCEEDED(hr))
    {
        if (bCloak)
        {
            // Move window to top-left to minimize clipping
            //
            if (!m_bMaximized)
            {
                CRect rect(m_rcOriginal);
                m_pFrame->MoveWindow(0, 0, rect.Width(), rect.Height(), FALSE);
            }
            else
            {
                auto placement = WindowHelpers::GetWindowPlacement(m_pFrame);
                placement.rcNormalPosition = { 0, 0, m_rcNormalPosition.Width(), m_rcNormalPosition.Height() };
                VERIFY(m_pFrame->SetWindowPlacement(&placement));
            }
        }
        else
        {
            // Move window back to original position
            //
            if (!m_bMaximized)
            {
                m_pFrame->MoveWindow(m_rcOriginal);
            }
            else
            {
                auto placement = WindowHelpers::GetWindowPlacement(m_pFrame);
                placement.rcNormalPosition = m_rcNormalPosition;
                VERIFY(m_pFrame->SetWindowPlacement(&placement));

                // Undo layering of View, since that's less-frequently needed,
                // seems to cause trouble for Ctrl+Tab logic, and doesn't seem
                // as harmful to toggle
                // - hmm.... it's still somewhat harmful...
                // - By "causes trouble for Ctrl+Tab", I mean that CapturedFrame seems
                //   unable to capture a frame if the view of that frame is composed.
                // - By "harmful," I'm referring to the flicker that occurs at the moment
                //   of toggling composition.
                //
                ExposeHelpers::EnableComposition(m_pFrame->GetTopWindow(), false);
            }
        }
    }

    return hr;
}

void CComposedFrame::Compose(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer,
	                         BOOL bInsertAbove /*=TRUE*/, IDCompositionVisual* pReferenceVisual /*=nullptr*/)
{
    ASSERT(pDevice && pContainer && m_pFrame);

    if (pDevice && pContainer && m_pFrame)
    {
        // - On the one hand, performance/behavior is improved if we just leave the
        //   window layered rather than toggling every time.
        // - On the other hand, leaving windows layered and using SetLayeredWindowAttributes
        //   (instead of the manual UpdateLayeredWindow) requires more memory in the DWM per window.
        // - Layered windows also don't seem to play nicely with maximized Ctrl+Tab for some reason.
        //   - So we could intercept the tab switch and force a redraw... or intercept the maximize
        //     and turn off composition?
        //
        ExposeHelpers::EnableComposition(m_pFrame);

        if (m_bMaximized)
        {
            // We need to enable composition for the view since it's the thing we actually 
            // animate if maximized, but we still need it on the frame also so we can cloak it.
            //
            ExposeHelpers::EnableComposition(m_pFrame->GetTopWindow());
        }
        else
        {
            // Undo layering of View, since that's less-frequently needed,
            // seems to cause trouble for Ctrl+Tab logic, and doesn't seem
            // as harmful to toggle
            // (this is mainly for if we're getting called from Recompose)
            //
            ExposeHelpers::EnableComposition(m_pFrame->GetTopWindow(), false);
        }


        HRESULT hr = S_OK;

        if (SUCCEEDED(hr))
        {
            if (!m_bMaximized)
            {
                hr = pDevice->CreateSurfaceFromHwnd(m_pFrame->m_hWnd, &m_pSurface);
            }
            else
            {
                CWnd* pWnd = m_pFrame->GetTopWindow();
                hr = pDevice->CreateSurfaceFromHwnd(pWnd->m_hWnd, &m_pSurface);
            }
        }

        if (SUCCEEDED(hr))
        {
            hr = DCompHelpers::CreateVisualForContent(pDevice, m_pSurface, &m_pVisual);
        }

        if (SUCCEEDED(hr))
        {
            hr = pContainer->AddVisual(m_pVisual, bInsertAbove, pReferenceVisual);
        }

        if (SUCCEEDED(hr))
        {
            // Position the visual at the same location as the
            // the child window.
            hr = DCompHelpers::OffsetVisual(m_pVisual, m_rcOriginal.TopLeft());
        }

        if (SUCCEEDED(hr))
        {
            hr = m_highlight.Initialize(pDevice);
        }
    }
}

void CComposedFrame::AnimateToRegion(IDCompositionDevice* pDevice, const CRect& rcBounds, double dDuration)
{
    ASSERT(pDevice);

    if (!(pDevice))
    {
        return;
    }

    // Prepare rects
    //
    CRect rcSource(m_rcOriginal);

	m_rcBounds = rcBounds;

    // Adjust size for clipped child
    // (direct composition won't draw the clipped region of the source window,
    //  so no point pretending it's there...)
    //
    rcSource.MoveToXY(0, 0);
    rcSource.IntersectRect(rcSource, rcBounds);
    rcSource.MoveToXY(m_rcOriginal.TopLeft());

    m_rcOriginalClipped = rcSource;

    HRESULT hr = AnimateToRect(pDevice, rcSource, m_rcDCRegion, dDuration);

    if (SUCCEEDED(hr))
    {
        // Figure out what rect we just animated to and store it for later
        // TODO: Do this *first* and animate to the calculated rect
        //
        rcSource.MoveToXY(0, 0);
        rcSource.right = (long)(rcSource.right * m_fScaleFactor);
        rcSource.bottom = (long)(rcSource.bottom * m_fScaleFactor);

        rcSource.MoveToXY(m_rcDCRegion.CenterPoint().x - rcSource.Width() / 2,
            m_rcDCRegion.CenterPoint().y - rcSource.Height() / 2);
        
        m_rcCurrent = rcSource;

        // Place highlight
        //
        hr = PlaceHighlight(pDevice);
    }
}

HRESULT CComposedFrame::AnimateToRect(IDCompositionDevice* pDevice, const CRect& rcFrom,
                                      const CRect& rcTo, double dDuration)
{
    HRESULT hr = S_OK;

    if (!(pDevice && m_pVisual))
    {
        ASSERT(false);
        return E_INVALIDARG;
    }

    // Animate position

    // Calculate the X and Y offsets that will position the source window 
    // in the center of the destination window's client area.

    float endValX = rcTo.CenterPoint().x - rcFrom.Width() / 2.0f;
    float endValY = rcTo.CenterPoint().y - rcFrom.Height() / 2.0f;

    // Keep for later
    m_ptTopLeftUnscaled = { endValX, endValY };

	if (dDuration > 0)
	{
		hr = DCompHelpers::AnimatePosition(pDevice, m_pVisual,
			{ static_cast<float>(rcFrom.left), static_cast<float>(rcFrom.top) },
			m_ptTopLeftUnscaled, dDuration);
	}
    else
    {
        hr = DCompHelpers::OffsetVisual(m_pVisual, m_ptTopLeftUnscaled);
    }

    // Animate scale
    if (SUCCEEDED(hr))
    {
        float fScaleFactor = MathHelpers::AspectFit(rcFrom, rcTo);

        // Get relative center for scaling
        CRect rcFromAtZero(CPoint(0, 0), rcFrom.Size());
        CPoint ptScaleCenter = rcFromAtZero.CenterPoint();

		if (dDuration > 0)
		{
			hr = DCompHelpers::AnimateScale(pDevice, m_pVisual, 1.0f, fScaleFactor, dDuration, ptScaleCenter);
		}
        else
        {
            hr = DCompHelpers::SetScale(pDevice, m_pVisual, fScaleFactor, fScaleFactor, ptScaleCenter);
        }

        m_fScaleFactor = fScaleFactor;
    }

    return hr;
}


void CComposedFrame::AnimateBack(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer, double dDuration)
{
    ASSERT(pDevice);

    if (!(pDevice))
    {
        return;
    }

    ShowHighlight(pContainer, false);

    if (m_bMaximized)
    {
        // Check if we're still maximized. If not, adjust original rect.
        //
        WINDOWPLACEMENT placement = WINDOWPLACEMENT();

        if (m_pFrame->GetWindowPlacement(&placement))
        {
            if (placement.showCmd != SW_MAXIMIZE)
            {
                m_bMaximized = false;
                m_rcOriginal = m_rcNormalPosition;

				Recompose(pDevice, pContainer);
			}
        }
    }

    HRESULT hr = DCompHelpers::AnimatePosition(pDevice, m_pVisual,
        m_ptTopLeftUnscaled,
        { static_cast<float>(m_rcOriginal.left), static_cast<float>(m_rcOriginal.top) },
        dDuration);

    if (SUCCEEDED(hr))
    {
        // Get relative center for scaling
        CRect rcFromAtZero(CPoint(0, 0), m_rcOriginalClipped.Size());
        CPoint ptScaleCenter = rcFromAtZero.CenterPoint();

        hr = DCompHelpers::AnimateScale(pDevice, m_pVisual, m_fScaleFactor, 1.0, dDuration, ptScaleCenter);
    }
}



BOOL CComposedFrame::HitTest(const CPoint& pt) const
{
    return m_rcCurrent.PtInRect(pt);
}

void CComposedFrame::Activate(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer)
{
    m_pFrame->BringWindowToTop();

	ASSERT(pDevice && pContainer && m_pVisual);
	if (!(pDevice && pContainer && m_pVisual))
	{
		return;
	}

    ShowHighlight(pContainer, false);

    // Move DirectComp visual to the top of the Z-order within its root visual
    // (note that we use FALSE for "InsertAbove" to make sure that our visual
    //  is on top... MSDN explains the logic behind that.... )
    //

	bool bRecomposed = false;

    if (!m_bMaximized)
    {
        // Check if we've become maximized now
        WINDOWPLACEMENT placement = WINDOWPLACEMENT();

        if (m_pFrame->GetWindowPlacement(&placement))
        {
            if (placement.showCmd == SW_MAXIMIZE)
            {
                m_bMaximized = true;

				// Get the client rect since we're maximized and lost our frame...
				m_pFrame->GetClientRect(&m_rcOriginal);

				Recompose(pDevice, pContainer);
				bRecomposed = true;
            }
        }
    }

	if (!bRecomposed)
	{
		pContainer->RemoveVisual(m_pVisual);
		pContainer->AddVisual(m_pVisual, FALSE, nullptr);
	}
}

void CComposedFrame::Recompose(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer)
{
	// Decomposing/recomposing unmaximized windows is tricky since we need to put
    // them back at the correct z-order.
	// We keep the current visual, then create the new visual with the current visual
    // as reference visual (at that point it doesn't even matter if we say below or above),
    // then remove the current visual.
	// (this is actually overkill for the maximized case, but doesn't seem like it'd be too expensive..)

	// Copy
    CComPtr<IDCompositionVisual> pVisual;
    CComPtr<IUnknown> pSurface;

    pVisual.Attach(m_pVisual.Detach());
    pSurface.Attach(m_pSurface.Detach());

    ASSERT(!m_pVisual && !m_pSurface);

	if (m_bMaximized)
	{
		// Recompose on top of everything
		// (note that this will place the new visual
		//  at the maximized window's location, not at the old visual's
		//  location... )
		//
		Compose(pDevice, pContainer, FALSE);
	}
	else
	{
		// Recompose 
		// (this will place the new visual at the original window
		//  location, not at the old visual's location)
		//
		Compose(pDevice, pContainer, TRUE, pVisual);
	}

	// Initialize to where it would have been
	AnimateToRegion(pDevice, m_rcBounds, 0);

	// Remove old visual
	pContainer->RemoveVisual(pVisual);

	// Decompose old visual
	pVisual.Release();
	pSurface.Release();
}

HRESULT CComposedFrame::PlaceHighlight(IDCompositionDevice* pDevice)
{
    ASSERT(pDevice);

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    CRect rcHighlight(m_rcCurrent);
    rcHighlight.InflateRect(nHIGHLIGHT_MARGIN, nHIGHLIGHT_MARGIN);

    HRESULT hr = m_highlight.PlaceHighlight(pDevice, rcHighlight);

    return hr;
}

HRESULT CComposedFrame::ShowHighlight(IDCompositionVisual* pContainer, bool bShow /*=true*/)
{
    ASSERT(m_highlight.GetVisual() && pContainer);

    HRESULT hr = !(m_highlight.GetVisual() && pContainer) ? E_UNEXPECTED : S_OK;

    // Was planning to animate opacity using an EffectGroup, but:
    // 1) It's a pretty subtle effect, not actually that common in Win8
    //    (not used in window switchers)
    // 2) It would probably require Windows Animation Manager to support
    //    interrupting animations if the user mouses over things quickly
    // 3) Docs seem to suggest that it's better to move elements in and
    //    out of the visual tree than to leave them in and toggle opacity

    if (SUCCEEDED(hr) && m_bHighlighted != bShow)
    {
        if (bShow)
        {
            hr = pContainer->AddVisual(m_highlight.GetVisual(), FALSE, m_pVisual);
        }
        else if (m_bHighlighted)
        {
            hr = pContainer->RemoveVisual(m_highlight.GetVisual());
        }

        if (SUCCEEDED(hr))
        {
            m_bHighlighted = bShow;
        }
    }

    return hr;
}