#pragma once

#include <d3d11_1.h>
#include <d2d1_1.h>
#include "DCompHelpers.h"

// Manages both device resources and device-independent resources
// (the naming is borrowed from a VS template...)

class CDeviceResources
{
public:
    CDeviceResources();

    // D3D Accessors.
    ID3D11Device*           GetD3DDevice() const        { return m_pd3dDevice; }

    // D2D Accessors.
    ID2D1Device*            GetD2DDevice() const        { return m_pd2dDevice; }
    //IDWriteFactory*         GetDWriteFactory() const    { return m_pdwriteFactory; }

    // DComp Accessors.
    IDCompositionDevice*    GetDCompDevice() const      { return m_pdcompDevice; }

    bool HasValidDevices() const;

    void RecreateDeviceResources();

private:
    void CreateDeviceIndependentResources();
    void CreateDeviceResources();
    void ReleaseDeviceResources();


    // Direct3D
    CComPtr<ID3D11Device>        m_pd3dDevice;

    // Direct2D
    CComPtr<ID2D1Device>         m_pd2dDevice;

    // DirectComposition
    CComPtr<IDCompositionDevice> m_pdcompDevice;

    // DirectWrite - Not yet used
    //CComPtr<IDWriteFactory>      m_pdwriteFactory;
};