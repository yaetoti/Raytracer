#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "Flame/math/Ray.h"

namespace Flame {
  struct Camera final {
    /**
     * \param fov FOV in degrees
     */
    Camera(uint32_t width, uint32_t height, float fov, float nearPlane, float farPlane);

    void Resize(uint32_t width, uint32_t height);

    void SetRotation(glm::quat rotation);
    void SetPosition(glm::vec3 position);
    void Rotate(glm::quat rotation);

    Ray GetRay(uint32_t x, uint32_t y) const;
    Ray GetRandomizedRay(uint32_t x, uint32_t y) const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetFrontUnit() const;
    glm::vec3 GetRightUnit() const;
    glm::vec3 GetUpUnit() const;

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetInversedProjectionMatrix() const;

  private:
    void Recalculate();

  private:
    uint32_t m_width;
    uint32_t m_height;
    float m_fov;
    float m_near;
    float m_far;

    glm::vec3 m_position;
    glm::quat m_rotation;

    glm::mat4 m_projection;
    glm::mat4 m_iProjection;
    glm::mat4 m_view;
    glm::mat4 m_iView;

    glm::vec3 m_cornerTl;
    glm::vec3 m_toRightCorner;
    glm::vec3 m_toBottomCorner;
  };
}
