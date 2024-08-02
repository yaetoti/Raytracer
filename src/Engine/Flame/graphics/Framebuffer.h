#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Flame {
  struct Framebuffer final {
    Framebuffer();
    Framebuffer(uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);
    void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);

    glm::ivec3 GetPixel(uint32_t x, uint32_t y) const;
    const uint8_t* GetData() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

  private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_width;
    uint32_t m_height;
  };
}
