#include "scene/scene.h"

namespace Scene {

SceneNode* Scene::CreateNode(const std::string& name) {
    auto node = std::make_unique<SceneNode>(name);
    SceneNode* ptr = node.get();
    m_rootNodes.push_back(std::move(node));
    return ptr;
}

SceneNode* Scene::CreateChildNode(SceneNode* parent, const std::string& name) {
    auto node = std::make_unique<SceneNode>(name);
    SceneNode* ptr = node.get();
    parent->AddChild(std::move(node));
    return ptr;
}

std::vector<SceneNode*> Scene::GetAllNodes() const {
    std::vector<SceneNode*> result;
    result.reserve(m_rootNodes.size() * 4);
    for (const auto& root : m_rootNodes) {
        CollectRecursive(root.get(), result);
    }
    return result;
}

void Scene::GetAllNodesInto(std::vector<SceneNode*>& out) const {
    out.clear();
    out.reserve(m_rootNodes.size() * 4);
    for (const auto& root : m_rootNodes) {
        CollectRecursive(root.get(), out);
    }
}

void Scene::CollectRecursive(SceneNode* node, std::vector<SceneNode*>& out) const {
    out.push_back(node);
    for (const auto& child : node->GetChildren()) {
        CollectRecursive(child.get(), out);
    }
}

void Scene::Clear() {
    m_rootNodes.clear();
}

} // namespace Scene
