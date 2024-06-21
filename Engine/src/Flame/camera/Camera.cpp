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
    m_rotation = glm::inverse(glm::eulerAngleYXZ(0.0f, 0.0f, 0.0f));
    Recalculate();
  }

  void Camera::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    Recalculate();
  }

  void Camera::SetRoll(float roll) {
    m_roll = roll;
    m_rotation = glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), glm::radians(m_roll));
    Recalculate();
  }

  void Camera::SetPitch(float pitch) {
    m_pitch = pitch;
    m_rotation = glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), glm::radians(m_roll));
    Recalculate();
  }

  void Camera::SetYaw(float yaw) {
    m_yaw = yaw;
    m_rotation = glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), glm::radians(m_roll));
    Recalculate();
  }

  void Camera::SetRotation(glm::quat rotation) {
    m_rotation = rotation;
    Recalculate();
  }

  void Camera::SetPosition(glm::vec3 position) {
    m_position = position;
    Recalculate();
  }

  glm::vec3 Camera::GetPosition() const {
    return m_position;
  }

  void Camera::Rotate(float roll, float pitch, float yaw) {
    m_roll += roll;
    m_pitch += pitch;
    m_yaw += yaw;
    m_rotation = glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), glm::radians(m_roll));
    Recalculate();
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

  Ray Camera::GetRay(uint32_t x, uint32_t y) const {
    assert(x < m_width && y < m_height);
    // TODO Start ray at near plane
    glm::vec2 coords(
      (static_cast<float>(x) + 0.5f) / static_cast<float>(m_width),
      (static_cast<float>(y) + 0.5f) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_position, 0.0f));
    return Ray(m_position, glm::normalize(direction));
  }

  Ray Camera::GetRandomizedRay(uint32_t x, uint32_t y) const {
    // Cache is inapplicable due to the need to get fuzzified direction for each ray in sample (for antialiasing)
    assert(x < m_width && y < m_height);
    glm::vec2 coords(
      (static_cast<float>(x) + Random::Float()) / static_cast<float>(m_width),
      (static_cast<float>(y) + Random::Float()) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_position, 0.0f));
    return Ray(m_position, glm::normalize(direction));
  }

  void Camera::Recalculate() {
    // TODO Manual creation
    m_projection = glm::perspective(
      glm::radians(m_fov),
      static_cast<float>(m_width) / static_cast<float>(m_height),
      m_far, m_near
    );
    m_iProjection = glm::inverse(m_projection);
    m_iView = glm::translate(m_position) * glm::mat4(m_rotation);
    m_view = glm::inverse(m_view);

    glm::vec4 target = m_iView * m_iProjection * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
    m_cornerTl = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
    glm::vec3 cornerTr = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 cornerBl = glm::vec3(target) / target.w;

    m_toRightCorner = cornerTr - m_cornerTl;
    m_toBottomCorner = cornerBl - m_cornerTl;
  }
}
