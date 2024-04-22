#include "Camera.h"

namespace Flame {
  Camera::Camera(size_t width, size_t height, float fov, float near, float far) {
    m_bufferWidth = width;
    m_bufferHeight = height;
    m_position = glm::vec3(0);
  }

  const Ray& Camera::GetRay(size_t x, size_t y) {
    return m_rays[y * m_bufferWidth + x];
  }

  void Camera::GenerateRays() {
    m_projection = glm::perspective(
      glm::radians(90.0f),
      static_cast<float>(m_bufferWidth) / static_cast<float>(m_bufferHeight),
      0.1f, 1000.0f
    );
    m_iProjection = glm::inverse(m_projection);
    m_view = glm::lookAt(m_position, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_iView = glm::inverse(m_view);

    m_rays.resize(m_bufferWidth * m_bufferHeight);
    for (size_t row = 0; row < m_bufferHeight; ++row) {
      for (size_t col = 0; col < m_bufferWidth; ++col) {
        glm::vec2 coords(
          static_cast<float>(col) / static_cast<float>(m_bufferWidth),
          static_cast<float>(row) / static_cast<float>(m_bufferHeight)
        );
        coords = 2.0f * coords - 1.0f;
        glm::vec4 target = m_iProjection * glm::vec4(coords, 1.0f, 1.0f);
        glm::vec3 direction(m_iView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
        m_rays[row * m_bufferWidth + col] = Ray(m_position, direction);
      }
    }
  }

  void Camera::HandleEvent(const WindowEvent& e) {
  }
}
