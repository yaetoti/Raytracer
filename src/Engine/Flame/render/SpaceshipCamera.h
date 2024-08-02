#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "Flame/math/Ray.h"
#include "Flame/components/Transform.h"

namespace Flame {
  struct SpaceshipCamera final {
    /**
     * \param fov FOV in degrees
     */
    SpaceshipCamera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane);

    void Resize(uint32_t width, uint32_t height);
    void Rotate(float pitch, float yaw, float roll);
    void SetPosition(const glm::vec3& position);

    const Transform& GetTransform() const;
    const glm::vec3& GetPosition() const;

    Ray GetRay(uint32_t x, uint32_t y) const;
    Ray GetRandomizedRay(uint32_t x, uint32_t y) const;
    glm::vec3 GetFrontUnit() const;
    glm::vec3 GetRightUnit() const;
    glm::vec3 GetUpUnit() const;

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetInversedProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetInversedViewMatrix() const;

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
    Transform m_transform;

    // Matrices
    mutable glm::mat4 m_projection;
    mutable glm::mat4 m_iProjection;
    mutable glm::mat4 m_view;
    mutable glm::mat4 m_iView;
    mutable bool m_matricesDirty;

    // Near plane vectors
    mutable glm::vec3 m_cornerTl;
    mutable glm::vec3 m_toRightCorner;
    mutable glm::vec3 m_toBottomCorner;
  };
}
