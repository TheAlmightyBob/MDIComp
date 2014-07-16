#pragma once

#include <vector>

#include "ComposedFrame.h"

namespace ExposeHelpers
{
    // Toggle automatic window layering (as opposed to the UpdateLayeredWindow route,
    // which probably works better but requires more responsibility)
    void EnableComposition(CWnd* pWnd, bool bEnable = true);

    std::vector<CRect> Rectify(const CRect& rcTotal, int nCount, double dAvgChildRatio, int nMargin = 10);

    void PlaceWindows(const std::vector<CRect>& vRegions, std::vector<CComposedFrame>& vFrames);
}