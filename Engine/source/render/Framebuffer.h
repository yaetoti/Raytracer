#pragma once

#include <vector>
#include <cassert>
#include <windef.h>
#include "../math/Vector.h"

struct Framebuffer final {
    Framebuffer(size_t width, size_t height)
    : m_buffer(width * height * 4), m_width(width), m_height(height) {
    }

    void Resize(size_t width, size_t height) {
        m_buffer.resize(width * height * 4);
        m_width = width;
        m_height = height;
    }

    void SetPixel(size_t x, size_t y, BYTE r, BYTE g, BYTE b) {
        assert(x < m_width && y < m_height);
        m_buffer[(y * m_width + x) * 4 + 0] = b;
        m_buffer[(y * m_width + x) * 4 + 1] = g;
        m_buffer[(y * m_width + x) * 4 + 2] = r;
    }

    Vec3I GetPixel(size_t x, size_t y) {
        assert(x < m_width && y < m_height);
        return Vec3I(
            m_buffer[(y * m_width + x) * 4 + 2],
            m_buffer[(y * m_width + x) * 4 + 1],
            m_buffer[(y * m_width + x) * 4 + 0]
        );
    }

    // Getters
    const BYTE* GetData() const {
        return m_buffer.data();
    }

    size_t GetWidth() const {
        return m_width;
    }

    size_t GetHeight() const {
        return m_height;
    }

private:
    std::vector<BYTE> m_buffer;
    size_t m_width;
    size_t m_height;
};
