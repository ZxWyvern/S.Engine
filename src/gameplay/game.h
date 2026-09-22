#pragma once

#include "core/i_game.h"

#include "gameplay/collision_system.h"
#include "gameplay/level.h"
#include "gameplay/player_controller.h"

#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace Scene {
class Scene;
class SceneNode;
}
namespace Renderer {
class MeshRegistry;
}

namespace Gameplay {

class Game : public Core::IGame {
public:
    Game() = default;
    ~Game() override = default;

    bool Initialize(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry) override;
    void Update(float deltaTime, const Core::InputState& input, Scene::Camera& camera) override;
    void Shutdown() override;

private:
    void BuildLevel(Scene::Scene& scene);
    void Reset(Scene::Scene& scene);
    void UpdateCamera(float deltaTime, Scene::Camera& camera);
    bool IsVoid() const;
    glm::vec3 GetPlayerWorldPosition() const;

    std::unique_ptr<Level> m_level;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    Renderer::MeshRegistry* m_meshRegistry{nullptr};
    std::vector<Scene::SceneNode*> m_nodeBuffer;
    bool m_hasWon{false};
    Scene::Scene* m_scenePtr{nullptr};
    glm::vec3 m_cameraOffset{0.0f, 4.0f, 7.0f};
};

} // namespace Gameplay
