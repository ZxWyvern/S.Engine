# AGENTS.md — S.Engine (PSX Retro Engine)

> Greenfield repo — only `docs/` exists. No `CMakeLists.txt`, `src/`, or build system yet. Agent scaffolds structure per `docs/workflow-game-engine-psx.md` §1 before coding.

## Source of Truth

- **Rules (how to code):** `docs/RULES.md` — layer boundaries, RAII, naming, performance invariants. Wins over speed.
- **Workflow (what to build):** `docs/workflow-game-engine-psx.md` — 8 phased milestones A–H. Do not skip phases; each DoD must be verified before next.
- **Global harness:** `~/.config/opencode/opencode.json` provides Context7 MCP (`resolve-library-id` → `query-docs`) and `9router` provider. Use Context7 for SDL2 / OpenGL 3.3 / GLM / CMake docs. No repo-local `opencode.json`.

## Stack (locked in workflow §0)

C++17/20 · SDL2 (window/input) · OpenGL 3.3 core · GLM (header-only) · CMake · No scripting in v1 (hardcoded C++). `external/` for SDL2, glm, glad/glew, stb_image. Target internal res **320×240**, upscaled with **nearest-neighbor** (not linear).

## Expected Layout (create if missing)

```
CMakeLists.txt | external/ | src/{platform,core,renderer,scene,gameplay,main.cpp}
shaders/{psx.vert,psx.frag} | assets/{models,textures} | build/
```
Reference `docs/workflow-game-engine-psx.md:38-59` for exact structure.

## Architecture — Layer Boundaries (non-negotiable, `docs/RULES.md:2.1`)

```
Game Layer → Engine Core & Systems
Engine Core → Systems & Platform
Systems → Platform only
Platform → nothing above
```

- Violation = architecture bug, fix before merge. Systems/Renderer/Camera/Collision must not know about `Player` or gameplay types — use interfaces/callbacks/events.
- No circular includes. No `renderer/` → `gameplay/` include.

## C++ Conventions Agents Get Wrong Here

- **Naming:** `PascalCase` class/method, `camelCase` locals, `m_` private members, `kPascalCase` constants (`kInternalResolutionWidth`), `snake_case` files matching primary class — `docs/RULES.md:3.1`.
- **Ownership explicit:** `unique_ptr` default, `shared_ptr` only with justification in PR. No bare `new`/`delete`. GPU handles (VAO/VBO/Texture/Shader) must be RAII wrappers releasing in destructor.
- **No alloc in hot loop:** `new`/`malloc`/`vector::push_back` realloc forbidden in per-frame render/update loops — pre-allocate/pool and `reserve()` at init.
- **Const-correctness:** params not mutated → `const`; non-mutating methods → `const`.
- **Error handling:** file/shader load failures log clearly + safe fallback; never silent, never corrupt state. `assert` only for invariants, not external input validation.
- **Naming side effects:** `GetX()` must not mutate; mutators use verbs (`UpdateX`, `ApplyX`).
- Naming shader constants: no raw magic numbers for internal res/snap grid/color levels — use named uniforms/constants.

## Render Gotchas (PSX look, workflow Fase D)

- Upscale via nearest-neighbor is intentional — linear blurs and kills retro feel.
- Vertex snap after clip-space divide: `floor(pos.xy * 160.0)/160.0` range, tune visually (`docs/workflow-game-engine-psx.md:97-107`). Too aggressive = broken geometry.
- Affine mapping = linear UV in screen-space (no `w` divide) to get warp; don't rely on default perspective-correct interpolation.
- Color quantize + Bayer 4×4 dithering, short linear fog, flat Lambertian per-vertex lighting. `colorLevels` etc must be named constants.

## Build & Verify (expected — no CMake yet)

```bash
cmake -S . -B build
cmake --build build          # must be warning-free; no new warnings allowed
./build/psx-engine           # Windows: build/Debug/psx-engine.exe — path has space, quote "S.Engine"
```
Per `docs/RULES.md:6` DoD for every phase: build clean, no leak (ASan/valgrind when touching resources), behavior **observed** not assumed — describe manual verification, layer boundary intact, no bare `TODO` (use `// TODO(debt): reason — plan`).

## How to Work — Phase Order

Execute one phase per session: **A Window → B RenderTarget → C Mesh/Camera → D PSX Shaders → E SceneGraph → F Player/Camera → G Collision → H Playable Level**. Do not jump to D–H without A–C. Checklist at `docs/workflow-game-engine-psx.md:161-170`.

- Delta-time mandatory for all movement — never assume 60 FPS.
- Cache per-frame uniforms that don't change (e.g., internal res); no verbose `std::cout` in release render loop.
- Document new draw calls / allocations / GPU resources in PR description.

## Commits & PRs

- Format: `[Fase-X] Imperative description` e.g. `[Fase-D] Tambah vertex snapping di shader PSX` (`docs/RULES.md:5`).
- One logical change per commit.
- PRs touching render/memory must state perf + memory impact.

## Do NOT

- Singleton `Instance()`, God `Engine` class, >2-level inheritance without written justification, hidden side effects.
- Full ECS, custom scripting, GUI editor, asset pipeline, multiplayer, or third-party physics before v1 playable (workflow §4–5) — premature and will block v1.
- Only three rules have **zero** exceptions: memory safety, layer dependency direction, delta-time correctness (`docs/RULES.md:7`).
