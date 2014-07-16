#pragma once

#include <d3d11_1.h>
#include <d2d1_1.h>

namespace D2DHelpers
{
	// This here because it seems silly to have a D3DHelpers with only one function,
	// but D2D requires D3D
	//
	HRESULT CreateD3DDevice(ID3D11Device** ppDevice);

    HRESULT CreateD2DDevice(ID3D11Device* pD3DDevice, ID2D1Device** ppD2DDevice);

    // Not yet used
    //HRESULT CreateDWriteFactory(IDWriteFactory** ppFactory);

#ifdef _DEBUG
    void ReportLiveDeviceObjects(ID3D11Device* pD3DDevice);
#endif
}