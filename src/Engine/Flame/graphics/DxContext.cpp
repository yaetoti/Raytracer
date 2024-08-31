#include "DxContext.h"

#include <cassert>
#include <vector>

namespace Flame {
  DxContext* DxContext::m_instance = nullptr;

  bool DxContext::Init() {
    // TODO move swapchain / backbuffer creation into Window class
    HRESULT result;

    // DXGI Factory
    {
      result = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }

      result = dxgiFactory.As(&dxgiFactory2);
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Get primary adapter
    {
      result = dxgiFactory->EnumAdapters(0, dxgiAdapter.GetAddressOf());
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Create D3D Device
    {
      result = D3D11CreateDevice(
        dxgiAdapter.Get(),
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        d3d11Device.GetAddressOf(),
        nullptr,
        d3d11DeviceContext.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }

      result = d3d11Device.As(&d3d11Debug);;
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    return true;
  }

  void DxContext::Cleanup() {
    d3d11DeviceContext->ClearState();
    d3d11DeviceContext->Flush();
    d3d11DeviceContext.Reset();
    dxgiOutput.Reset();
    dxgiAdapter.Reset();
    dxgiFactory2.Reset();
    dxgiFactory.Reset();
    d3d11Debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
    d3d11Debug.Reset();
    d3d11Device.Reset();
  }

  DxContext* DxContext::Get() {
    if (!m_instance) {
      m_instance = new DxContext();
    }

    return m_instance;
  }
}
