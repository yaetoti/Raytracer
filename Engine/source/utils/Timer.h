#pragma once
#include <chrono>

struct Timer final {
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::duration<double>;

  Timer()
  : m_lastTime(Clock::now())
  , m_currentTime(Clock::now())
  , m_elapsedTime(0.0f) {
  }

  double Tick() {
    m_lastTime = m_currentTime;
    m_currentTime = Clock::now();
    m_elapsedTime = Duration(m_currentTime - m_lastTime).count();
    return m_elapsedTime;
  }

  double GetElapsedTime() const {
    return m_elapsedTime;
  }

  TimePoint GetLastTimePoint() const {
    return m_lastTime;
  }

  TimePoint GetCurrentTimePoint() const {
    return m_currentTime;
  }

private:
  TimePoint m_lastTime;
  TimePoint m_currentTime;
  double m_elapsedTime;
};
