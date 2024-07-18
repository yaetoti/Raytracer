#include "DxContext.h"

#include <assert.h>

namespace Flame {
  std::unique_ptr<DxContext> DxContext::m_instance = nullptr;

  bool DxContext::Init(HWND hWnd) {
    // TODO move swapchain / backbuffer creation into Window class
    HRESULT result;

    // Init swap chain
    {
      DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
      swapChainDesc.BufferCount = 1;
      swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM ;
      swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
      swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.OutputWindow = hWnd;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.Windowed = true;

      result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        dxgiSwapChain.GetAddressOf(),
        d3d11Device.GetAddressOf(),
        nullptr,
        d3d11DeviceContext.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Retrieve render texture
    {
      result = dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(d3d11RenderTexture.GetAddressOf()));
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Create render target view
    {
      result = d3d11Device->CreateRenderTargetView(
        d3d11RenderTexture.Get(),
        nullptr,
        d3d11TargetView.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }
  }

  DxContext* DxContext::Get() {
    if (!m_instance) {
      m_instance = std::make_unique<DxContext>();
    }

    return m_instance.get();
  }
}
