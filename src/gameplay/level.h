#pragma once

#include "scene/scene.h"
#include "renderer/mesh.h"

#include <memory>

namespace Gameplay {

class Level {
public:
    explicit Level(Scene::Scene& scene);

    void Build();
    Scene::SceneNode* GetPlayerNode() const { return m_playerNode; }
    Scene::SceneNode* GetGoalNode() const { return m_goalNode; }

    bool CheckWinCondition() const;
    bool CheckVoidCondition() const;

    // Named constants for level geometry
    static constexpr float kFloorSize = 20.0f;
    static constexpr float kWallHeight = 3.0f;
    static constexpr float kWallThickness = 0.5f;
    static constexpr float kPlayerSize = 1.0f;
    static constexpr float kGoalRadius = 0.6f;

private:
    Scene::Scene& m_scene;
    std::shared_ptr<Renderer::Mesh> m_cubeMesh;
    std::shared_ptr<Renderer::Mesh> m_planeMesh;

    Scene::SceneNode* m_playerNode{nullptr};
    Scene::SceneNode* m_goalNode{nullptr};
    Scene::SceneNode* m_floorNode{nullptr};
};

} // namespace Gameplay
