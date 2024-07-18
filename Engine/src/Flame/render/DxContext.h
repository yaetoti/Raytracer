#pragma once

#include <memory>
#include <wrl/client.h>
#include "dxgi.h"
#include "d3d11.h"

namespace Flame {
  struct DxContext final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    bool Init(HWND hWnd);

    static DxContext* Get();

  public:
    ComPtr<IDXGIFactory> dxgiFactory;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ComPtr<ID3D11Device> d3d11Device;
    ComPtr<ID3D11DeviceContext> d3d11DeviceContext;

    ComPtr<IDXGISwapChain> dxgiSwapChain;
    ComPtr<ID3D11Texture2D> d3d11RenderTexture;
    ComPtr<ID3D11RenderTargetView> d3d11TargetView;

  private:
    DxContext() = default;

    static std::unique_ptr<DxContext> m_instance;
  };
}
