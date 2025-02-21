#include "TextureManager.h"
#include <memory>
#include <DirectXTex/DirectXTex.h>
#include <Flame/graphics/DxContext.h>

namespace Flame {
  std::shared_ptr<Texture> TextureManager::GetTexture(const std::wstring& path) {
    if (m_textures.contains(path) || LoadTexture(path)) {
      return m_textures[path];
    }

    return nullptr;
  }

  bool TextureManager::LoadTexture(const std::wstring& path) {
    auto texture = std::make_shared<Texture>();
    if (texture->InitFromFile(path.c_str())) {
      m_textures[path] = std::move(texture);
      return true;
    }

    return false;
  }

  void TextureManager::SaveToDDS(const std::wstring& filename, ID3D11Resource* texture, DXGI_FORMAT format, bool generateMips, bool convert, DXGI_FORMAT convertTo) {
    DirectX::ScratchImage scratchImage;
    HRESULT result;
    result = DirectX::CaptureTexture(
      DxContext::Get()->d3d11Device.Get(),
      DxContext::Get()->d3d11DeviceContext.Get(),
      texture,
      scratchImage
    );
    assert(SUCCEEDED(result));

    const DirectX::ScratchImage* imagePtr = &scratchImage;

    DirectX::ScratchImage converted;
    if (convert) {
      result = DirectX::Convert(*imagePtr->GetImage(0, 0, 0), convertTo, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
      assert(SUCCEEDED(result));
      imagePtr = &converted;
    }

    DirectX::ScratchImage mipchain;
    if (generateMips) {
      result = DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipchain);
      assert(SUCCEEDED(result));
      imagePtr = &mipchain;
    }

    DirectX::ScratchImage compressed;
    if (DirectX::IsCompressed(DXGI_FORMAT(format))) {
      if (DXGI_FORMAT_BC6H_UF16 <= format && format <= DXGI_FORMAT_BC7_UNORM_SRGB) {
        result = DirectX::Compress(
          DxContext::Get()->d3d11Device.Get(),
          imagePtr->GetImages(),
          imagePtr->GetImageCount(),
          imagePtr->GetMetadata(),
          DXGI_FORMAT(format),
          DirectX::TEX_COMPRESS_PARALLEL,
          1.f,
          compressed
        );
      }
      else {
        result = DirectX::Compress(
          imagePtr->GetImages(),
          imagePtr->GetImageCount(),
          imagePtr->GetMetadata(),
          format,
          DirectX::TEX_COMPRESS_PARALLEL,
          1.f,
          compressed
        );
      }

      assert(SUCCEEDED(result));
      imagePtr = &compressed;
    }

    result = DirectX::SaveToDDSFile(
      imagePtr->GetImages(),
      imagePtr->GetImageCount(),
      imagePtr->GetMetadata(),
      DirectX::DDS_FLAGS(0),
      filename.c_str()
    );
    assert(SUCCEEDED(result));
  }

  void TextureManager::Cleanup() {
    m_textures.clear();
  }

  TextureManager* TextureManager::Get() {
    static TextureManager instance;
    return &instance;
  }
}
