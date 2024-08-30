#pragma once

#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Flame {
  struct TextureManager final {
    enum class FileFormat {
      NONE,
      PNG,
      TGA,
      HDR,
      BC1_LINEAR = DXGI_FORMAT_BC1_UNORM,     // RGB, 1 bit Alpha
      BC1_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,  // RGB, 1-bit Alpha, SRGB
      BC3_LINEAR = DXGI_FORMAT_BC3_UNORM,     // RGBA
      BC3_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,  // RGBA, SRGB
      BC4_UNSIGNED = DXGI_FORMAT_BC4_UNORM,   // GRAY, unsigned
      BC4_SIGNED = DXGI_FORMAT_BC4_SNORM,     // GRAY, signed
      BC5_UNSIGNED = DXGI_FORMAT_BC5_UNORM,   // RG, unsigned
      BC5_SIGNED = DXGI_FORMAT_BC5_SNORM,     // RG, signed
      BC6_UNSIGNED = DXGI_FORMAT_BC6H_UF16,   // RGB HDR, unsigned
      BC6_SIGNED = DXGI_FORMAT_BC6H_SF16,     // RGB HDR, signed
      BC7_LINEAR = DXGI_FORMAT_BC7_UNORM,     // RGBA Advanced
      BC7_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,  // RGBA Advanced, SRGB
    };

    std::shared_ptr<Texture> GetTexture(const std::wstring& path);
    bool LoadTexture(const std::wstring& path);
    static void SaveToDDS(const std::wstring& filename, ID3D11Resource* texture, FileFormat format, bool generateMips);

    void Cleanup();

    static TextureManager* Get();

  private:
    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;
  };
}
