#include "TextureManager.h"
#include <memory>

namespace Flame {
  std::shared_ptr<Texture> TextureManager::GetTexture(const std::wstring& path) {
    if (!m_textures.contains(path) && !LoadTexture(path)) {
      return nullptr;
    }

    return m_textures[path];
  }

  bool TextureManager::LoadTexture(const std::wstring& path) {
    auto texture = std::make_shared<Texture>();
    if (texture->InitFromFile(path.c_str())) {
      m_textures[path] = std::move(texture);
    }

    return false;
  }

  void TextureManager::Cleanup() {
    m_textures.clear();
  }

  TextureManager* TextureManager::Get() {
    static TextureManager instance;
    return &instance;
  }
}
