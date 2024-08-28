#include "Texture.h"
#include "Flame/graphics/DxContext.h"
#include "Flame/utils/DDSTextureLoader11.h"
#include <cassert>
#include <winerror.h>

namespace Flame {
  bool Texture::InitFromFile(const wchar_t* path) {
    HRESULT result = DirectX::CreateDDSTextureFromFile(
      DxContext::Get()->d3d11Device.Get(),
      path,
      m_resource.GetAddressOf(),
      m_resourceView.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    return SUCCEEDED(result);
  }

  void Texture::Reset() {
    m_resourceView.Reset();
    m_resource.Reset();
  }

  ID3D11Resource* Texture::GetResource() const {
    return m_resource.Get();
  }

  ID3D11ShaderResourceView* Texture::GetResourceView() const {
    return m_resourceView.Get();
  }

  ID3D11Resource* const* Texture::GetResourceAddress() const {
    return m_resource.GetAddressOf();
  }

  ID3D11ShaderResourceView* const* Texture::GetResourceViewAddress() const {
    return m_resourceView.GetAddressOf();
  }
}
