#include "scene/scene_node.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Scene {

SceneNode::SceneNode(std::string name)
    : m_name(std::move(name)) {
}

void SceneNode::AddChild(std::unique_ptr<SceneNode> child) {
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

glm::mat4 SceneNode::GetWorldMatrix() const {
    const glm::mat4 local = m_transform.GetMatrix();
    if (m_parent == nullptr) {
        return local;
    }
    return m_parent->GetWorldMatrix() * local;
}

glm::vec3 SceneNode::GetWorldPosition() const {
    const glm::mat4 world = GetWorldMatrix();
    return glm::vec3(world[3]);
}

void SceneNode::SetWorldPosition(const glm::vec3& worldPosition) {
    if (m_parent == nullptr) {
        m_transform.SetPosition(worldPosition);
    } else {
        const glm::vec3 parentWorld = m_parent->GetWorldPosition();
        m_transform.SetPosition(worldPosition - parentWorld);
    }
}

} // namespace Scene
