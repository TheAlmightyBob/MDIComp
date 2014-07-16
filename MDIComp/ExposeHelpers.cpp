#include "stdafx.h"
#include "ExposeHelpers.h"

#include "MathHelpers.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace ExposeHelpers
{

void EnableComposition(CWnd* pWnd, bool bEnable)
{
    if (!pWnd)
    {
        return;
    }

    if (bEnable)
    {
        pWnd->ModifyStyleEx(0, WS_EX_LAYERED);

        pWnd->SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
    }
    else
    {
        pWnd->ModifyStyleEx(WS_EX_LAYERED, 0);
    }
}

// Divide up the main window into a grid of regions per child
//
vector<CRect> Rectify(const CRect& rcTotal, int nCount, double dAvgRatio, int nMargin /*=10*/)
{
    vector<CRect> vRegions;
    vector<int> vCellsPerRow;
    int nRemainingCells = nCount;
    double dRatio = (double) rcTotal.Width() / rcTotal.Height();

    // Divide aspect ratio of main window by average aspect ratio of
    // child windows, to give us a rough idea of a reasonable rows/columns ratio
    // (e.g., if we have a wide window with a bunch of narrow children, we can
    //  just line up a whole bunch horizontally, etc)
    //
    dRatio /= dAvgRatio;

    while (0 < nRemainingCells)
    {
        int nCols = min((int) ceil(sqrt(nRemainingCells)*dRatio + 0.5), nRemainingCells);
        vCellsPerRow.push_back(nCols);
        nRemainingCells -= nCols;

        if (0 < nRemainingCells && nRemainingCells <= nCols)
        {
            vCellsPerRow.push_back(nRemainingCells);
            nRemainingCells = 0;
        }
    }

    int nRows = vCellsPerRow.size();
    int nHeight = rcTotal.Height() / nRows;
    int nHeightOffset = 0;

    for (int nCols : vCellsPerRow)
    {
        int nWidth = rcTotal.Width() / nCols;
        int nWidthOffset = 0;
        int nBottom = nHeightOffset + nHeight;

        for (int nCol = 0; nCol < nCols; nCol++)
        {
            vRegions.emplace_back(nWidthOffset, nHeightOffset, nWidthOffset + nWidth, nBottom);

            // Add margin (doesn't look good to have windows right up against each other)
            vRegions.back().DeflateRect(nMargin, nMargin);

            nWidthOffset += nWidth;
        }

        nHeightOffset += nHeight;
    }

    return vRegions;
}

// Iterate the regions and assign the nearest frame to them.
//
void PlaceWindows(const std::vector<CRect>& vRegions, std::vector<CComposedFrame>& vFrames)
{
    for (const CRect& rect : vRegions)
    {
        CPoint ptCenter = rect.CenterPoint();

        auto nearestFrame = vFrames.end();
        double dNearestDistance = DBL_MAX;

        // TODO: We could also perform the double-check of "if the window
        //       is nearest to this region AND it's nearer to this region
        //       than it is to any other region"..... but this is probably good enough.
        //
        for (auto it = vFrames.begin(); it != vFrames.end(); it++)
        {
            if (!it->IsAssigned())
            {
                double dDistance = MathHelpers::CalcDistance(it->GetOriginalCenter(), ptCenter);

                if (dDistance < dNearestDistance)
                {
                    nearestFrame = it;
                    dNearestDistance = dDistance;
                }
            }
        }

        ASSERT(nearestFrame != vFrames.end() && !nearestFrame->IsAssigned());

        if (nearestFrame != vFrames.end())
        {
            // Assign window to region
            //
            nearestFrame->SetDCRegion(rect);
        }
    }
}

}