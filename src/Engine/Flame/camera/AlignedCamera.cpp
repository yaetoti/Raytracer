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
    return glm::vec3(GetRotationMat()[2]);
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

  glm::vec3 AlignedCamera::GetToFrustumTlNear() const {
    InvalidateMatrices();
    return m_toCornerTl;
  }

  glm::vec3 AlignedCamera::GetToFrustumBlNear() const {
    InvalidateMatrices();
    return m_toCornerBl;
  }

  glm::vec3 AlignedCamera::GetToFrustumBrNear() const {
    InvalidateMatrices();
    return m_toCornerBr;
  }

  std::array<glm::vec4, 8> AlignedCamera::GetFrustumCornersWS() const {
    InvalidateMatrices();
    glm::mat4 transformInv = m_iView * m_iProjection;

    std::array<glm::vec4, 8> result;
    result[0] = transformInv * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    result[1] = transformInv * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    result[2] = transformInv * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
    result[3] = transformInv * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
    result[4] = transformInv * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
    result[5] = transformInv * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    result[6] = transformInv * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
    result[7] = transformInv * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
    for (uint32_t i = 0; i < 8; ++i) {
      result[i] /= result[i].w;
    }

    return result;
  }

  glm::vec4 AlignedCamera::ClipToWorld(glm::vec4 position) const {
    InvalidateMatrices();
    position = m_iView * (m_iProjection * position);
    return position / position.w;
  }

  Ray AlignedCamera::GetRay(uint32_t x, uint32_t y) const {
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
    // m_projection = glm::perspectiveFovRH_ZO(
    //   glm::radians(m_fov),
    //   static_cast<float>(m_width), static_cast<float>(m_height),
    //   m_far, m_near
    // );
    m_iProjection = glm::inverse(m_projection);
    m_iView = glm::translate(m_position) * GetRotationMat();
    m_view = glm::inverse(m_iView);

    // Near
    // TopLeft
    glm::vec4 target = m_iView * (m_iProjection * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f));
    m_cornerTl = glm::vec3(target) / target.w;
    // BottomLeft
    target = m_iView * (m_iProjection * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f));
    m_cornerBl = glm::vec3(target) / target.w;
    // BottomRight
    target = m_iView * (m_iProjection * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f));
    m_cornerBr = glm::vec3(target) / target.w;

    m_toRightCorner = m_cornerBr - m_cornerBl;
    m_toBottomCorner = m_cornerBl - m_cornerTl;

    m_toCornerTl = m_cornerTl - m_position;
    m_toCornerBl = m_cornerBl - m_position;
    m_toCornerBr = m_cornerBr - m_position;

    m_matricesDirty = false;
  }
}
