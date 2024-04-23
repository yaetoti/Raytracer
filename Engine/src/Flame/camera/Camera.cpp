#include "Camera.h"

#include "Flame/utils/Random.h"

namespace Flame {
  Camera::Camera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane)
  : m_width(width)
  , m_height(height)
  , m_fov(fov)
  , m_near(nearPlane)
  , m_far(farPlane) {
    m_position = glm::vec3(0.0f);
    // TODO Setup
    m_rotation = glm::inverse(glm::eulerAngleXYZ(0.0f, glm::radians(0.0f), 0.0f));
    CalculateProjection();
    CalculateRays();
  }

  void Camera::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    CalculateProjection();
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

  glm::vec3 Camera::GetPosition() const {
    return m_position;
  }

  void Camera::Rotate(glm::quat rotation) {
    //::vec3 angles = glm::eulerAngles(m_rotation) - glm::eulerAngles(rotation);
    //m_rotation = glm::normalize(glm::quat(glm::eulerAngleZXY(angles.z, angles.x, angles.y)));
    m_rotation = glm::normalize(m_rotation * rotation);
    CalculateRays();
  }

  glm::vec3 Camera::GetFrontUnit() const {
    // TODO Cache or something
    glm::mat4 rotation(m_rotation);
    // TODO Why inverted ?!?
    return -glm::vec3(rotation[2]);
  }

  glm::vec3 Camera::GetRightUnit() const {
    // TODO Cache or something
    glm::mat4 rotation(m_rotation);
    return glm::vec3(rotation[0]);
  }

  glm::vec3 Camera::GetUpUnit() const {
    // TODO Cache or something
    glm::mat4 rotation(m_rotation);
    return glm::vec3(rotation[1]);
  }

  glm::mat4 Camera::GetProjectionMatrix() const {
    return m_projection;
  }

  glm::mat4 Camera::GetInversedProjectionMatrix() const {
    return m_iProjection;
  }

  //Ray Camera::GetRay(uint32_t x, uint32_t y) const {
  //  assert(x < m_width && y < m_height);
  //  // TODO Start ray at near plane
  //  return Ray(m_position, m_directions[y * m_width + x]);
  //}

  Ray Camera::GetRay(uint32_t x, uint32_t y) const {
    assert(x < m_width && y < m_height);
    // TODO Start ray at near plane
    glm::vec2 coords(
      (static_cast<float>(x) + Random::Float()) / static_cast<float>(m_width),
      (static_cast<float>(y) + Random::Float()) / static_cast<float>(m_height)
    );
    coords = 2.0f * coords - 1.0f;
    glm::vec4 target = m_iProjection * glm::vec4(coords, 1.0f, 1.0f);
    glm::vec3 direction(m_rotation * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
    return Ray(m_position, direction);
  }

  void Camera::CalculateProjection() {
    // TODO Manual creation
    m_projection = glm::perspective(
      glm::radians(m_fov),
      static_cast<float>(m_width) / static_cast<float>(m_height),
      m_far, m_near
    );
    m_iProjection = glm::inverse(m_projection);
  }

  void Camera::CalculateRays() {
#if 0
      m_directions.resize(m_width * m_height);
    for (uint32_t row = 0; row < m_height; ++row) {
      for (uint32_t col = 0; col < m_width; ++col) {
        glm::vec2 coords(
          (static_cast<float>(col) + Random::Float()) / static_cast<float>(m_width),
          (static_cast<float>(row) + Random::Float()) / static_cast<float>(m_height)
        );
        coords = 2.0f * coords - 1.0f;
        glm::vec4 target = m_iProjection * glm::vec4(coords, 1.0f, 1.0f);
        glm::vec3 direction(m_rotation * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
        m_directions[row * m_width + col] = direction;
      }
    }
#endif
  }
}
