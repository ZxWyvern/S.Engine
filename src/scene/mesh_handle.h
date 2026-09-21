#pragma once

#include <cstdint>

namespace Scene {

// Neutral handle to a GPU mesh owned by Renderer.
// Scene stores only this handle — no concrete Renderer types.
struct MeshHandle {
    uint32_t id{0};

    bool IsValid() const { return id != 0; }
    bool operator==(const MeshHandle& other) const { return id == other.id; }
    bool operator!=(const MeshHandle& other) const { return id != other.id; }
};

constexpr MeshHandle kInvalidMeshHandle{0};

} // namespace Scene
