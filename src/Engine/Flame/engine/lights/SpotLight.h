#pragma once

#include "glm/ext.hpp"
#include "glm/fwd.hpp"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/detail/type_quat.hpp>

namespace Flame {
  struct SpotLight final {
    struct ShaderData final {
      float position[4];
      float direction[4];
      float radiance[3];
      float radius;

      float cutoffCosineInner;
      float cutoffCosineOuter;

      float padding0[2];

      glm::mat4 lightMat; // WStoVS
    };

  public:
    ShaderData ToShaderData() const {
      glm::mat lightMat = glm::mat4(
        axisRight.x, axisRight.y, axisRight.z, 0,
        axisUp.x, axisUp.y, axisUp.z, 0,
        axisFront.x, axisFront.y, axisFront.z, 0,
        position.x, position.y, position.z, 1
      );

      // glm::quat rotationQuat(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(direction));
      // glm::mat4 lightMat = glm::mat4(
      //   1, 0, 0, 0,
      //   0, 1, 0, 0,
      //   0, 0, 1, 0,
      //   position.x, position.y, position.z, 1
      // ) * glm::mat4(rotationQuat);
      
      lightMat = glm::inverse(lightMat); // Create ViewMatrix

      return ShaderData {
        { position.x, position.y, position.z, 1.0f },
        { axisFront.x, axisFront.y, axisFront.z, 0.0f },
        { radiance.x, radiance.y, radiance.z },
        radius,
        cutoffCosineInner,
        cutoffCosineOuter,
        { 0, 0 },
        lightMat
      };
    }

  public:
    glm::vec3 position;
    glm::vec3 axisFront;
    glm::vec3 axisUp;
    glm::vec3 axisRight;
    glm::vec3 radiance;
    float radius;

    float cutoffCosineInner;
    float cutoffCosineOuter;
  };
}
