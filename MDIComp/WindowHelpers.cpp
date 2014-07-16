#include "stdafx.h"

#include "WindowHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace WindowHelpers
{
    bool IsMaximized(CWnd* pWnd)
    {
        ASSERT(pWnd);

        if (!pWnd)
        {
            return false;
        }

        bool bMaximized = false;
        WINDOWPLACEMENT placement = WINDOWPLACEMENT();

        if (pWnd->GetWindowPlacement(&placement))
        {
            if (placement.showCmd == SW_MAXIMIZE)
            {
                bMaximized = true;
            }
        }

        return bMaximized;
    }

    WINDOWPLACEMENT GetWindowPlacement(CWnd* pWnd)
    {
        ASSERT(pWnd);

        WINDOWPLACEMENT placement = {};

        VERIFY(pWnd->GetWindowPlacement(&placement));

        return placement;
    }

    void WindowHelpers::TrackMouseLeave(HWND hWnd)
    {
        TRACKMOUSEEVENT trackmouseevent;
        trackmouseevent.cbSize = sizeof(trackmouseevent);
        trackmouseevent.dwFlags = TME_QUERY;
        TrackMouseEvent(&trackmouseevent);

        if (trackmouseevent.dwFlags == 0)
        {
            trackmouseevent.dwFlags = TME_LEAVE;
            trackmouseevent.hwndTrack = hWnd;
            TrackMouseEvent(&trackmouseevent);
        }
    }
}