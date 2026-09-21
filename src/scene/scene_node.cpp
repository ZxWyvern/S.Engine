#include "scene/scene_node.h"

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

} // namespace Scene
