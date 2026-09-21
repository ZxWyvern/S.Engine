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

    void GetMouseDelta(int& outDx, int& outDy) const;

    // Test helpers — allow direct control of owned snapshots without SDL
    void TestSetPrevious(SDL_Scancode key, bool down);
    void TestSetCurrent(SDL_Scancode key, bool down);
    void TestSnapshotUpdate();

private:
    Uint8 m_currentState[SDL_NUM_SCANCODES];
    Uint8 m_previousState[SDL_NUM_SCANCODES];
    int m_mouseDeltaX;
    int m_mouseDeltaY;
};

} // namespace Platform
