#include "Framebuffer.h"
#include <cassert>

namespace Flame {
  Framebuffer::Framebuffer()
  : m_width(0)
  , m_height(0) {
  }

  Framebuffer::Framebuffer(uint32_t width, uint32_t height)
  : m_buffer(width * height * 4)
  , m_width(width)
  , m_height(height) {
  }

  void Framebuffer::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_buffer.resize(width * height * 4);
  }

  void Framebuffer::SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    assert(x < m_width && y < m_height);
    m_buffer[(y * m_width + x) * 4 + 0] = b;
    m_buffer[(y * m_width + x) * 4 + 1] = g;
    m_buffer[(y * m_width + x) * 4 + 2] = r;
    // 4th component is used for alignment in BitBlit()
  }

  glm::ivec3 Framebuffer::GetPixel(uint32_t x, uint32_t y) const {
    assert(x < m_width && y < m_height);
    return glm::ivec3(
      m_buffer.at((y * m_width + x) * 4 + 2),
      m_buffer.at((y * m_width + x) * 4 + 1),
      m_buffer.at((y * m_width + x) * 4 + 0)
    );
  }

  const uint8_t* Framebuffer::GetData() const {
    return m_buffer.data();
  }

  uint32_t Framebuffer::GetWidth() const {
    return m_width;
  }

  uint32_t Framebuffer::GetHeight() const {
    return m_height;
  }
}
