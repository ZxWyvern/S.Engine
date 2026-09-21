#pragma once

#include "core/i_game.h"

#include "gameplay/collision_system.h"
#include "gameplay/level.h"
#include "gameplay/player_controller.h"

#include <memory>
#include <vector>

namespace Scene {
class Scene;
class SceneNode;
}

namespace Gameplay {

class Game : public Core::IGame {
public:
    Game() = default;
    ~Game() override = default;

    bool Initialize(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry);
    // IGame interface — uses injected dependencies set during Initialize
    bool Initialize(Scene::Scene& scene) override;
    void Update(float deltaTime, const Core::InputState& input, Scene::Camera& camera) override;
    bool HasWon() const override { return m_hasWon; }
    glm::vec3 GetPlayerPosition() const override;
    void Reset(Scene::Scene& scene) override;
    bool IsVoid() const;
    void SetMeshRegistry(Renderer::MeshRegistry* meshRegistry) { m_meshRegistry = meshRegistry; }

private:
    void BuildLevel(Scene::Scene& scene);

    std::unique_ptr<Level> m_level;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    Renderer::MeshRegistry* m_meshRegistry{nullptr};
    std::vector<Scene::SceneNode*> m_nodeBuffer;
    bool m_hasWon{false};
    Scene::Scene* m_scenePtr{nullptr};
};

} // namespace Gameplay
