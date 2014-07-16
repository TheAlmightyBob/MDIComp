#pragma once

namespace WindowHelpers
{
    bool IsMaximized(CWnd* pWnd);

    WINDOWPLACEMENT GetWindowPlacement(CWnd* pWnd);

    void TrackMouseLeave(HWND hWnd);
}