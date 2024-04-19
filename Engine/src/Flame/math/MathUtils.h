#pragma once

#include <random>
#include <glm/glm.hpp>

struct MathUtils final {
  template<glm::length_t L = 3, typename T, glm::qualifier Q = glm::defaultp>
  static glm::vec<L, T, Q> RandomUnitVector(std::mt19937_64& gen, std::uniform_real_distribution<T>& dist) {
    return glm::normalize(2 * glm::vec<L, T, Q>(dist(gen), dist(gen), dist(gen)) - glm::vec<L, T, Q>(1));
  }
};
