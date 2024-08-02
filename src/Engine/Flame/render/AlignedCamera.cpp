#include "AlignedCamera.h"

#include <iostream>

#include "Flame/math/MathUtils.h"
#include "Flame/utils/Random.h"

namespace Flame {
  AlignedCamera::AlignedCamera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane)
  : m_width(width)
  , m_height(height)
  , m_fov(fov)
  , m_near(nearPlane)
  , m_far(farPlane)
  , m_position(0.0f)
  , m_rotation(0.0f, 0.0f, 0.0f, 0.0f)
  , m_matricesDirty(true) {
  }

  void AlignedCamera::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_matricesDirty = true;
  }

  void AlignedCamera::Rotate(float pitch, float yaw) {
    glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), GetRightUnit());
    glm::quat yawQuat = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    m_rotation = glm::normalize(yawQuat * pitchQuat * m_rotation);
    m_matricesDirty = true;
  }

  void AlignedCamera::SetPosition(const glm::vec3& position) {
    m_position = position;
    m_matricesDirty = true;
  }

  const glm::vec3& AlignedCamera::GetPosition() const {
    return m_position;
  }

  const glm::quat& AlignedCamera::GetRotationQuat() const {
    return m_rotation;
  }

  glm::mat4 AlignedCamera::GetRotationMat() const {
    return glm::mat4(m_rotation);
  }

  glm::vec3 AlignedCamera::GetFrontUnit() const {
    InvalidateMatrices();
    // Inversion is related to the OpenGL world coordinate system (camera looks at -Z)
    return -glm::vec3(GetRotationMat()[2]);
  }

  glm::vec3 AlignedCamera::GetRightUnit() const {
    InvalidateMatrices();
    return glm::vec3(GetRotationMat()[0]);
  }

  glm::vec3 AlignedCamera::GetUpUnit() const {
    InvalidateMatrices();
    return glm::vec3(GetRotationMat()[1]);
  }

  glm::mat4 AlignedCamera::GetProjectionMatrix() const {
    InvalidateMatrices();
    return m_projection;
  }

  glm::mat4 AlignedCamera::GetInversedProjectionMatrix() const {
    InvalidateMatrices();
    return m_iProjection;
  }

  glm::mat4 AlignedCamera::GetViewMatrix() const {
    InvalidateMatrices();
    return m_view;
  }

  glm::mat4 AlignedCamera::GetInversedViewMatrix() const {
    InvalidateMatrices();
    return m_iView;
  }

  Ray AlignedCamera::GetRay(uint32_t x, uint32_t y) const {
    // TODO assert failed
    assert(x < m_width && y < m_height);

    InvalidateMatrices();

    glm::vec2 coords(
      (static_cast<float>(x) + 0.5f) / static_cast<float>(m_width),
      (static_cast<float>(y) + 0.5f) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_position, 0.0f));
    return Ray(m_position, glm::normalize(direction));
  }

  Ray AlignedCamera::GetRandomizedRay(uint32_t x, uint32_t y) const {
    // Cache is inapplicable due to the need to get fuzzified direction for each ray in sample (for antialiasing)
    assert(x < m_width && y < m_height);

    InvalidateMatrices();

    glm::vec2 coords(
      (static_cast<float>(x) + Random::Float()) / static_cast<float>(m_width),
      (static_cast<float>(y) + Random::Float()) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_position, 0.0f));
    return Ray(m_position, glm::normalize(direction));
  }

  void AlignedCamera::InvalidateMatrices() const {
    if (!m_matricesDirty) {
      return;
    }

    m_projection = MathUtils::Perspective(
      glm::radians(m_fov),
      static_cast<float>(m_width) / static_cast<float>(m_height),
      m_near, m_far
    );
    m_iProjection = glm::inverse(m_projection);
    m_iView = glm::translate(m_position) * GetRotationMat();
    m_view = glm::inverse(m_iView);

    glm::vec4 target = m_iView * m_iProjection * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    m_cornerTl = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 cornerTr = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
    glm::vec3 cornerBl = glm::vec3(target) / target.w;

    m_toRightCorner = cornerTr - m_cornerTl;
    m_toBottomCorner = cornerBl - m_cornerTl;

    m_matricesDirty = false;
  }
}
