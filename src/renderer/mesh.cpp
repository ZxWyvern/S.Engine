#include "renderer/mesh.h"

#include <cstddef>

namespace Renderer {

Mesh::~Mesh() {
    Destroy();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_indexCount(other.m_indexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Destroy();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_indexCount = other.m_indexCount;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::Destroy() {
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    m_indexCount = 0;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    Destroy();

    m_indexCount = indices.size();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, color)));

    glBindVertexArray(0);
}

void Mesh::Bind() const {
    glBindVertexArray(m_vao);
}

void Mesh::Unbind() const {
    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (m_vao == 0 || m_indexCount == 0) {
        return;
    }
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh Mesh::CreateCube(const float size) {
    const float h = size * 0.5f;

    std::vector<Vertex> vertices;
    vertices.reserve(24);

    auto pushFace = [&](glm::vec3 n, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 col) {
        vertices.push_back({v0, n, {0.0f, 0.0f}, col});
        vertices.push_back({v1, n, {1.0f, 0.0f}, col});
        vertices.push_back({v2, n, {1.0f, 1.0f}, col});
        vertices.push_back({v3, n, {0.0f, 1.0f}, col});
    };

    pushFace({0, 0, 1},  {-h,-h, h}, { h,-h, h}, { h, h, h}, {-h, h, h}, {1.0f, 0.2f, 0.2f});
    pushFace({0, 0,-1},  { h,-h,-h}, {-h,-h,-h}, {-h, h,-h}, { h, h,-h}, {0.2f, 1.0f, 0.2f});
    pushFace({-1, 0, 0}, {-h,-h,-h}, {-h,-h, h}, {-h, h, h}, {-h, h,-h}, {0.2f, 0.2f, 1.0f});
    pushFace({ 1, 0, 0}, { h,-h, h}, { h,-h,-h}, { h, h,-h}, { h, h, h}, {1.0f, 1.0f, 0.2f});
    pushFace({0, 1, 0},  {-h, h, h}, { h, h, h}, { h, h,-h}, {-h, h,-h}, {1.0f, 0.2f, 1.0f});
    pushFace({0,-1, 0},  {-h,-h,-h}, { h,-h,-h}, { h,-h, h}, {-h,-h, h}, {0.2f, 1.0f, 1.0f});

    std::vector<unsigned int> indices;
    indices.reserve(36);
    for (unsigned int i = 0; i < 6; ++i) {
        const unsigned int base = i * 4;
        indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
        indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
    }

    Mesh mesh;
    mesh.SetVertices(vertices, indices);
    return mesh;
}

Mesh Mesh::CreatePlane(const float width, const float depth) {
    const float hw = width * 0.5f;
    const float hd = depth * 0.5f;

    std::vector<Vertex> vertices = {
        {{-hw, 0.0f, -hd}, {0, 1, 0}, {0.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        {{ hw, 0.0f, -hd}, {0, 1, 0}, {1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        {{ hw, 0.0f,  hd}, {0, 1, 0}, {1.0f, 1.0f}, {0.8f, 0.8f, 0.8f}},
        {{-hw, 0.0f,  hd}, {0, 1, 0}, {0.0f, 1.0f}, {0.8f, 0.8f, 0.8f}},
    };
    std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

    Mesh mesh;
    mesh.SetVertices(vertices, indices);
    return mesh;
}

Scene::MeshHandle MeshRegistry::AddMesh(Mesh mesh) {
    const uint32_t id = m_nextId++;
    m_meshes.emplace(id, std::move(mesh));
    return Scene::MeshHandle{id};
}

Scene::MeshHandle MeshRegistry::CreateCube(const float size) {
    Mesh m = Mesh::CreateCube(size);
    return AddMesh(std::move(m));
}

Scene::MeshHandle MeshRegistry::CreatePlane(const float width, const float depth) {
    Mesh m = Mesh::CreatePlane(width, depth);
    return AddMesh(std::move(m));
}

const Mesh* MeshRegistry::Get(const Scene::MeshHandle handle) const {
    if (!handle.IsValid()) {
        return nullptr;
    }
    const auto it = m_meshes.find(handle.id);
    if (it == m_meshes.end()) {
        return nullptr;
    }
    return &it->second;
}

void MeshRegistry::Clear() {
    m_meshes.clear();
}

} // namespace Renderer
