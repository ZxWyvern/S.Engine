#pragma once

#include "scene/mesh_handle.h"

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

namespace Renderer {

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
    glm::vec2 texCoord{0.0f};
    glm::vec3 color{1.0f};
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void SetVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Bind() const;
    void Unbind() const;
    void Draw() const;

    bool IsValid() const { return m_vao != 0; }
    size_t GetIndexCount() const { return m_indexCount; }

    static Mesh CreateCube(float size = 1.0f);
    static Mesh CreatePlane(float width, float depth);

private:
    void Destroy();

    GLuint m_vao{0};
    GLuint m_vbo{0};
    GLuint m_ebo{0};
    size_t m_indexCount{0};
};

// GPU mesh store — owns all Mesh objects and maps neutral handles.
// Renderer owns this; Scene only ever sees MeshHandle.
class MeshRegistry {
public:
    MeshRegistry() = default;
    ~MeshRegistry() = default;

    Scene::MeshHandle CreateCube(float size = 1.0f);
    Scene::MeshHandle CreatePlane(float width, float depth);
    Scene::MeshHandle AddMesh(Mesh mesh);

    const Mesh* Get(Scene::MeshHandle handle) const;
    void Clear();

private:
    uint32_t m_nextId{1};
    std::unordered_map<uint32_t, Mesh> m_meshes;
};

} // namespace Renderer
