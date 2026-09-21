# S.Engine — PSX Retro Engine

> Minimal C++17 3D engine with authentic PS1 look: 320×240, vertex jitter, affine warp, dithering, short fog. Playable v1 — no scripting, no full ECS, no third-party physics.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![SDL2](https://img.shields.io/badge/SDL2-2.30-green)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%20core-red)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-orange)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

## Demo

> Add `docs/demo.gif` (960×720 window, PSX jitter visible). Capture: run `build/psx-engine`, move with WASD.

| Low-res 320×240 | Upscaled nearest | PSX shader |
|---|---|---|
| FBO `GL_NEAREST` | No blur | snap 160 + Bayer 4×4 |

## Features (Fase D)

- **Vertex Snapping** `floor(pos.xy*160)/160` after `w` divide — `src/renderer/renderer.h:36 kSnapScale`
- **Affine Mapping** `noperspective` UV — screen-space linear, PS1 warp
- **Color Quantize + Dithering** `uColorLevels=32` + Bayer 4×4 `shaders/psx.frag`
- **Short Linear Fog** `uFogNear=8 / uFogFar=22`
- **Flat Lambertian** per-vertex, no PBR

Plus: 320×240 `RenderTarget` with nearest-neighbor blit, third-person camera with `exp(-k*dt)` smoothing, AABB collision + gravity, win/void conditions.

## Quick Start

### Prerequisites

- CMake ≥3.16, MinGW-w64 GCC 13+ or MSVC 2022, Git
- Windows: `winget install BrechtSanders.WinLibs` (or MSYS2). Linux: `sudo apt install libsdl2-dev libgl1-mesa-dev`

External deps are vendored (or auto-fetched if `external/` empty):

- `external/SDL` — SDL2 2.30.8
- `external/glm` — GLM 1.0.1
- `external/glad` — GL 3.3 core (generated via `pip install glad` + `python -m glad --profile core --out-path external/glad --api gl=3.3 --generator c`)

### Build & Run

```bash
# path has space — always quote "S.Engine"
cmake -S "S.Engine" -B "S.Engine/build" -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
cmake --build "S.Engine/build" -j4
"S.Engine/build/psx-engine.exe"          # Windows
# Linux: ./build/psx-engine
# Shaders auto-copied to build/shaders/ via POST_BUILD
```

> First configure clones SDL2+glm if `external/` empty and generates GLAD. Build is warning-free (`-Wall -Wextra -Wpedantic`).

## Controls

| Key | Action |
|---|---|
| W/A/S/D | Move (camera-relative, delta-time) |
| Space | Jump (`kDefaultJumpSpeed=6`, `kGravity=18`) |
| R | Reset level |
| ESC / Window X | Quit |
| Touch yellow cube at (7,7) | **Win** — `Level::CheckWinCondition()` |
| Y < -10 | **Void** — auto reset |

## Architecture

```
Game Layer  →  Engine Core (Application, IGame, InputState)  →  Systems  →  Platform
               gameplay/game.h  core/i_game.h                  renderer/    platform/
                                                        scene/    window.h
                                                      collision   input.h, logger.h
```

- **Layer rule** `docs/RULES.md:2.1` — `Systems → Platform only`, `Renderer/Scene` never include `gameplay/`. Decoupling via `Core::IGame` + `Core::InputState` (`src/core/i_game.h`, `input_state.h`) — `Application` owns `Scene`, `Game` is injected in `src/main.cpp`.
- **Naming** `RULES.md:3.1` — `PascalCase` class/method, `m_` members, `kPascal` constants, `snake_case` files.
- **RAII** — VAO/VBO/Shader `Destroy()` in dtor, `unique_ptr` ownership, no `new/delete`, no alloc in hot loop (`Scene::GetAllNodesInto()` + `reserve()`).
- **Error handling** — shader/file failures log + safe fallback, `assert` only for invariants.

## Project Structure

```
S.Engine/
├── CMakeLists.txt          # SDL options OFF, glad static, glm interface, copy shaders
├── external/{SDL,glm,glad} # vendored (SDL built as shared)
├── src/
│   ├── platform/{window,input,logger}
│   ├── core/{application,i_game,input_state,logger,time,engine_config}
│   ├── renderer/{renderer,render_target,shader,mesh}
│   ├── scene/{scene,scene_node,transform,camera}
│   ├── gameplay/{game,level,player_controller,collision_system}
│   └── main.cpp            # injects Gameplay::Game via IGame
├── shaders/{psx.vert,psx.frag}
├── assets/{models,textures} # reserved
└── build/                  # out-of-source (ignored)
```

## Shaders

| Uniform | Value | Meaning |
|---|---|---|
| `uSnapScale` | 160 | Vertex jitter grid |
| `uColorLevels` | 32 | 15-bit quantize |
| `uFogNear/Far` | 8 / 22 | Short PS1 fog |
| `uFogColor` | 0.08,0.08,0.10 | Fog solid color |
| `uLightDir` | -0.4,-1,-0.3 | Flat Lambert dir |

All uniforms named — no magic numbers in shaders (`RULES.md:2.2`).

## Roadmap

- [x] Fase A Window & Input
- [x] Fase B RenderTarget 320×240 nearest
- [x] Fase C Mesh + Camera
- [x] Fase D PSX Shaders
- [x] Fase E Scene Graph
- [x] Fase F Player + Camera follow
- [x] Fase G Collision (AABB + gravity)
- [x] Fase H Playable Level (floor + walls + goal)
- [ ] Fase 2 — Data-driven scenes (JSON), ImGui editor overlay, audio

`workflow-game-engine-psx.md:182` explicitly scopes out editor/scripting before v1 — now unblocked. Next: `Dear ImGui` overlay editor (see plan in discussion).

## Contributing

PRs must be warning-free, no leak (ASan), behavior observed, layer intact, no bare `TODO` (`// TODO(debt): reason — plan`) per `RULES.md:6`. Commit: `[Fase-X] Imperative`.

## License

MIT — add `LICENSE` if publishing.

## Acknowledgments

PSX jitter formula `docs/workflow-game-engine-psx.md:97`, Bayer matrix, [GLM](https://github.com/g-truc/glm), [SDL2](https://github.com/libsdl-org/SDL), [GLAD](https://github.com/Dav1dde/glad).
