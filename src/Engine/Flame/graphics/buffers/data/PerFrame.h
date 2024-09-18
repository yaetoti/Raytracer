#pragma once

#include <glm/glm.hpp>

namespace Flame {
  struct PerFrame final {
    float time;
    bool isNormalVisMode[4];
    // ImGUI
    bool diffuseEnabled[4];
    bool specularEnabled[4];
    bool iblDiffuseEnabled[4];
    bool iblSpecularEnabled[4];
    bool overwriteRoughness[4];
    float roughness;
  };
}
