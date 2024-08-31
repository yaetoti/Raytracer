#pragma once

#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Flame {
  struct TextureManager final {
    std::shared_ptr<Texture> GetTexture(const std::wstring& path);
    bool LoadTexture(const std::wstring& path);
    void Cleanup();

    static TextureManager* Get();

  private:
    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;
  };
}
