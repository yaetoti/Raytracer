#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <wrl/client.h>

#include "Flame/render/DxContext.h"

namespace Flame {
  struct IndexBuffer final {
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

    HRESULT Init(const uint32_t* data, uint32_t size, UINT cpuAccess = 0) {
      Reset();

      // 16 byte alignment
      D3D11_BUFFER_DESC desc {
        static_cast<UINT>(sizeof(uint32_t)) * size,
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
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
        m_size = size;
      }

      return result;
    }

    HRESULT Init(uint32_t size, UINT cpuAccess = 0) {
      m_buffer.Reset();

      // 16 byte alignment
      D3D11_BUFFER_DESC desc {
        static_cast<UINT>(sizeof(uint32_t)) * size,
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        cpuAccess,
        0,
        0
      };

      HRESULT result = DxContext::Get()->d3d11Device->CreateBuffer(&desc, nullptr, m_buffer.GetAddressOf());
      if (SUCCEEDED(result)) {
        m_size = size;
      }

      return result;
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

  private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    uint32_t m_size = 0;
  };
}
