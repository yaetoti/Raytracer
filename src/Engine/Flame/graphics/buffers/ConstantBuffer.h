#pragma once

#include <cassert>
#include <cstring>

#include "Flame/graphics/DxContext.h"

namespace Flame {
  template <typename T>
  struct ConstantBuffer final {
    T data;

    ConstantBuffer()
    : data(static_cast<T>(0)) {
    }

    ID3D11Buffer* Get() const {
      return m_buffer.Get();
    }

    ID3D11Buffer* const* GetAddressOf() const {
      return m_buffer.GetAddressOf();
    }

    void Reset() {
      m_buffer.Reset();
    }

    HRESULT Init(UINT cpuAccess = D3D11_CPU_ACCESS_WRITE) {
      m_buffer.Reset();

      // 16 byte alignment
      D3D11_BUFFER_DESC desc {
        sizeof(T) + 16 - sizeof(T) % 16,
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_CONSTANT_BUFFER,
        cpuAccess,
        0,
        0
      };

      D3D11_SUBRESOURCE_DATA dataDesc {
        &data,
        0,
        0
      };

      HRESULT result = DxContext::Get()->d3d11Device->CreateBuffer(&desc, &dataDesc, m_buffer.GetAddressOf());
      return result;
    }

    D3D11_MAPPED_SUBRESOURCE Map(D3D11_MAP mapType) const {
      D3D11_MAPPED_SUBRESOURCE mapping {};
      HRESULT result = DxContext::Get()->d3d11DeviceContext->Map(Get(), 0, mapType, 0, &mapping);
      assert(SUCCEEDED(result));
      return mapping;
    }

    void Update(const T* data) {
      std::memcpy(&this->data, data, sizeof(T));
      DxContext::Get()->d3d11DeviceContext->UpdateSubresource(Get(), 0, nullptr, data, 0, 0);
    }

    void Unmap() const {
      DxContext::Get()->d3d11DeviceContext->Unmap(Get(), 0);
    }

    void ApplyChanges() const {
      auto subres = Map(D3D11_MAP_WRITE_DISCARD);
      std::memcpy(subres.pData, &data, sizeof(T));
      Unmap();
    }

  private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
  };
}
