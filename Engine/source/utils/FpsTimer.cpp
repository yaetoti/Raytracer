#include "FpsTimer.h"

FpsTimer::FpsTimer() : m_lastTime(Clock::now())
, m_currentTime(Clock::now())
, m_elapsedTime(0.0f) {

}

double FpsTimer::Tick() {
    m_lastTime = m_currentTime;
    m_currentTime = Clock::now();
    m_elapsedTime = Duration(m_currentTime - m_lastTime).count();
    return m_elapsedTime;
}

double FpsTimer::GetElapsedTime() const {
    return m_elapsedTime;
}

FpsTimer::TimePoint FpsTimer::GetLastTimePoint() const {
    return m_lastTime;
}

FpsTimer::TimePoint FpsTimer::GetCurrentTimePoint() const {
    return m_currentTime;
}
