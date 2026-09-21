# RULES.md — Aturan Wajib untuk Agent yang Mengerjakan PSX Engine

Dokumen ini adalah **kontrak kerja** untuk coding agent (atau manusia) yang mengerjakan proyek ini. Setiap Pull Request / commit harus patuh pada aturan di sini. Kalau ada konflik antara "cepat selesai" vs "aturan di bawah", **aturan di bawah menang**, kecuali dinyatakan eksplisit sebagai pengecualian sementara di kode (`// TODO(debt):`).

---

## 1. Prinsip Inti (non-negotiable)

1. **Correctness dulu, baru speed.** Kode yang cepat tapi salah tidak lolos.
2. **Readability > cleverness.** Kalau butuh komentar panjang untuk jelasin trik pintar, tulis ulang yang lebih sederhana.
3. **Tidak ada "tutorial-grade code".** Semua kode yang masuk harus production-minded: ada error handling, tidak ada magic number tanpa nama, tidak ada asumsi silent.
4. **Jangan optimasi prematur.** Optimasi hanya boleh dilakukan setelah ada bukti (profiling) bahwa itu bottleneck. Kekecualian: hal yang oleh sifatnya harus benar dari awal (delta-time, cache-friendly data layout untuk hot loop yang JELAS akan jadi hot loop, misalnya render loop per-frame).
5. **Preserve behavior saat refactor**, kecuali task eksplisit minta ubah behavior.

---

## 2. Aturan Arsitektur

### 2.1 Layer Boundary (WAJIB)
```
Game Layer      → boleh depend ke Engine Core & Systems
Engine Core     → boleh depend ke Systems & Platform
Systems         → boleh depend ke Platform SAJA
Platform        → tidak boleh depend ke layer manapun di atasnya
```
- **Dependency arah harus satu jalur ke bawah.** Kalau `Systems` butuh sesuatu dari `Game Layer`, itu tanda desain salah → gunakan interface/callback/event, bukan include langsung.
- Renderer, Camera, Collision **tidak boleh tahu** apa itu "Player" atau logic gameplay spesifik. Mereka generic.

### 2.2 Larangan Eksplisit
- ❌ **Singleton abuse** — kalau butuh akses global, lewat dependency injection (constructor parameter atau service locator terbatas), bukan `static Instance()` di mana-mana.
- ❌ **God Object** — kelas `Engine` atau `Game` yang punya method untuk semua hal (render, input, audio, physics sekaligus) dilarang. Pecah per tanggung jawab.
- ❌ **Circular dependency** antar module/folder. Kalau `renderer/` include header dari `gameplay/`, itu bug arsitektur, harus difix sebelum merge.
- ❌ **Hidden side effects** — fungsi bernama `GetX()` tidak boleh mengubah state. Fungsi yang mengubah state harus punya nama verb yang jelas (`UpdateX()`, `ApplyX()`).
- ❌ **Magic numbers** tanpa named constant, khususnya di shader (resolusi internal, snap grid, color levels — semua harus jadi named constant/uniform, bukan angka mentah tersembunyi).

### 2.3 Composition over Inheritance
- Hierarki class dalam (>2 level inheritance) harus dijustifikasi tertulis di PR description. Default: komposisi + interface kecil.

---

## 3. Standar Kode C++

### 3.1 Naming Convention
| Elemen | Konvensi | Contoh |
|---|---|---|
| Class/Struct | PascalCase | `RenderTarget`, `PlayerController` |
| Fungsi/Method | PascalCase | `UpdateTransform()` |
| Variabel lokal | camelCase | `deltaTime` |
| Member variable privat | `m_` prefix + camelCase | `m_position` |
| Konstanta | `k` prefix + PascalCase | `kInternalResolutionWidth` |
| File | snake_case, match nama class utama | `render_target.h/.cpp` |

### 3.2 Memory & Resource Management
- **RAII wajib.** Tidak ada `new`/`delete` manual tanpa smart pointer atau wrapper RAII yang jelas kepemilikannya.
- **Ownership harus eksplisit** di tipe: `std::unique_ptr` untuk single ownership, `std::shared_ptr` hanya kalau memang perlu shared lifetime (dan harus dijustifikasi, bukan default).
- GPU resource (VAO/VBO/Texture/Shader) wajib dibungkus RAII wrapper yang release di destructor — tidak boleh ada leak GPU handle.
- Tidak boleh ada allocation (`new`, `malloc`, `std::vector::push_back` yang re-alloc) di dalam **hot loop per-frame** (render loop, update loop) tanpa pre-allocated buffer/pool. Kalau perlu, reserve kapasitas di init.

### 3.3 Error Handling
- Tidak ada silent failure. File loading gagal, shader compile gagal, dsb → log jelas + fallback aman (bukan crash tanpa info, bukan diam-diam lanjut dengan state korup).
- Assertion untuk invariant yang harus selalu benar (`assert(mesh.vertexCount > 0)`), bukan untuk validasi input user/file eksternal (itu pakai proper error handling, bukan assert).

### 3.4 Const-correctness
- Semua parameter yang tidak dimodifikasi harus `const`. Semua method yang tidak mengubah state class harus ditandai `const`.

---

## 4. Aturan Performance (khusus render/game loop)

- **Delta-time based, selalu.** Tidak ada logic yang berasumsi frame rate tetap.
- Draw call per objek dihitung sadar — kalau jumlah objek di scene bertambah dan draw call linear naik tanpa batching, itu harus dicatat sebagai technical debt (`// TODO(debt): batch draw calls`).
- Shader uniform update per-frame yang tidak berubah (misalnya resolusi internal) harus di-cache, tidak di-upload ulang tiap frame tanpa alasan.
- Tidak boleh ada `std::cout`/logging verbose di dalam render loop per-frame pada build release.

---

## 5. Struktur Commit & PR

- Satu commit = satu perubahan logis (jangan campur "fix bug X" dengan "refactor Y" dalam satu commit).
- Commit message format: `[Fase-X] Deskripsi singkat imperative` — contoh: `[Fase-D] Tambah vertex snapping di shader PSX`.
- Setiap PR yang menyentuh render loop atau memory management **wajib** menyebutkan di deskripsi: dampak ke performance (alokasi baru? draw call baru?) dan dampak ke memory (resource baru yang perlu di-release?).

---

## 6. Definition of Done — Checklist Wajib per Fase

Sebelum fase dianggap selesai, semua ini harus ✅:
- [ ] Build sukses tanpa warning (treat warning as visible, minimal tidak ada warning baru yang muncul)
- [ ] Tidak ada memory leak terverifikasi (ASan/valgrind untuk fase yang menyentuh resource management)
- [ ] Behavior yang diklaim di DoD workflow **teramati langsung** (bukan "seharusnya jalan"), jelaskan cara verifikasi manual
- [ ] Tidak ada pelanggaran layer boundary (lihat §2.1)
- [ ] Tidak ada TODO tanpa label (`// TODO(debt): alasan` jika memang utang teknis disengaja)

---

## 7. Kapan Boleh Melanggar Aturan Ini

Boleh, **hanya** dengan syarat:
1. Ditulis eksplisit di kode sebagai `// TODO(debt): <alasan> — <rencana fix>`
2. Dicatat di PR description
3. Tidak menyentuh: memory safety, layer boundary arah dependency, atau delta-time correctness — tiga hal ini **tidak ada pengecualian**, karena bug di sini menyebar ke seluruh sistem dan mahal diperbaiki belakangan.

---

*Pasangkan file ini dengan `workflow-game-engine-psx.md`. Workflow itu menentukan APA yang dikerjakan tiap fase; file ini menentukan BAGAIMANA cara mengerjakannya supaya tidak jadi utang teknis yang menumpuk.*
