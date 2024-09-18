#include "SpaceshipCamera.h"

#include "Flame/math/MathUtils.h"
#include "Flame/utils/Random.h"

namespace Flame {
  SpaceshipCamera::SpaceshipCamera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane)
  : m_width(width)
  , m_height(height)
  , m_fov(fov)
  , m_near(nearPlane)
  , m_far(farPlane)
  , m_matricesDirty(true) {
  }

  void SpaceshipCamera::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_matricesDirty = true;
  }

  void SpaceshipCamera::Rotate(float pitch, float yaw, float roll) {
    m_transform.Rotate(pitch, yaw, roll);
    m_matricesDirty = true;
  }

  void SpaceshipCamera::SetPosition(const glm::vec3& position) {
    m_transform.SetPosition(position);
    m_matricesDirty = true;
  }

  const Transform& SpaceshipCamera::GetTransform() const {
    return m_transform;
  }

  const glm::vec3& SpaceshipCamera::GetPosition() const {
    return m_transform.GetPosition();
  }

  glm::vec3 SpaceshipCamera::GetFrontUnit() const {
    InvalidateMatrices();
    // Inversion is related to the OpenGL world coordinate system (camera looks at -Z)
    return -glm::vec3(m_transform.GetRotationMat()[2]);
  }

  glm::vec3 SpaceshipCamera::GetRightUnit() const {
    InvalidateMatrices();
    return glm::vec3(m_transform.GetRotationMat()[0]);
  }

  glm::vec3 SpaceshipCamera::GetUpUnit() const {
    InvalidateMatrices();
    return glm::vec3(m_transform.GetRotationMat()[1]);
  }

  glm::mat4 SpaceshipCamera::GetProjectionMatrix() const {
    InvalidateMatrices();
    return m_projection;
  }

  glm::mat4 SpaceshipCamera::GetInversedProjectionMatrix() const {
    InvalidateMatrices();
    return m_iProjection;
  }

  glm::mat4 SpaceshipCamera::GetViewMatrix() const {
    InvalidateMatrices();
    return m_view;
  }

  glm::mat4 SpaceshipCamera::GetInversedViewMatrix() const {
    InvalidateMatrices();
    return m_iView;
  }

  Ray SpaceshipCamera::GetRay(uint32_t x, uint32_t y) const {
    assert(x < m_width && y < m_height);

    InvalidateMatrices();

    glm::vec2 coords(
      (static_cast<float>(x) + 0.5f) / static_cast<float>(m_width),
      (static_cast<float>(y) + 0.5f) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_transform.GetPosition(), 0.0f));
    return Ray(m_transform.GetPosition(), glm::normalize(direction));
  }

  Ray SpaceshipCamera::GetRandomizedRay(uint32_t x, uint32_t y) const {
    // Cache is inapplicable due to the need to get fuzzified direction for each ray in sample (for antialiasing)
    assert(x < m_width && y < m_height);

    InvalidateMatrices();

    glm::vec2 coords(
      (static_cast<float>(x) + Random::Float()) / static_cast<float>(m_width),
      (static_cast<float>(y) + Random::Float()) / static_cast<float>(m_height)
    );

    glm::vec3 target = m_cornerTl + m_toRightCorner * coords.x + m_toBottomCorner * coords.y;
    glm::vec3 direction(glm::vec4(target - m_transform.GetPosition(), 0.0f));
    return Ray(m_transform.GetPosition(), glm::normalize(direction));
  }

  void SpaceshipCamera::InvalidateMatrices() const {
    if (!m_matricesDirty) {
      return;
    }

    //m_projection = MathUtils::Perspective(
    //  glm::radians(m_fov),
    //  static_cast<float>(m_width) / static_cast<float>(m_height),
    //  m_near, m_far
    //);
    m_projection = glm::perspective(
      glm::radians(m_fov),
      static_cast<float>(m_width) / static_cast<float>(m_height),
      m_near, m_far
    );
    m_iProjection = glm::inverse(m_projection);
    //m_iView = MathUtils::Translate(m_transform.GetPosition()) * glm::mat4(m_transform.GetRotationMat());
    m_iView = m_transform.GetMat();
    m_view = glm::inverse(m_iView);

    glm::vec4 target = m_iView * m_iProjection * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
    m_cornerTl = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
    glm::vec3 cornerTr = glm::vec3(target) / target.w;

    target = m_iView * m_iProjection * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 cornerBl = glm::vec3(target) / target.w;

    m_toRightCorner = cornerTr - m_cornerTl;
    m_toBottomCorner = cornerBl - m_cornerTl;

    m_matricesDirty = false;
  }
}
