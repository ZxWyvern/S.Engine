# Workflow Agent: Membangun Game Engine Retro PSX-Style (Basic, Playable)

Dokumen ini adalah **workflow eksekusi** yang bisa langsung dipakai sebagai instruksi berurutan untuk coding agent (Claude Code, atau kamu sendiri manual). Setiap fase punya *Definition of Done* (DoD) yang jelas — agent tidak boleh lanjut ke fase berikutnya sebelum DoD terpenuhi.

**Target akhir:** engine minimal yang bisa render 3D dengan estetika retro PSX (low-res, vertex jitter, affine texture warping, dithering, fog), punya 1 karakter yang bisa digerakkan (WASD + kamera), collision dasar dengan lantai/dinding, dan berjalan sebagai executable standalone.

**Yang SENGAJA di-scope-out di v1:** bahasa scripting custom, editor GUI penuh, asset pipeline kompleks, multiplayer, physics engine pihak ketiga. Ini semua fase lanjutan setelah v1 solid — bikin di awal = over-scope dan proyek mati sebelum ada yang bisa dimainkan.

---

## 0. Keputusan Arsitektur (jangan skip, ini fondasi)

| Keputusan | Pilihan | Alasan |
|---|---|---|
| Bahasa core | **C++17/20** | Kontrol memory, ekosistem grafis matang, portable |
| Windowing/Input | **SDL2** | Cross-platform, ringan, API stabil, banyak referensi |
| Graphics API | **OpenGL 3.3 core** | Cukup untuk efek PSX, tooling debug matang, tidak perlu kompleksitas Vulkan untuk v1 |
| Math | **GLM** (header-only) | Standar de-facto, kompatibel GLSL |
| Build system | **CMake** | Cross-platform, dipahami semua agent/tooling |
| Scripting v1 | **Tidak ada** (hardcoded C++ logic) | Fokus dulu ke render + gameplay loop; scripting itu proyek terpisah |
| Arsitektur layer | Lihat diagram bawah | Supaya runtime tidak coupling ke tooling/editor |

```
┌───────────────────────────────┐
│  Game Layer (gameplay code)   │  ← logic spesifik game ini
├───────────────────────────────┤
│  Engine Core (Application)    │  ← game loop, scene, input dispatch
├───────────────────────────────┤
│  Systems (Domain)             │  ← Renderer, Camera, Collision, Transform
├───────────────────────────────┤
│  Platform (Infrastructure)    │  ← SDL2 window, GL context, file IO
└───────────────────────────────┘
```

Aturan: **Systems tidak boleh tahu soal Game Layer.** Game Layer memanggil Systems lewat interface publik, bukan sebaliknya. Ini yang bikin engine bisa dipakai untuk game lain nantinya tanpa refactor total.

---

## 1. Struktur Folder (agent buat struktur ini di awal)

```
psx-engine/
├── CMakeLists.txt
├── external/              # third-party (SDL2, glm, glad/glew, stb_image)
├── src/
│   ├── platform/          # window, gl context, input raw, file io
│   ├── core/              # Application, Time, Logger, EngineConfig
│   ├── renderer/          # Renderer, Shader, Mesh, Texture, RenderTarget
│   ├── scene/              # Transform, SceneNode, Camera
│   ├── gameplay/           # PlayerController, CollisionSystem
│   └── main.cpp
├── shaders/
│   ├── psx.vert
│   └── psx.frag
├── assets/
│   ├── models/
│   └── textures/
└── build/
```

**DoD Fase 1:** Project ter-generate, `cmake --build .` sukses menghasilkan executable kosong yang membuka window hitam 320x240 (internal resolution PSX-style) yang di-upscale ke window besar.

---

## 2. Fase-per-Fase

### Fase A — Platform Layer & Window
**Tugas agent:**
- Setup SDL2 window + OpenGL context (core profile 3.3)
- Buat abstraksi `Window` class: `PollEvents()`, `SwapBuffers()`, `ShouldClose()`
- Setup input raw capture (keyboard state per-frame)

**DoD:** Window terbuka, bisa ditutup dengan ESC/tombol close, tidak ada memory leak (cek dengan valgrind/ASan).

### Fase B — Render Loop Dasar
**Tugas agent:**
- Buat `RenderTarget` dengan resolusi internal rendah (contoh **320x240**, ini kunci estetika PSX)
- Render target di-blit/upscale ke window resolution pakai **nearest-neighbor filtering** (bukan linear — ini penting, linear bikin blur dan hilang karakter retro)
- Buat abstraksi `Shader` (load, compile, link, uniform setter)
- Clear screen tiap frame dengan warna solid sebagai test

**DoD:** Layar menampilkan warna solid di resolusi rendah yang terlihat blocky/pixelated saat di-upscale.

### Fase C — Mesh & Basic Rendering
**Tugas agent:**
- Buat `Mesh` class (VAO/VBO/EBO wrapper)
- Load model sederhana (mulai dari hardcoded cube vertices, baru nanti .obj loader minimal)
- Buat `Camera` (perspective projection + view matrix, kontrol posisi/rotasi)
- Render 1 cube ke layar dengan kamera

**DoD:** Cube 3D ter-render, bisa dilihat dari kamera statis, resolusi tetap 320x240 di-upscale.

### Fase D — Efek PSX di Shader (bagian paling penting untuk estetika)

Ini yang bikin "terlihat kayak PSX" secara teknis, bukan cuma resolusi rendah:

1. **Vertex Snapping / Jitter**
   Di vertex shader, snap posisi vertex ke grid diskrit setelah transform ke clip space — ini yang bikin geometry "goyang" khas PS1 karena PS1 tidak punya sub-pixel precision.
   ```glsl
   // di vertex shader, setelah gl_Position dihitung:
   vec4 snapped = gl_Position;
   snapped.xyz = snapped.xyz / snapped.w;
   float snapScale = 160.0; // sesuaikan dengan resolusi internal
   snapped.xy = floor(snapped.xy * snapScale) / snapScale;
   snapped.xyz *= snapped.w;
   gl_Position = snapped;
   ```

2. **Affine Texture Mapping (bukan perspective-correct)**
   PS1 tidak punya perspective-correct texture interpolation asli, hasilnya texture "warp" saat permukaan miring. Interpolasi UV linear di screen-space (bukan di clip-space) memberi efek ini. Cara paling praktis: interpolasi UV manual tanpa pembagian oleh `w`, atau approximate dengan mengurangi jumlah subdivisi polygon pada permukaan besar.

3. **Color Depth Reduction / Dithering**
   Quantize warna output ke depth rendah (contoh 15-bit, seperti PS1 asli) di fragment shader, lalu tambahkan ordered dithering (Bayer matrix 4x4) supaya banding tidak terlalu kasar.
   ```glsl
   float colorLevels = 32.0;
   vec3 quantized = floor(color * colorLevels) / colorLevels;
   ```

4. **Fog Jarak Pendek**
   PS1 pakai fog untuk menyembunyikan clipping plane yang dekat (keterbatasan hardware). Tambahkan fog linear berbasis depth, warna solid (bukan gradient realistis).

5. **No/Flat Lighting**
   Gunakan vertex lighting flat sederhana (Lambertian per-vertex, bukan per-pixel PBR) — ini gaya lighting era itu.

**DoD Fase D:** Cube yang sama dari Fase C sekarang terlihat "goyang" saat kamera gerak, warna banding terlihat, ada fog di kejauhan.

### Fase E — Scene Graph Minimal
**Tugas agent:**
- `Transform` (position, rotation, scale, model matrix)
- `SceneNode` dengan parent-child hierarchy sederhana (array/vector, bukan dulu ECS penuh — ECS penuh itu optimisasi, jangan premature)
- `Scene` class yang menyimpan list node dan iterasi render tiap frame

**DoD:** Bisa spawn beberapa cube dengan transform berbeda dalam satu scene, semua ter-render dengan benar.

### Fase F — Player Movement & Kamera
**Tugas agent:**
- `PlayerController`: baca input WASD, update posisi Transform player
- Kamera third-person atau first-person sederhana yang mengikuti player (pilih salah satu, third-person lebih mudah di-test visual)
- Delta-time based movement (jangan frame-rate dependent — ini bug klasik yang harus dihindari dari awal)

**DoD:** Player (bentuk cube/capsule placeholder) bisa digerakkan dengan WASD, kamera mengikuti dengan smooth, gerakan konsisten di frame rate berapa pun.

### Fase G — Collision Dasar
**Tugas agent:**
- AABB (Axis-Aligned Bounding Box) collision untuk lantai dan dinding sederhana
- Player tidak bisa menembus lantai (gravity sederhana + ground check)
- Player tidak bisa menembus dinding level

**DoD:** Player berjalan di atas lantai tanpa jatuh tembus, menabrak dinding tanpa menembus.

### Fase H — Level Minimal & Playable Loop
**Tugas agent:**
- Buat 1 level kecil: lantai + beberapa dinding + 1-2 objek dekorasi
- Tambahkan kondisi menang/kalah sederhana (contoh: sentuh objek = "win", jatuh ke void = "reset")
- Build ke executable release, test end-to-end dari start sampai selesai

**DoD FINAL v1:** Executable bisa dijalankan tanpa IDE, player bisa gerak, kamera bekerja, collision bekerja, ada tujuan sederhana, visual terlihat jelas retro-PSX (goyang, dithering, fog, resolusi rendah).

---

## 3. Checklist Milestone (agent update status di sini)

- [ ] Fase A — Window & Input
- [ ] Fase B — Render Loop + Low-Res Target
- [ ] Fase C — Mesh + Camera
- [ ] Fase D — Efek Shader PSX (jitter, affine, dither, fog)
- [ ] Fase E — Scene Graph Minimal
- [ ] Fase F — Player Movement
- [ ] Fase G — Collision
- [ ] Fase H — Level Playable

---

## 4. Risiko & Hal yang Harus Dihindari Agent

- **Jangan** mulai bikin ECS penuh, scripting language, atau editor GUI sebelum v1 di atas selesai dan playable — ini over-engineering di fase yang salah.
- **Jangan** pakai perspective-correct interpolation default OpenGL tanpa modifikasi — hasilnya terlihat "3D modern", bukan PSX.
- **Jangan** taruh logic gameplay di dalam kelas Renderer/Camera — pecah jadi Game Layer terpisah dari awal, atau refactor besar menanti nanti.
- **Jangan** hardcode delta-time atau asumsi 60 FPS — pakai `deltaTime` di semua update logic.
- Vertex jitter terlalu agresif bisa bikin geometry terlihat rusak, bukan retro — mulai dengan snap resolution rendah dulu (misal grid 160) lalu tuning visual manual.

## 5. Lanjutan Setelah v1 (fase 2, jangan dikerjakan sekarang)

Setelah checklist di atas semua selesai dan game bisa dimainkan end-to-end:
1. Asset pipeline (.obj/.gltf loader proper, texture atlas)
2. Audio system (SDL_mixer atau OpenAL, dengan sample rate rendah untuk nuansa PS1)
3. Embed scripting (Lua via LuaJIT) untuk gameplay logic yang sering berubah
4. Editor tooling minimal (Dear ImGui overlay untuk debug/inspector)
5. Save system, level format ter-serialisasi (JSON/binary custom)

---

*Cara pakai dokumen ini dengan coding agent: paste satu Fase per sesi, minta agent implementasi sampai DoD terpenuhi dan verifiable (build sukses + behavior teramati), baru lanjut ke Fase berikutnya. Jangan minta agent loncat langsung ke Fase D-H tanpa fondasi A-C selesai — 3D rendering yang salah di layer bawah akan menyebabkan bug yang sulit dilacak di layer atas.*
