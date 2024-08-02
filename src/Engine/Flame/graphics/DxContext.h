#pragma once

#include <wrl/client.h>
#include "dxgi.h"
#include "dxgi1_2.h"
#include "d3d11.h"
#include <dxgidebug.h>

namespace Flame {
  struct DxContext final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    bool Init();
    void Cleanup();

    static DxContext* Get();

  public:
    ComPtr<IDXGIFactory> dxgiFactory;
    ComPtr<IDXGIFactory2> dxgiFactory2;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ComPtr<IDXGIOutput> dxgiOutput;

    ComPtr<ID3D11Device> d3d11Device;
    ComPtr<ID3D11DeviceContext> d3d11DeviceContext;

    ComPtr<ID3D11Debug> d3d11Debug;

  private:
    DxContext() = default;

    static DxContext* m_instance;
  };
}
