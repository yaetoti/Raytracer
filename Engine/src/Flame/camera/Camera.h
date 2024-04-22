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

    void SetRotation(glm::quat rotation);
    void SetPosition(glm::vec3 position);
    void Rotate(glm::quat rotation);

    Ray GetRay(uint32_t x, uint32_t y) const;

  private:
    void CalculateProjection();
    void CalculateView();
    void CalculateRays();

  private:
    uint32_t m_width;
    uint32_t m_height;
    float m_fov;
    float m_near;
    float m_far;

    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::quat m_rotation;

    glm::mat4 m_projection;
    glm::mat4 m_iProjection;
    glm::mat4 m_view;
    glm::mat4 m_iView;

    std::vector<glm::vec3> m_directions;
  };  
}
