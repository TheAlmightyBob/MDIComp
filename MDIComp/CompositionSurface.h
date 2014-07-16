/*
Represents the canvas upon which DirectComposition visuals are drawn.
*/
#pragma once

#include "DeviceResources.h"

class CCompositionRoot
{
public:
    HRESULT Initialize(HWND hWndTarget);

    IDCompositionDevice* GetDevice() const { return m_devResources.GetDCompDevice(); }
    CComPtr<IDCompositionVisual> GetContainer() { return m_pRootVisual; }

    void RecreateDeviceResources();

    static bool AreDeviceResourcesValid();

protected:
    static CDeviceResources m_devResources;

    // These are also device resources
    CComPtr<IDCompositionTarget> m_pHwndRenderTarget;
    CComPtr<IDCompositionVisual> m_pRootVisual;

    HWND m_hwndTarget;
};