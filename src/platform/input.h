#pragma once

#include <SDL.h>

namespace Platform {

class Input {
public:
    Input();

    void Update();
    bool IsKeyDown(SDL_Scancode key) const;
    bool IsKeyPressed(SDL_Scancode key) const;
    bool IsKeyReleased(SDL_Scancode key) const;

    // Mouse helpers (optional for camera debug)
    void GetMouseDelta(int& outDx, int& outDy) const;

private:
    const Uint8* m_currentState;
    Uint8 m_previousState[SDL_NUM_SCANCODES];
    int m_keyCount;
    int m_mouseDeltaX;
    int m_mouseDeltaY;
};

} // namespace Platform
