#include "platform/input.h"

#include <cstring>

namespace Platform {

Input::Input()
    : m_currentState(nullptr)
    , m_keyCount(SDL_NUM_SCANCODES)
    , m_mouseDeltaX(0)
    , m_mouseDeltaY(0) {
    std::memset(m_previousState, 0, sizeof(m_previousState));
    m_currentState = SDL_GetKeyboardState(&m_keyCount);
}

void Input::Update() {
    if (m_currentState != nullptr) {
        const int copyCount = m_keyCount < SDL_NUM_SCANCODES ? m_keyCount : SDL_NUM_SCANCODES;
        std::memcpy(m_previousState, m_currentState, static_cast<size_t>(copyCount));
    }
    // Pump events already done via Window::PollEvents + SDL_Pump handled by SDL_PollEvent
    // Still query fresh keyboard state
    m_currentState = SDL_GetKeyboardState(&m_keyCount);

    int dx = 0;
    int dy = 0;
    // Use relative mode query via SDL_GetRelativeMouseState if needed
    SDL_GetRelativeMouseState(&dx, &dy);
    m_mouseDeltaX = dx;
    m_mouseDeltaY = dy;
}

bool Input::IsKeyDown(const SDL_Scancode key) const {
    if (m_currentState == nullptr) {
        return false;
    }
    if (static_cast<int>(key) >= m_keyCount) {
        return false;
    }
    return m_currentState[key] != 0;
}

bool Input::IsKeyPressed(const SDL_Scancode key) const {
    if (m_currentState == nullptr) {
        return false;
    }
    if (static_cast<int>(key) >= m_keyCount) {
        return false;
    }
    const bool wasDown = m_previousState[key] != 0;
    const bool isDown = m_currentState[key] != 0;
    return isDown && !wasDown;
}

bool Input::IsKeyReleased(const SDL_Scancode key) const {
    if (m_currentState == nullptr) {
        return false;
    }
    if (static_cast<int>(key) >= m_keyCount) {
        return false;
    }
    const bool wasDown = m_previousState[key] != 0;
    const bool isDown = m_currentState[key] != 0;
    return !isDown && wasDown;
}

void Input::GetMouseDelta(int& outDx, int& outDy) const {
    outDx = m_mouseDeltaX;
    outDy = m_mouseDeltaY;
}

} // namespace Platform
