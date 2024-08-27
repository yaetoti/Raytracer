#pragma once

#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_bool3.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <algorithm>
#include <ios>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

namespace Flame {
  struct PpmImage final {
    PpmImage() = default;
    PpmImage(uint32_t width, uint32_t height) {
      Resize(width, height);
    }

    void Resize(uint32_t width, uint32_t height) {
      m_width = width;
      m_height = height;
      m_framebuffer.resize(width * height);
    }

    void Fill(const glm::vec3& color) {
      std::fill(m_framebuffer.begin(), m_framebuffer.end(), color);
    }

    void Fill(float r, float g, float b) {
      Fill(glm::vec3(r, g, b));
    }

    void Set(uint32_t x, uint32_t y, const glm::vec3& color) {
      assert(x < m_width && y < m_height);
      m_framebuffer[y * m_width + x] = color;
    }

    void Set(uint32_t x, uint32_t y, float r, float g, float b) {
      Set(x, y, glm::vec3(r, g, b));
    }

    void Set(glm::uvec2 coord, const glm::vec3& color) {
      Set(coord.x, coord.y, color);
    }

    void Set(glm::uvec2 coord, float r, float g, float b) {
      Set(coord.x, coord.y, glm::vec3(r, g, b));
    }

    glm::vec3 Get(uint32_t x, uint32_t y) {
      assert(x < m_width && y < m_height);
      return m_framebuffer[y * m_width + x];
    }

    glm::vec3 Get(glm::uvec2 coord) {
      return Get(coord.x, coord.y);
    }

    void SaveToFile(const std::wstring& path, uint32_t maxColor = 255) {
      std::ofstream file(path);
      if (!file.is_open()) {
        return;
      }

      file << "P3\n";
      file << m_width << ' ' << m_height << '\n';
      file << maxColor << '\n';

      for (uint32_t row = 0; row < m_height; ++row) {
        for (uint32_t col = 0; col < m_width; ++col) {
          const glm::vec3& color = m_framebuffer[row * m_width + col];
          file
            << uint32_t(color.r * float(maxColor)) << ' '
            << uint32_t(color.g * float(maxColor)) << ' '
            << uint32_t(color.b * float(maxColor)) << '\n';
        }
      }

      file.close();
    }

    void SaveToFileBinary(const std::wstring& path, uint32_t maxColor = 255) {
      std::ofstream file(path);
      if (!file.is_open()) {
        return;
      }

      file << "P6\n";
      file << m_width << ' ' << m_height << '\n';
      file << maxColor << ' ';

      for (uint32_t row = 0; row < m_height; ++row) {
        for (uint32_t col = 0; col < m_width; ++col) {
          static_assert(sizeof(glm::vec<3, uint8_t>) == 3);
          glm::vec<3, uint8_t> color(m_framebuffer[row * m_width + col] * float(maxColor));
          file.write(reinterpret_cast<const char*>(&color), 3);
        }
      }

      file.close();
    }

    uint32_t Width() const {
      return m_width;
    }

    uint32_t Height() const {
      return m_height;
    }

  private:
    std::vector<glm::vec3> m_framebuffer;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
  };
}
