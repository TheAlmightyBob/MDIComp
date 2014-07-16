#include "stdafx.h"
#include "D2DHelpers.h"

#include <d2d1_1helper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace D2DHelpers
{

	HRESULT CreateD3DDevice(ID3D11Device** ppDevice)
	{
		HRESULT hr = (ppDevice == nullptr || *ppDevice != nullptr) ? E_UNEXPECTED : S_OK;

		if (SUCCEEDED(hr))
		{
			D3D_DRIVER_TYPE driverTypes[] =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP
			};

			for (int i = 0; i < sizeof(driverTypes) / sizeof(driverTypes[0]); ++i)
			{
				CComPtr<ID3D11Device> d3dDevice;

                unsigned flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT |
                    D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
                flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

				hr = ::D3D11CreateDevice(
					nullptr,
					driverTypes[i],
					NULL,
                    flags,
					nullptr,
					0,
					D3D11_SDK_VERSION,
					&d3dDevice,
					nullptr,
					nullptr);

				if (SUCCEEDED(hr))
				{
					*ppDevice = d3dDevice.Detach();

					break;
				}
			}
		}

		return hr;
	}

#ifdef _DEBUG
    void ReportLiveDeviceObjects(ID3D11Device* pD3DDevice)
    {
        HRESULT hr = (pD3DDevice == nullptr) ? E_UNEXPECTED : S_OK;

        if (SUCCEEDED(hr))
        {
            CComPtr<ID3D11Debug> d3dDebug;

            hr = pD3DDevice->QueryInterface(&d3dDebug);

            if (SUCCEEDED(hr))
            {
                d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
            }
        }
    }
#endif

    HRESULT CreateD2DDevice(ID3D11Device* pD3DDevice, ID2D1Device** ppD2DDevice)
    {
        HRESULT hr = ((pD3DDevice == nullptr) || (ppD2DDevice == nullptr) || (*ppD2DDevice != nullptr))
            ? E_UNEXPECTED : S_OK;

        if (SUCCEEDED(hr))
        {
            CComPtr<IDXGIDevice> dxgiDevice;

            hr = pD3DDevice->QueryInterface(&dxgiDevice);

            if (SUCCEEDED(hr))
            {
                D2D1_CREATION_PROPERTIES properties = {};

#ifdef _DEBUG
                properties.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

                hr = ::D2D1CreateDevice(dxgiDevice, properties, ppD2DDevice);
            }
        }

        return hr;
    }

    //HRESULT CreateDWriteFactory(IDWriteFactory** ppFactory)
    //{
    //    HRESULT hr = ((ppFactory == nullptr) || (*ppFactory != nullptr)) ? E_UNEXPECTED : S_OK;

    //    if (SUCCEEDED(hr))
    //    {
    //        hr = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(ppFactory));
    //    }

    //    return hr;
    //}
}