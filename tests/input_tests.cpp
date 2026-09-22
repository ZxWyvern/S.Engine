#include "platform/input.h"

#include <SDL.h>

extern void Check(bool, const char*, const char*, int, const char*);
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_MSG(expr, msg) Check((expr), #expr, __FILE__, __LINE__, msg)

void RunInputTests() {
    // Press edge: previous=false, current=true
    {
        Platform::Input input;
        input.TestSetPrevious(SDL_SCANCODE_SPACE, false);
        input.TestSetCurrent(SDL_SCANCODE_SPACE, true);
        CHECK_MSG(input.IsKeyPressed(SDL_SCANCODE_SPACE), "Up->Down Pressed must be true");
        CHECK_MSG(input.IsKeyDown(SDL_SCANCODE_SPACE), "Up->Down Held must be true");
        CHECK_MSG(!input.IsKeyReleased(SDL_SCANCODE_SPACE), "Up->Down Released must be false");
    }

    // Held state: previous=true, current=true  (simulate two frames of hold)
    {
        Platform::Input input;
        input.TestSetPrevious(SDL_SCANCODE_SPACE, true);
        input.TestSetCurrent(SDL_SCANCODE_SPACE, true);
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_SPACE), "Down->Down Pressed must be false");
        CHECK_MSG(input.IsKeyDown(SDL_SCANCODE_SPACE), "Down->Down Held must be true");
        CHECK_MSG(!input.IsKeyReleased(SDL_SCANCODE_SPACE), "Down->Down Released must be false");
    }

    // Release edge: previous=true, current=false
    {
        Platform::Input input;
        input.TestSetPrevious(SDL_SCANCODE_SPACE, true);
        input.TestSetCurrent(SDL_SCANCODE_SPACE, false);
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_SPACE), "Down->Up Pressed must be false");
        CHECK_MSG(!input.IsKeyDown(SDL_SCANCODE_SPACE), "Down->Up Held must be false");
        CHECK_MSG(input.IsKeyReleased(SDL_SCANCODE_SPACE), "Down->Up Released must be true");
    }

    // Jump uses Pressed — verify Space press edge is distinct from hold
    {
        Platform::Input input;
        input.TestSetPrevious(SDL_SCANCODE_SPACE, false);
        input.TestSetCurrent(SDL_SCANCODE_SPACE, false);
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_SPACE), "Up->Up should not trigger Pressed");
        // Now press
        input.TestSnapshotUpdate(); // previous = current (false)
        input.TestSetCurrent(SDL_SCANCODE_SPACE, true);
        CHECK_MSG(input.IsKeyPressed(SDL_SCANCODE_SPACE), "Press should be detected after snapshot");
        // Hold without snapshot should keep Pressed true until snapshot is taken
        CHECK_MSG(input.IsKeyPressed(SDL_SCANCODE_SPACE), "Held frame without snapshot still Pressed");
        input.TestSnapshotUpdate();
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_SPACE), "After snapshot, held should not be Pressed");
        CHECK_MSG(input.IsKeyDown(SDL_SCANCODE_SPACE), "But still Held");
    }

    // Reset (R) edge
    {
        Platform::Input input;
        input.TestSetPrevious(SDL_SCANCODE_R, false);
        input.TestSetCurrent(SDL_SCANCODE_R, true);
        CHECK_MSG(input.IsKeyPressed(SDL_SCANCODE_R), "R Up->Down must be Pressed");
        input.TestSnapshotUpdate();
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_R), "R held second frame must not be Pressed");
        CHECK_MSG(input.IsKeyDown(SDL_SCANCODE_R), "R held must be Down");
        input.TestSetCurrent(SDL_SCANCODE_R, false);
        CHECK_MSG(input.IsKeyReleased(SDL_SCANCODE_R), "R release must be Released");
    }

    // Test helper snapshot behavior invariants
    {
        Platform::Input input;
        input.TestSetCurrent(SDL_SCANCODE_A, true);
        input.TestSnapshotUpdate();
        // After snapshot, previous == current, so Pressed must be false (no edge)
        CHECK_MSG(!input.IsKeyPressed(SDL_SCANCODE_A), "Snapshot edge requires new change");
        CHECK_MSG(input.IsKeyDown(SDL_SCANCODE_A), "Still down");
    }
}
