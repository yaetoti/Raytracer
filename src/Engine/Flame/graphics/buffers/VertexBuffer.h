#pragma once
#include <cassert>

#include "Flame/graphics/DxContext.h"

namespace Flame {
  template <typename T>
  struct VertexBuffer final {
    explicit VertexBuffer() = default;

    HRESULT Init(const T* data, UINT count, UINT cpuAccess = 0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE) {
      Reset();

      D3D11_BUFFER_DESC desc {
        m_stride * count,
        usage,
        D3D11_BIND_VERTEX_BUFFER,
        cpuAccess,
        0,
        0
      };

      D3D11_SUBRESOURCE_DATA dataDesc {
        data,
        0,
        0
      };

      HRESULT result = DxContext::Get()->d3d11Device->CreateBuffer(&desc, &dataDesc, m_buffer.GetAddressOf());
      if (SUCCEEDED(result)) {
        m_size = count;
      }

      return result;
    }

    HRESULT Init(UINT count, UINT cpuAccess = 0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE) {
      D3D11_BUFFER_DESC desc {
        m_stride * count,
        usage,
        D3D11_BIND_VERTEX_BUFFER,
        cpuAccess,
        0,
        0
      };

      HRESULT result = DxContext::Get()->d3d11Device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf());
      if (SUCCEEDED(result)) {
        m_size = count;
      }

      return result;
    }

    ID3D11Buffer* Get() const {
      return m_buffer.Get();
    }

    ID3D11Buffer* const* GetAddressOf() const {
      return m_buffer.GetAddressOf();
    }

    void Reset() {
      m_buffer.Reset();
      m_size = 0;
    }

    void Update(const T* data) {
      DxContext::Get()->d3d11DeviceContext->UpdateSubresource(Get(), 0, nullptr, data, 0, 0);
    }

    D3D11_MAPPED_SUBRESOURCE Map(D3D11_MAP mapType) const {
      D3D11_MAPPED_SUBRESOURCE mapping {};
      HRESULT result = DxContext::Get()->d3d11DeviceContext->Map(Get(), 0, mapType, 0, &mapping);
      assert(SUCCEEDED(result));
      return mapping;
    }

    void Unmap() const {
      DxContext::Get()->d3d11DeviceContext->Unmap(Get(), 0);
    }

    UINT GetStride() const {
      return m_stride;
    }

    const UINT* GetStridePtr() const {
      return &m_stride;
    }

  private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    UINT m_stride = sizeof(T);
    UINT m_size = 0;
  };
}
