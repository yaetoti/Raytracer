#pragma once

#include <cassert>
#include <vector>
#include <Windows.h>
#include <glm/glm.hpp>

namespace Flame {
  struct RenderSurface final {
    RenderSurface()
    : m_width(0)
    , m_height(0) {
    }

    RenderSurface(uint32_t width, uint32_t height)
    : m_buffer(width * height * 4)
    , m_width(width)
    , m_height(height) {
    }

    void Resize(uint32_t width, uint32_t height) {
      m_buffer.resize(width * height * 4);
      m_width = width;
      m_height = height;
    }

    void SetPixel(uint32_t x, uint32_t y, BYTE r, BYTE g, BYTE b) {
      assert(x < m_width && y < m_height);
      m_buffer[(y * m_width + x) * 4 + 0] = b;
      m_buffer[(y * m_width + x) * 4 + 1] = g;
      m_buffer[(y * m_width + x) * 4 + 2] = r;
    }

    glm::ivec3 GetPixel(uint32_t x, uint32_t y) {
      assert(x < m_width && y < m_height);
      return glm::ivec3(
        m_buffer[(y * m_width + x) * 4 + 2],
        m_buffer[(y * m_width + x) * 4 + 1],
        m_buffer[(y * m_width + x) * 4 + 0]
      );
    }

    // Getters
    const BYTE* GetData() const {
      return m_buffer.data();
    }

    uint32_t GetWidth() const {
      return m_width;
    }

    uint32_t GetHeight() const {
      return m_height;
    }

  private:
    std::vector<BYTE> m_buffer;
    uint32_t m_width;
    uint32_t m_height;
  };
}
