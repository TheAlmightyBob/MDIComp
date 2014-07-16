#include "stdafx.h"
#include "MathHelpers.h"

#include <cmath>

namespace MathHelpers
{

double CalcDistance(const CPoint& pt1, const CPoint& pt2)
{
    return hypot(pt1.x - pt2.x, pt1.y - pt2.y);
}

float AspectFit(const CRect& rcSource, const CRect& rcDest)
{
    float fx = (float) rcDest.Width() / rcSource.Width();
    float fy = (float) rcDest.Height() / rcSource.Height();

    return min(min(fx, fy), 1.0f);
}

}