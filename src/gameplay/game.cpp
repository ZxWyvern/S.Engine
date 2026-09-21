#include "gameplay/game.h"

#include "core/logger.h"
#include "scene/camera.h"
#include "scene/scene.h"

#include <glm/glm.hpp>

namespace Gameplay {

bool Game::Initialize(Scene::Scene& scene) {
    m_scenePtr = &scene;
    BuildLevel(scene);
    m_hasWon = false;
    return true;
}

void Game::BuildLevel(Scene::Scene& scene) {
    m_level = std::make_unique<Level>(scene);
    m_level->Build();
    m_collisionSystem = std::make_unique<CollisionSystem>();
    m_collisionSystem->SetPlayerExtents(glm::vec3(0.4f, 0.5f, 0.4f));
    m_playerController = std::make_unique<PlayerController>(m_level->GetPlayerNode());
}

void Game::Reset(Scene::Scene& scene) {
    BuildLevel(scene);
    m_hasWon = false;
    Core::Logger::Info("Level reset");
}

bool Game::IsVoid() const {
    if (m_level == nullptr) return false;
    return m_level->CheckVoidCondition();
}

glm::vec3 Game::GetPlayerPosition() const {
    if (m_level == nullptr || m_level->GetPlayerNode() == nullptr) return glm::vec3(0.0f);
    return m_level->GetPlayerNode()->GetTransform().GetPosition();
}

void Game::Update(const float deltaTime, const Core::InputState& input, Scene::Camera& camera) {
    if (m_level == nullptr || m_playerController == nullptr || m_collisionSystem == nullptr) {
        return;
    }

    if (m_hasWon) {
        if (input.reset) {
            Reset(*m_scenePtr);
        }
        return;
    }

    if (input.reset) {
        Reset(*m_scenePtr);
        return;
    }

    m_playerController->Update(deltaTime, input, camera);

    Scene::SceneNode* playerNode = m_level->GetPlayerNode();
    const bool collided = m_collisionSystem->ResolvePlayerCollisions(playerNode, *m_scenePtr);

    // Ground check via feet box slightly below player
    if (playerNode != nullptr) {
        const glm::vec3 pos = playerNode->GetTransform().GetPosition();
        const glm::vec3 ext = m_collisionSystem->GetPlayerExtents();
        Aabb feetBox = CollisionSystem::MakeAabb(
            glm::vec3(pos.x, pos.y - ext.y - 0.05f, pos.z),
            glm::vec3(ext.x * 0.9f, 0.05f, ext.z * 0.9f));

        bool grounded = false;
        const auto nodes = m_scenePtr->GetAllNodes();
        for (const auto* n : nodes) {
            if (n == playerNode || !n->IsStatic()) continue;
            const glm::vec3 c = n->GetTransform().GetPosition();
            const glm::vec3 e = n->GetExtents();
            const Aabb box = CollisionSystem::MakeAabb(c, e);
            if (feetBox.Intersects(box)) {
                grounded = true;
                break;
            }
        }
        m_playerController->SetGrounded(grounded);
        if (grounded && m_playerController->GetVerticalVelocity() < 0.0f) {
            m_playerController->SetVerticalVelocity(0.0f);
        }
        if (collided && m_playerController->GetVerticalVelocity() > 0.0f) {
            m_playerController->SetVerticalVelocity(0.0f);
        }
    }

    if (m_level->CheckWinCondition()) {
        m_hasWon = true;
        Core::Logger::Info("WIN! Touch goal — press R to reset, ESC to quit");
    }

    if (m_level->CheckVoidCondition()) {
        Core::Logger::Warning("Player fell into void — resetting");
        Reset(*m_scenePtr);
    }
}

} // namespace Gameplay
