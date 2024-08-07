#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Flame {
  struct Texture final {
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    bool InitFromFile(const wchar_t* path);
    void Reset();

    ID3D11Resource* GetResource() const;
    ID3D11ShaderResourceView* GetResourceView() const;
    ID3D11Resource* const* GetResourceAddress() const;
    ID3D11ShaderResourceView* const* GetResourceViewAddress() const;

  private:
    ComPtr<ID3D11Resource> m_resource;
    ComPtr<ID3D11ShaderResourceView> m_resourceView;
  };
}
