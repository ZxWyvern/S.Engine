#pragma once

#include "scene/mesh_handle.h"
#include "scene/scene.h"

namespace Renderer {
class MeshRegistry;
}

namespace Gameplay {

class Level {
public:
    Level(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry);

    void Build();
    Scene::SceneNode* GetPlayerNode() const { return m_playerNode; }
    Scene::SceneNode* GetGoalNode() const { return m_goalNode; }

    bool CheckWinCondition() const;
    bool CheckVoidCondition() const;

    static constexpr float kFloorSize = 20.0f;
    static constexpr float kWallHeight = 3.0f;
    static constexpr float kWallThickness = 0.5f;
    static constexpr float kPlayerSize = 1.0f;
    static constexpr float kGoalRadius = 0.6f;

private:
    Scene::Scene& m_scene;
    Renderer::MeshRegistry& m_meshRegistry;
    Scene::MeshHandle m_cubeHandle{Scene::kInvalidMeshHandle};
    Scene::MeshHandle m_planeHandle{Scene::kInvalidMeshHandle};

    Scene::SceneNode* m_playerNode{nullptr};
    Scene::SceneNode* m_goalNode{nullptr};
    Scene::SceneNode* m_floorNode{nullptr};
};

} // namespace Gameplay
