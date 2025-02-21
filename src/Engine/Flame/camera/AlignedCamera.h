#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "Flame/math/Ray.h"
#include "Flame/engine/Transform.h"

namespace Flame {
  struct AlignedCamera final {
    /**
     * \param fov FOV in degrees
     */
    AlignedCamera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane);

    void Resize(uint32_t width, uint32_t height);
    void Rotate(float pitch, float yaw);
    void SetPosition(const glm::vec3& position);

    const glm::vec3& GetPosition() const;
    const glm::quat& GetRotationQuat() const;
    glm::mat4 GetRotationMat() const;

    Ray GetRay(uint32_t x, uint32_t y) const;
    Ray GetRandomizedRay(uint32_t x, uint32_t y) const;
    glm::vec3 GetFrontUnit() const;
    glm::vec3 GetRightUnit() const;
    glm::vec3 GetUpUnit() const;

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetInversedProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetInversedViewMatrix() const;

    glm::vec3 GetToFrustumTlNear() const;
    glm::vec3 GetToFrustumBlNear() const;
    glm::vec3 GetToFrustumBrNear() const;

    /**
     * @return Array of frustum points in the following order: Near-Far, TL-RT-BR-BL
     */
    std::array<glm::vec4, 8> GetFrustumCornersWS() const;

    glm::vec4 ClipToWorld(glm::vec4 position) const;

  private:
    void InvalidateMatrices() const;

  private:
    // Viewport
    uint32_t m_width;
    uint32_t m_height;
    float m_fov;
    float m_near;
    float m_far;

    // Position
    glm::vec3 m_position;
    glm::quat m_rotation;

    // Matrices
    mutable glm::mat4 m_projection;
    mutable glm::mat4 m_iProjection;
    mutable glm::mat4 m_view;
    mutable glm::mat4 m_iView;
    mutable bool m_matricesDirty;

    // Near plane points and vectors
    mutable glm::vec3 m_cornerTl;
    mutable glm::vec3 m_cornerBl;
    mutable glm::vec3 m_cornerBr;
    mutable glm::vec3 m_toCornerTl;
    mutable glm::vec3 m_toCornerBl;
    mutable glm::vec3 m_toCornerBr;
    mutable glm::vec3 m_toRightCorner;
    mutable glm::vec3 m_toBottomCorner;
  };
}
