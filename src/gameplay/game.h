#pragma once

#include "core/i_game.h"

#include "gameplay/collision_system.h"
#include "gameplay/level.h"
#include "gameplay/player_controller.h"

#include <memory>

namespace Scene {
class Scene;
}

namespace Gameplay {

class Game : public Core::IGame {
public:
    Game() = default;
    ~Game() override = default;

    bool Initialize(Scene::Scene& scene) override;
    void Update(float deltaTime, const Core::InputState& input, Scene::Camera& camera) override;
    bool HasWon() const override { return m_hasWon; }
    glm::vec3 GetPlayerPosition() const override;
    void Reset(Scene::Scene& scene) override;
    bool IsVoid() const;

private:
    void BuildLevel(Scene::Scene& scene);

    std::unique_ptr<Level> m_level;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    bool m_hasWon{false};
    Scene::Scene* m_scenePtr{nullptr};
};

} // namespace Gameplay
