#pragma once

#include "scene/mesh_handle.h"
#include "scene/transform.h"

#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Scene {

class SceneNode {
public:
    explicit SceneNode(std::string name);
    ~SceneNode() = default;

    SceneNode(const SceneNode&) = delete;
    SceneNode& operator=(const SceneNode&) = delete;

    const std::string& GetName() const { return m_name; }
    Transform& GetTransform() { return m_transform; }
    const Transform& GetTransform() const { return m_transform; }

    void SetMeshHandle(MeshHandle handle) { m_meshHandle = handle; }
    MeshHandle GetMeshHandle() const { return m_meshHandle; }
    bool HasMesh() const { return m_meshHandle.IsValid(); }

    void SetColor(const glm::vec3& color) { m_color = color; }
    const glm::vec3& GetColor() const { return m_color; }

    void AddChild(std::unique_ptr<SceneNode> child);
    const std::vector<std::unique_ptr<SceneNode>>& GetChildren() const { return m_children; }
    SceneNode* GetParent() const { return m_parent; }

    void SetExtents(const glm::vec3& extents) { m_extents = extents; }
    const glm::vec3& GetExtents() const { return m_extents; }
    void SetIsStatic(bool isStatic) { m_isStatic = isStatic; }
    bool IsStatic() const { return m_isStatic; }

    glm::mat4 GetWorldMatrix() const;
    glm::vec3 GetWorldPosition() const;
    void SetWorldPosition(const glm::vec3& worldPosition);

private:
    std::string m_name;
    Transform m_transform;
    MeshHandle m_meshHandle{kInvalidMeshHandle};
    glm::vec3 m_color{1.0f};
    glm::vec3 m_extents{0.5f};
    bool m_isStatic{false};
    SceneNode* m_parent{nullptr};
    std::vector<std::unique_ptr<SceneNode>> m_children;
};

} // namespace Scene
