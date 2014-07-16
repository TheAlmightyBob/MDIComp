#include "stdafx.h"
#include "Exposer.h"

#include "ExposeHelpers.h"
#include "WindowHelpers.h"
#include "CapturedFrame.h"

#include <algorithm>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDT_EXPOSE 42
#define IDT_CONCEAL 43
#define IDT_CURTAIN 44

const double dDURATION_DEFAULT = 0.3;


CExposer::CExposer(CMDIFrameWnd* pWnd, UINT uiExposeCommand, UINT uiConcealCommand)
: m_pMainWnd(pWnd)
, m_bDirectCompInitialized(false)
, m_bExpose(false)
, m_bAnimating(false)
, m_uiExposeTimer(0)
, m_uiConcealTimer(0)
, m_uiCurtainTimer(0)
, m_dDuration(dDURATION_DEFAULT)
, m_pHighlightedFrame(nullptr)
, m_pCurtain(nullptr)
, m_uiExposeCommand(uiExposeCommand)
, m_uiConcealCommand(uiConcealCommand)
{
}

CComPtr<IDCompositionDevice> CExposer::GetDevice()
{
    return m_compSurface.GetDevice(); 
}

CComPtr<IDCompositionVisual> CExposer::GetContainer() 
{ 
    return m_compSurface.GetContainer(); 
}

HRESULT CExposer::Initialize()
{
    HRESULT hr = S_OK;

    hr = m_compSurface.Initialize(m_pMainWnd->m_hWndMDIClient);

    if (SUCCEEDED(hr))
    {
        m_bDirectCompInitialized = true;

        // (moved here so we can handle device change before invoking Expose...
        //  ...it's still theoretically possible for the device to change between
        //  initializing and hooking these up, but c'mon.....)
        //
        // We need to subclass both the main window *and* the MDIClient, to
        // handle selection clicks as well as bailing out if the user opens a menu etc.
        //
        SetWindowSubclass(m_pMainWnd->m_hWndMDIClient, SubclassProc, 0, (DWORD_PTR)this);
        SetWindowSubclass(m_pMainWnd->m_hWnd, SubclassProc, 0, (DWORD_PTR)this);
    }

    if (SUCCEEDED(hr))
    {
        hr = GetDevice()->Commit();
    }

    return hr;
}

void CExposer::SetAnimDuration(double dDuration)
{
    m_dDuration = dDuration;
}

void CExposer::Expose()
{
    if (m_bDirectCompInitialized && !m_bExpose)
    {
        m_bExpose = true;

        double dRatioSum = 0.0;

        ASSERT(m_vFrames.size() == 0);
        m_vFrames.clear(); // just in case...?

        // Collect frames
        IterateFrames([this, &dRatioSum](CWnd* pFrame)
        {
            m_vFrames.emplace_back(pFrame);
            dRatioSum += m_vFrames.back().GetAspectRatio();
        });

        if (m_vFrames.size() == 0)
        {
            // okay there is LITERALLY NOTHING to do
            m_bExpose = false;
            return;
        }

        double dRatioAvg = dRatioSum / m_vFrames.size();

        CRect rcTotal;
        ::GetClientRect(m_pMainWnd->m_hWndMDIClient, &rcTotal);

        auto vRegions = ExposeHelpers::Rectify(rcTotal, m_vFrames.size(), dRatioAvg);

        // Iterate the regions and assign the nearest frame to them.
        //
        ExposeHelpers::PlaceWindows(vRegions, m_vFrames);

        // Compose! Animate!
        // (we do this according to frames rather than regions to preserve Z-order)
        //
        for (CComposedFrame& frame : m_vFrames)
        {
            frame.Compose(GetDevice(), GetContainer());
            frame.AnimateToRegion(GetDevice(), rcTotal, m_dDuration);
        }
        m_bAnimating = true;

        // Commit DirectComp changes!
        HRESULT hr = GetDevice()->Commit();

        if (FAILED(hr))
        {
            m_bAnimating = false;
            Conceal();
            // Check for and handle device loss?
            return;
        }

        // Hide! Flee! Run away!
        for (const CComposedFrame& frame : m_vFrames)
        {
            frame.Cloak();
        }

        m_uiExposeTimer = SetTimer(m_pMainWnd->m_hWndMDIClient, IDT_EXPOSE, (UINT) (m_dDuration * 1000), nullptr);
    }
}

void CExposer::Conceal()
{
    // This does not include animation (there are other functions for that),
    // this is just the final switch back from DComp visuals to real windows.
    // (this may be called directly instead of the animation functions if
    //  we need to suddenly abort... e.g. window resize, new file, menus, ctrl-tab...)
    //
    if (m_bExpose)
    {
        m_bExpose = false;

        GetContainer()->RemoveAllVisuals();

        m_pHighlightedFrame = nullptr;
        m_vFrames.clear();

        // We do the commit after the clear (which de-cloaks the real windows) to
        // minimize flicker when we get rid of the DirectComp visuals
        // (there's still some.... but I'm not sure what to do besides sleep?)
        // (sleep *does* appear to help, but still not crazy about it)
        //
        GetDevice()->Commit();
    }
}

template<class Func>
void CExposer::IterateFrames(Func F)
{
    CWnd* pFrame = m_pMainWnd->GetActiveFrame();

    while (pFrame)
    {
        F(pFrame);
        pFrame = pFrame->GetNextWindow(GW_HWNDNEXT);
    }
}

CComposedFrame* CExposer::HitTest(const CPoint& pt)
{
    auto it = std::find_if(begin(m_vFrames), end(m_vFrames),
        [&pt](const CComposedFrame& frame)
    {
        return frame.HitTest(pt);
    });

    return it != end(m_vFrames) ? &(*it) : nullptr;
}

void CExposer::HighlightFrame(CComposedFrame* pFrame)
{
    if (pFrame != m_pHighlightedFrame)
    {
        if (m_pHighlightedFrame)
        {
            m_pHighlightedFrame->ShowHighlight(GetContainer(), false);
        }

        if (pFrame)
        {
            pFrame->ShowHighlight(GetContainer(), true);

            // Set up monitoring of MouseLeave, to make sure we clear the hover state
            //
            WindowHelpers::TrackMouseLeave(m_pMainWnd->m_hWndMDIClient);
        }

        m_pHighlightedFrame = pFrame;

        GetDevice()->Commit();
    }
}

void CExposer::SelectFrame(CComposedFrame* pFrame)
{
    if (pFrame)
    {
        pFrame->Activate(GetDevice(), GetContainer());
        
        AnimateBack();
    }
}

void CExposer::AnimateBack()
{
    for (CComposedFrame& frame : m_vFrames)
    {
        frame.AnimateBack(GetDevice(), GetContainer(), m_dDuration);
    }
    m_bAnimating = true;
    GetDevice()->Commit();
    m_uiConcealTimer = ::SetTimer(m_pMainWnd->m_hWndMDIClient, IDT_CONCEAL, (UINT) (m_dDuration * 1000), nullptr);
}

LRESULT CALLBACK CExposer::SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CExposer* pExposer = reinterpret_cast<CExposer*>(dwRefData);
    ASSERT(pExposer);

    if (pExposer)
    {
        bool bHandled = false;

        if (hWnd == pExposer->m_pMainWnd->m_hWndMDIClient)
        {
            bHandled = pExposer->HandleMdiWndMsg(msg, wParam, lParam);
        }
        else if (hWnd == pExposer->m_pMainWnd->m_hWnd)
        {
            bHandled = pExposer->HandleMainWndMsg(msg, wParam, lParam);
        }

        if (bHandled)
        {
            return 0;
        }
    }

    if (msg == WM_NCDESTROY)
    {
        // Clean up the curtain (if closing the application whilst maximized,
        // the MDIDESTROY causes the curtain to appear) & some other resources
        //
        if (pExposer && pExposer->m_bDirectCompInitialized)
        {
            pExposer->HideCurtain();

            // Abandon ship!
            pExposer->Conceal();
            CHighlight::ReleaseStaticDeviceResources();
        }

        RemoveWindowSubclass(hWnd, SubclassProc, uIdSubclass);
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

// Returns true if handled (i.e., if default wndproc should be skipped)
//
bool CExposer::HandleMainWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool bHandled = false;
    WORD cmd = LOWORD(wParam);

    // If receiving a regular command that's not expose-related or resizing the window,
    // immediately bail out of expose mode because the rest of the system ain't gonna
    // wait for us to animate things
    //
    // TODO: Actually handle the expose command ourselves rather than making
    //       the application responsible for that?
    //       Also, if already exposed, then use the expose command to iterate windows?
    //       (we could also make it behave more like alt-tab, where it only stays open
    //        as long as the user is holding down some modifier key(s)... but that seems
    //        overly complex...)
    //       - I'm still somewhat hesitant to take activation out of the hands of the
    //         main app, just in case it wants more control.... might even be handy
    //         to support a callback for when we're done, in case it wants to restore
    //         any extra state.
    //
    if (msg == WM_COMMAND && cmd == m_uiConcealCommand)
    {
        AnimateBack();
        bHandled = true;
    }
    else if ((msg == WM_COMMAND && cmd != m_uiExposeCommand) ||
             (msg == WM_SIZE))
    {
        Conceal();
    }

    return bHandled;
}

// Returns true if handled (i.e., if default wndproc should be skipped)
//
bool CExposer::HandleMdiWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool bHandled = false;

    if (!m_bDirectCompInitialized)
    {
        return false;
    }

    if (m_bExpose)
    {
        bHandled = true;

        switch (msg)
        {
        case WM_MOUSELEAVE:
            if (!m_bAnimating)
            {
                HighlightFrame(nullptr);
            }
            break;
        case WM_MOUSEMOVE:
            if (!m_bAnimating)
            {
                CComposedFrame* pFrame = HitTest(lParam);

                HighlightFrame(pFrame);
            }
            break;
        case WM_LBUTTONDOWN:
            if (!m_bAnimating)
            {
                CPoint pt(lParam);
                CComposedFrame* pFrame = HitTest(pt);

                if (pFrame)
                {
                    SelectFrame(pFrame);
                }
            }
            break;
        case WM_TIMER:
            if (m_uiExposeTimer && m_uiExposeTimer == wParam)
            {
                KillTimer(m_pMainWnd->m_hWndMDIClient, m_uiExposeTimer);
                m_uiExposeTimer = 0;

                m_bAnimating = false;
            }
            else if (m_uiConcealTimer && m_uiConcealTimer == wParam)
            {

                KillTimer(m_pMainWnd->m_hWndMDIClient, m_uiConcealTimer);
                m_uiConcealTimer = 0;

                m_bAnimating = false;
                Conceal();
            }
            break;
        case WM_MDINEXT:
            break; // not allowed during Expose... for now
        case WM_MDIACTIVATE:
        case WM_MDICREATE:
        case WM_MDIDESTROY:
            // bail out
            Conceal();
            bHandled = false; // allow the action to occur
            break;
        default:
            bHandled = false;
            break;
        }
    }
    else
    {
        // The normal MDINEXT behavior doesn't seem to play well with layered child windows.
        // 
        if (msg == WM_MDINEXT)
        {
            CWnd* pFrame = CWnd::FromHandle((HWND) wParam);
            ASSERT_VALID(pFrame);

            if (pFrame && pFrame->IsZoomed())
            {
                OnMdiNext(pFrame, lParam == 0);

                bHandled = true;
            }
        }
        else if (msg == WM_MDIACTIVATE || msg == WM_MDICREATE || msg == WM_MDIDESTROY)
        {
            CWnd* pFrame = m_pMainWnd->GetActiveFrame();

            ASSERT_VALID(pFrame);

            if (pFrame && pFrame->IsZoomed())
            {
                ShowCurtain(pFrame);
            }
        }
        else if (msg == WM_TIMER && wParam == IDT_CURTAIN)
        {
            CWnd* pFrame = m_pMainWnd->GetActiveFrame();
            if (pFrame)
            {
                pFrame->RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_ERASE);
            }
            HideCurtain();

            bHandled = true;
        }
    }

    if (msg == WM_PAINT)
    {
        if (!m_compSurface.AreDeviceResourcesValid())
        {
            HandleDeviceLoss();
        }
    }

    return bHandled;
}


void CExposer::OnMdiNext(CWnd* pFrame, bool bNext)
{
    ASSERT(pFrame);

    if (!pFrame || !pFrame->IsZoomed())
    {
        return;
    }

    CWnd* pNextFrame = pFrame->GetNextWindow(bNext ? GW_HWNDNEXT : GW_HWNDLAST);
    ASSERT(pNextFrame);

    if (!pNextFrame)
    {
        return;
    }

    // Create the "curtain" behind which our window switching will be taking
    // place
    //
    ShowCurtain(pFrame);

    // Even with the screen (and other ideas, such as cloaking windows and calling SetRedraw(FALSE)),
    // the default MdiNext handling causes flicker. Not sure why. So instead we reverse-engineer
    // its basic logic of bringing windows to top and sending other windows to bottom.
    //
    pNextFrame->BringWindowToTop();

    if (bNext)
    {
        pFrame->SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
    }
}

void CExposer::ShowCurtain(CWnd* pWnd)
{
    ASSERT(pWnd);
    ASSERT_VALID(pWnd);

    if (m_pCurtain)
    {
        pWnd->RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_ERASE);

        HideCurtain();
    }

    ASSERT(!m_pCurtain);

    if (pWnd)
    {
        m_pCurtain = new CCapturedFrame();

        m_pCurtain->Initialize(GetDevice(), GetContainer(), pWnd);
        GetDevice()->Commit();

        // Don't want to start moving windows until we're sure that the curtain is up
        GetDevice()->WaitForCommitCompletion();

        m_uiCurtainTimer = SetTimer(m_pMainWnd->m_hWndMDIClient, IDT_CURTAIN, 1, nullptr);
    }
}

void CExposer::HideCurtain()
{
    KillTimer(m_pMainWnd->m_hWndMDIClient, IDT_CURTAIN);
    GetContainer()->RemoveAllVisuals();
    GetDevice()->Commit();
    delete m_pCurtain;
    m_pCurtain = nullptr;
}

void CExposer::HandleDeviceLoss()
{
    Conceal();
    CHighlight::ReleaseStaticDeviceResources();
    HideCurtain();
    m_compSurface.RecreateDeviceResources();
}