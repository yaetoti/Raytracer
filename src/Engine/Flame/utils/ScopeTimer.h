#pragma once

#include <chrono>
#include <iostream>

struct ScopeTimer final {
  ScopeTimer() {
    m_startTime = std::chrono::high_resolution_clock::now();
  }

  ~ScopeTimer() {
    m_endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Passed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(m_endTime - m_startTime).count() << " ns.\n";
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime;
};
