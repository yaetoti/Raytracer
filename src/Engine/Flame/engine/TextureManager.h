#pragma once

#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Flame {
  struct TextureManager final {
    std::shared_ptr<Texture> GetTexture(const std::wstring& path);
    bool LoadTexture(const std::wstring& path);
    static void SaveToDDS(const std::wstring& filename, ID3D11Resource* texture, DXGI_FORMAT format, bool generateMips = false, bool convert = false, DXGI_FORMAT convertTo = DXGI_FORMAT_R8G8B8A8_UNORM);

    void Cleanup();

    static TextureManager* Get();

  private:
    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;
  };
}
