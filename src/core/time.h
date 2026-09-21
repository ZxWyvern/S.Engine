#pragma once

#include <cstdint>

namespace Core {

class Time {
public:
    Time();

    void Tick();
    float GetDeltaTime() const { return m_deltaTime; }
    float GetTotalTime() const { return m_totalTime; }
    uint64_t GetFrameCount() const { return m_frameCount; }

    static float GetClampedDeltaTime(float deltaTime);

private:
    uint64_t m_lastTicksMs;
    float m_deltaTime;
    float m_totalTime;
    uint64_t m_frameCount;

    static constexpr float kMaxDeltaTime = 0.05f;
};

} // namespace Core
