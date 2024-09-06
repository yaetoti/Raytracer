#pragma once

#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Flame {
  struct TextureManager final {
    std::shared_ptr<Texture> GetTexture(const std::wstring& path);
    bool LoadTexture(const std::wstring& path);
    static void SaveToDDS(const std::wstring& filename, ID3D11Resource* texture, DXGI_FORMAT format, bool generateMips);

    void Cleanup();

    static TextureManager* Get();

  private:
    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;
  };
}
