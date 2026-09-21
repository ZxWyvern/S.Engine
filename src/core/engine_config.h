#pragma once

namespace Core {

struct EngineConfig {
    static constexpr int kInternalResolutionWidth = 320;
    static constexpr int kInternalResolutionHeight = 240;
    static constexpr int kWindowWidth = 960;
    static constexpr int kWindowHeight = 720;
    static constexpr const char* kWindowTitle = "S.Engine - PSX Retro";
    static constexpr float kTargetFps = 60.0f;
};

} // namespace Core
