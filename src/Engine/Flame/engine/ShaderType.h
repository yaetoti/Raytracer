#pragma once
#include <type_traits>

namespace Flame {
  enum class ShaderType {
    NONE             = 0,
    VERTEX_SHADER    = 1 << 0,
    HULL_SHADER      = 1 << 1,
    DOMAIN_SHADER    = 1 << 2,
    GEOMETRY_SHADER  = 1 << 3,
    PIXEL_SHADER     = 1 << 4,
    ALL              = VERTEX_SHADER | HULL_SHADER | DOMAIN_SHADER | GEOMETRY_SHADER | PIXEL_SHADER,
  };

  inline ShaderType operator|(ShaderType lhs, ShaderType rhs) {
    using T = std::underlying_type_t<ShaderType>;
    return static_cast<ShaderType>(static_cast<T>(lhs) | static_cast<T>(rhs));
  }

  inline ShaderType operator&(ShaderType lhs, ShaderType rhs) {
    using T = std::underlying_type_t<ShaderType>;
    return static_cast<ShaderType>(static_cast<T>(lhs) & static_cast<T>(rhs));
  }

  inline ShaderType operator^(ShaderType lhs, ShaderType rhs) {
    using T = std::underlying_type_t<ShaderType>;
    return static_cast<ShaderType>(static_cast<T>(lhs) ^ static_cast<T>(rhs));
  }

  inline ShaderType operator~(ShaderType type) {
    using T = std::underlying_type_t<ShaderType>;
    return static_cast<ShaderType>(~static_cast<T>(type));
  }

  inline ShaderType& operator|=(ShaderType& lhs, ShaderType rhs) {
    lhs = lhs | rhs;
    return lhs;
  }

  inline ShaderType& operator&=(ShaderType& lhs, ShaderType rhs) {
    lhs = lhs & rhs;
    return lhs;
  }

  inline ShaderType& operator^=(ShaderType& lhs, ShaderType rhs) {
    lhs = lhs ^ rhs;
    return lhs;
  }

  inline bool Any(ShaderType type) {
    return type != ShaderType::NONE;
  }

  inline bool IsSet(ShaderType var, ShaderType flag) {
    return Any(var & flag);
  }
}
