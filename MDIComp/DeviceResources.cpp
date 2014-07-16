#include "stdafx.h"
#include "DeviceResources.h"

#include "D2DHelpers.h"
#include "DCompHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// TODO: Move this to some more common place
//       (and actually use it in more places so we don't just silently fail..)
//
#ifdef _DEBUG
#define HR(expression) ASSERT(S_OK == (expression))
#else
struct ComException
{
    HRESULT const hr;
    ComException(HRESULT const value) : hr(value) {}
};
inline void HR(HRESULT const hr)
{
    if (S_OK != hr) throw ComException(hr);
}
#endif

CDeviceResources::CDeviceResources()
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

void CDeviceResources::RecreateDeviceResources()
{
    ReleaseDeviceResources();
    CreateDeviceResources();
}

void CDeviceResources::CreateDeviceIndependentResources()
{
    //D2DHelpers::CreateDWriteFactory(&m_pdwriteFactory);
}

void CDeviceResources::CreateDeviceResources()
{
    HRESULT hr = D2DHelpers::CreateD3DDevice(&m_pd3dDevice);

    if (SUCCEEDED(hr))
    {
        D2DHelpers::CreateD2DDevice(m_pd3dDevice, &m_pd2dDevice);
    }

    if (SUCCEEDED(hr) && !m_pdcompDevice)
    {
        hr = DCompHelpers::CreateDCompDevice(&m_pdcompDevice, GetD2DDevice());
    }
}

void CDeviceResources::ReleaseDeviceResources()
{
    m_pdcompDevice.Release();
    m_pd2dDevice.Release();

#ifdef _DEBUG
    if (GetD3DDevice())
    {
        D2DHelpers::ReportLiveDeviceObjects(GetD3DDevice());
    }
#endif

    m_pd3dDevice.Release();
}

bool CDeviceResources::HasValidDevices() const
{
    // The "official" technique for checking device validity is supposedly
    // just to use GetDeviceRemovedReason, but:
    // 1. CompositionDevice also has a CheckDeviceState function...
    // 2. In my testing experience, both GetDeviceRemovedReason and CheckDeviceState
    //    may claim that everything is fine when the device has in fact been lost...
    // 3. The Visual Studio DirectX project template also checked the adapter LUIDs
    //    in its device loss handling logic, and *that* appears to show a change when
    //    the other's don't. (that logic is directly copied from the project template)
    //    (note that this introduces an otherwise unnecessary dependency on the DXGI lib)
    //
    // ...so we just check everything.

    // Do the devices exist?
    //
    if (!(GetD3DDevice() && GetD2DDevice() && GetDCompDevice()))
    {
        return false;
    }

    // Is the DirectComposition device valid?
    //
    BOOL bValid = FALSE;

    if (!SUCCEEDED(GetDCompDevice()->CheckDeviceState(&bValid)) || !bValid)
    {
        return false;
    }

    // Was the Direct3D device removed?
    //
    if (GetD3DDevice()->GetDeviceRemovedReason() != S_OK)
    {
        return false;
    }

    // The D3D Device is no longer valid if the default adapter changed since the device
    // was created or if the device has been removed.

    // First, get the information for the default adapter from when the device was created.

    CComPtr<IDXGIDevice> dxgiDevice;
    HR(GetD3DDevice()->QueryInterface(&dxgiDevice));

    CComPtr<IDXGIAdapter> deviceAdapter;
    HR(dxgiDevice->GetAdapter(&deviceAdapter));

    CComPtr<IDXGIFactory2> deviceFactory;
    HR(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

    CComPtr<IDXGIAdapter1> previousDefaultAdapter;
    HR(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

    DXGI_ADAPTER_DESC previousDesc;
    HR(previousDefaultAdapter->GetDesc(&previousDesc));

    // Next, get the information for the current default adapter.

    CComPtr<IDXGIFactory2> currentFactory;
    HR(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

    CComPtr<IDXGIAdapter1> currentDefaultAdapter;
    HR(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

    DXGI_ADAPTER_DESC currentDesc;
    HR(currentDefaultAdapter->GetDesc(&currentDesc));

    // If the adapter LUIDs don't match, or if the device reports that it has been removed,
    // a new D3D device must be created.

    if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
        previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart)
    {
        return false;
    }

    return true;
}