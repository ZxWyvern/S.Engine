#include "platform/input.h"

#include <SDL.h>

#include <cstring>

namespace Platform {

Input::Input()
    : m_mouseDeltaX(0)
    , m_mouseDeltaY(0) {
    std::memset(m_previousState, 0, sizeof(m_previousState));
    std::memset(m_currentState, 0, sizeof(m_currentState));

    // Seed current from live state at startup
    int keyCount = 0;
    const Uint8* live = SDL_GetKeyboardState(&keyCount);
    if (live != nullptr) {
        const int copyCount = keyCount < SDL_NUM_SCANCODES ? keyCount : SDL_NUM_SCANCODES;
        std::memcpy(m_currentState, live, static_cast<size_t>(copyCount));
    }
}

void Input::Update() {
    // Save owned snapshot of previous frame
    std::memcpy(m_previousState, m_currentState, sizeof(m_previousState));

    // Refresh current owned snapshot from SDL live state
    int keyCount = 0;
    const Uint8* live = SDL_GetKeyboardState(&keyCount);
    if (live != nullptr) {
        const int copyCount = keyCount < SDL_NUM_SCANCODES ? keyCount : SDL_NUM_SCANCODES;
        std::memcpy(m_currentState, live, static_cast<size_t>(copyCount));
        if (copyCount < SDL_NUM_SCANCODES) {
            std::memset(m_currentState + copyCount, 0, static_cast<size_t>(SDL_NUM_SCANCODES - copyCount));
        }
    }

    int dx = 0;
    int dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);
    m_mouseDeltaX = dx;
    m_mouseDeltaY = dy;
}

bool Input::IsKeyDown(const SDL_Scancode key) const {
    const int idx = static_cast<int>(key);
    if (idx < 0 || idx >= SDL_NUM_SCANCODES) {
        return false;
    }
    return m_currentState[idx] != 0;
}

bool Input::IsKeyPressed(const SDL_Scancode key) const {
    const int idx = static_cast<int>(key);
    if (idx < 0 || idx >= SDL_NUM_SCANCODES) {
        return false;
    }
    const bool wasDown = m_previousState[idx] != 0;
    const bool isDown = m_currentState[idx] != 0;
    return isDown && !wasDown;
}

bool Input::IsKeyReleased(const SDL_Scancode key) const {
    const int idx = static_cast<int>(key);
    if (idx < 0 || idx >= SDL_NUM_SCANCODES) {
        return false;
    }
    const bool wasDown = m_previousState[idx] != 0;
    const bool isDown = m_currentState[idx] != 0;
    return !isDown && wasDown;
}

void Input::GetMouseDelta(int& outDx, int& outDy) const {
    outDx = m_mouseDeltaX;
    outDy = m_mouseDeltaY;
}

void Input::TestSetPrevious(const SDL_Scancode key, const bool down) {
    const int idx = static_cast<int>(key);
    if (idx >= 0 && idx < SDL_NUM_SCANCODES) {
        m_previousState[idx] = down ? 1 : 0;
    }
}

void Input::TestSetCurrent(const SDL_Scancode key, const bool down) {
    const int idx = static_cast<int>(key);
    if (idx >= 0 && idx < SDL_NUM_SCANCODES) {
        m_currentState[idx] = down ? 1 : 0;
    }
}

void Input::TestSnapshotUpdate() {
    std::memcpy(m_previousState, m_currentState, sizeof(m_previousState));
}

} // namespace Platform
