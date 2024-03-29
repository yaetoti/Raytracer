#pragma once
#include <chrono>

struct FpsTimer final {
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    FpsTimer();

    double Tick();
    double GetElapsedTime() const;
    TimePoint GetLastTimePoint() const;
    TimePoint GetCurrentTimePoint() const;

private:
    TimePoint m_lastTime;
    TimePoint m_currentTime;
    double m_elapsedTime;
};
