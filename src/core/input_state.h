#pragma once

namespace Core {

// Input abstraction so Game Layer never includes Platform headers (RULES 2.1 layer boundary)
struct InputState {
    bool forward{false};   // W
    bool backward{false};  // S
    bool left{false};      // A
    bool right{false};     // D
    bool jump{false};      // Space (pressed this frame)
    bool jumpHeld{false};  // Space held
    bool reset{false};     // R pressed this frame
};

} // namespace Core
