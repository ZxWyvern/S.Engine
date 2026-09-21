#pragma once

#include "scene/camera.h"
#include "scene/scene_node.h"

#include <memory>
#include <vector>

namespace Scene {

class Scene {
public:
    Scene() = default;

    SceneNode* CreateNode(const std::string& name);
    SceneNode* CreateChildNode(SceneNode* parent, const std::string& name);

    const std::vector<std::unique_ptr<SceneNode>>& GetRootNodes() const { return m_rootNodes; }
    std::vector<SceneNode*> GetAllNodes() const;
    void GetAllNodesInto(std::vector<SceneNode*>& out) const;

    Camera& GetCamera() { return m_camera; }
    const Camera& GetCamera() const { return m_camera; }

    void Clear();

private:
    void CollectRecursive(SceneNode* node, std::vector<SceneNode*>& out) const;

    std::vector<std::unique_ptr<SceneNode>> m_rootNodes;
    Camera m_camera;
};

} // namespace Scene
