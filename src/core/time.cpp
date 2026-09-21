#include "core/time.h"

#include <SDL.h>

namespace Core {

Time::Time()
    : m_lastTicksMs(SDL_GetTicks64())
    , m_deltaTime(0.0f)
    , m_totalTime(0.0f)
    , m_frameCount(0) {
}

void Time::Tick() {
    const uint64_t current = SDL_GetTicks64();
    float delta = static_cast<float>(current - m_lastTicksMs) / 1000.0f;
    delta = GetClampedDeltaTime(delta);
    m_deltaTime = delta;
    m_totalTime += delta;
    m_lastTicksMs = current;
    ++m_frameCount;
}

float Time::GetClampedDeltaTime(const float deltaTime) {
    return deltaTime > kMaxDeltaTime ? kMaxDeltaTime : deltaTime;
}

} // namespace Core
