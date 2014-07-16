/*
This is currently only used to help work around issues with Ctrl+Tab (or Ctrl+F6)
and layered windows, but it could potentially be used for other scenarios where a
frozen capture of a window is required, such as using drag gestures to switch windows.
(i.e., a more interactive/animated version of Ctrl+Tab, or fading between restored/maximized
states while animating back from expose mode...)
*/

#pragma once

#include "DCompHelpers.h"

class CCapturedFrame
{
public:
    ~CCapturedFrame();

    // Creates and adds visual
    HRESULT Initialize(IDCompositionDevice* pDevice, IDCompositionVisual* pContainer, CWnd* pSource);

protected:
    CComPtr<IDCompositionVisual> m_pVisual;
    CComPtr<IDCompositionSurface> m_pSurface;

};