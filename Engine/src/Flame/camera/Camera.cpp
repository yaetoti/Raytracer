#include "Camera.h"

namespace Flame {
  Camera::Camera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane)
  : m_width(width)
  , m_height(height)
  , m_fov(fov)
  , m_near(nearPlane)
  , m_far(farPlane) {
    m_position = glm::vec3(0.0f);
    m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    // TODO Setup
    m_rotation = glm::eulerAngleXYZ(0.0f, 0.0f, 0.0f);
    CalculateProjection();
    // TODO Don't need view matrix, replaced with quaternions
    // CalculateView();
    CalculateRays();
  }

  void Camera::SetRotation(glm::quat rotation) {
    m_rotation = rotation;
    // TODO Dirty flag to exclude multiple recalculations on multiple rotations
    CalculateRays();
  }

  void Camera::SetPosition(glm::vec3 position) {
    m_position = position;
    CalculateRays();
  }

  void Camera::Rotate(glm::quat rotation) {
    m_rotation *= rotation;
    CalculateRays();
  }


  Ray Camera::GetRay(uint32_t x, uint32_t y) const {
    assert(x < m_width && y < m_height);
    // TODO Start ray at near plane
    return Ray(m_position, m_directions[y * m_width + x]);
  }

  void Camera::CalculateProjection() {
    // TODO Manual creation
    m_projection = glm::perspective(
      glm::radians(m_fov),
      static_cast<float>(m_width) / static_cast<float>(m_height),
      m_near, m_far
    );
    m_iProjection = glm::inverse(m_projection);
  }

  void Camera::CalculateView() {
    // TODO Manual creation from basis
    m_view = glm::lookAt(m_position, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_iView = glm::inverse(m_view);
  }

  void Camera::CalculateRays() {
    m_directions.resize(m_width * m_height);
    for (uint32_t row = 0; row < m_height; ++row) {
      for (uint32_t col = 0; col < m_width; ++col) {
        glm::vec2 coords(
          static_cast<float>(col) / static_cast<float>(m_width),
          static_cast<float>(row) / static_cast<float>(m_height)
        );
        coords = 2.0f * coords - 1.0f;
        glm::vec4 target = m_iProjection * glm::vec4(coords, 1.0f, 1.0f);
        glm::vec3 direction(m_rotation * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
        m_directions[row * m_width + col] = direction;
      }
    }
  }
}
