#pragma once

#include <chrono>
#include <random>
#include <glm/glm.hpp>

namespace Flame {
  struct Random final {
    static float Float() {
      return m_dist(m_gen);
    }

    template<glm::length_t L = 3, typename T, glm::qualifier Q = glm::defaultp>
    static glm::vec<L, T, Q> UnitVector() {
      return glm::normalize(2 * glm::vec<L, T, Q>(m_dist(m_gen), m_dist(m_gen), m_dist(m_gen)) - glm::vec<L, T, Q>(1));
    }

    template<glm::length_t L = 3, typename T, glm::qualifier Q = glm::defaultp>
    static glm::vec<L, T, Q> UnnormalizedVector() {
      return 2 * glm::vec<L, T, Q>(m_dist(m_gen), m_dist(m_gen), m_dist(m_gen)) - glm::vec<L, T, Q>(1);
    }
  
  private:
    inline static thread_local std::mt19937_64 m_gen = std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    inline static std::uniform_real_distribution<float> m_dist;
  };
}
