#include "gameplay/game.h"

#include "foundation/logger.h"
#include "renderer/mesh.h"
#include "scene/camera.h"
#include "scene/scene.h"

#include <glm/glm.hpp>

namespace Gameplay {

bool Game::Initialize(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry) {
    m_meshRegistry = &meshRegistry;
    m_scenePtr = &scene;
    m_nodeBuffer.reserve(64);
    BuildLevel(scene);
    m_hasWon = false;
    return true;
}

void Game::BuildLevel(Scene::Scene& scene) {
    m_level = std::make_unique<Level>(scene, *m_meshRegistry);
    m_level->Build();
    m_collisionSystem = std::make_unique<CollisionSystem>();
    m_collisionSystem->SetPlayerExtents(glm::vec3(0.4f, 0.5f, 0.4f));
    m_collisionSystem->Reserve(m_nodeBuffer.capacity());
    m_playerController = std::make_unique<PlayerController>(m_level->GetPlayerNode());
}

void Game::Reset(Scene::Scene& scene) {
    BuildLevel(scene);
    m_hasWon = false;
    Foundation::Logger::Info("Level reset");
}

bool Game::IsVoid() const {
    if (m_level == nullptr) return false;
    return m_level->CheckVoidCondition();
}

glm::vec3 Game::GetPlayerWorldPosition() const {
    if (m_level == nullptr || m_level->GetPlayerNode() == nullptr) return glm::vec3(0.0f);
    return m_level->GetPlayerNode()->GetWorldPosition();
}

void Game::UpdateCamera(const float deltaTime, Scene::Camera& camera) {
    if (m_scenePtr == nullptr) return;
    const glm::vec3 targetPos = GetPlayerWorldPosition();
    const glm::vec3 desired = targetPos + m_cameraOffset;
    const glm::vec3 current = camera.GetPosition();
    constexpr float kCameraLerpSpeed = 4.0f;
    const float t = 1.0f - std::exp(-kCameraLerpSpeed * deltaTime);
    const glm::vec3 smoothed = glm::mix(current, desired, t);
    camera.SetPosition(smoothed);
    camera.SetTarget(targetPos + glm::vec3(0.0f, 0.5f, 0.0f));
    camera.SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::Shutdown() {
    m_playerController.reset();
    m_collisionSystem.reset();
    m_level.reset();
    m_scenePtr = nullptr;
    m_meshRegistry = nullptr;
}

void Game::Update(const float deltaTime, const Core::InputState& input, Scene::Camera& camera) {
    if (m_level == nullptr || m_playerController == nullptr || m_collisionSystem == nullptr) {
        return;
    }

    if (m_hasWon) {
        if (input.reset) {
            Reset(*m_scenePtr);
        }
        UpdateCamera(deltaTime, camera);
        return;
    }

    if (input.reset) {
        Reset(*m_scenePtr);
        UpdateCamera(deltaTime, camera);
        return;
    }

    m_playerController->Update(deltaTime, input, camera);

    Scene::SceneNode* playerNode = m_level->GetPlayerNode();
    const auto contacts = m_collisionSystem->ResolvePlayerCollisions(playerNode, *m_scenePtr, m_nodeBuffer);

    if (playerNode != nullptr) {
        bool grounded = false;
        bool hitCeiling = false;
        for (const auto& c : contacts) {
            if (c.normal.y > 0.5f) grounded = true;
            if (c.normal.y < -0.5f) hitCeiling = true;
        }

        if (!grounded) {
            const glm::vec3 pos = playerNode->GetWorldPosition();
            const glm::vec3 ext = m_collisionSystem->GetPlayerExtents();
            Aabb feetBox = CollisionSystem::MakeAabb(
                glm::vec3(pos.x, pos.y - ext.y - 0.05f, pos.z),
                glm::vec3(ext.x * 0.9f, 0.05f, ext.z * 0.9f));

            m_scenePtr->GetAllNodesInto(m_nodeBuffer);
            for (const auto* n : m_nodeBuffer) {
                if (n == playerNode || !n->IsStatic()) continue;
                const glm::vec3 c = n->GetWorldPosition();
                const glm::vec3 e = n->GetExtents();
                const Aabb box = CollisionSystem::MakeAabb(c, e);
                if (feetBox.Intersects(box)) {
                    grounded = true;
                    break;
                }
            }
        }

        m_playerController->SetGrounded(grounded);
        if (grounded && m_playerController->GetVerticalVelocity() < 0.0f) {
            m_playerController->SetVerticalVelocity(0.0f);
        }
        if (hitCeiling && m_playerController->GetVerticalVelocity() > 0.0f) {
            m_playerController->SetVerticalVelocity(0.0f);
        }
    }

    if (m_level->CheckWinCondition()) {
        m_hasWon = true;
        Foundation::Logger::Info("WIN! Touch goal — press R to reset, ESC to quit");
    }

    if (m_level->CheckVoidCondition()) {
        Foundation::Logger::Warning("Player fell into void — resetting");
        Reset(*m_scenePtr);
    }

    UpdateCamera(deltaTime, camera);
}

} // namespace Gameplay
