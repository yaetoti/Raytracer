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
    }

    // Get primary adapter
    {
      result = dxgiFactory->EnumAdapters(0, dxgiAdapter.GetAddressOf());
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Get output
    //{
    //  result = dxgiAdapter->EnumOutputs(0, dxgiOutput.GetAddressOf());
    //  assert(SUCCEEDED(result));
    //  if (FAILED(result)) {
    //    return false;
    //  }

    //  // Get modes count
    //  uint32_t modesNumber;
    //  result = dxgiOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modesNumber, nullptr);
    //  assert(SUCCEEDED(result));
    //  if (FAILED(result)) {
    //    return false;
    //  }

    //  // Get modes
    //  std::vector<DXGI_MODE_DESC> modes;
    //  modes.resize(modesNumber);
    //  result = dxgiOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modesNumber, modes.data());
    //  assert(SUCCEEDED(result));
    //  if (FAILED(result)) {
    //    return false;
    //  }

    //  for (uint32_t i = 0; i < modesNumber; ++i) {
    //    
    //  }
    //}

    // Create D3D Device
    {
      result = D3D11CreateDevice(
        dxgiAdapter.Get(),
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED,
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
    }

    return true;
  }

  void DxContext::Deinit() {

  }

  DxContext* DxContext::Get() {
    if (!m_instance) {
      m_instance = new DxContext();
    }

    return m_instance;
  }
}
