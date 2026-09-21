#include "gameplay/player_controller.h"

#include "scene/camera.h"
#include "scene/transform.h"

#include <glm/glm.hpp>

namespace Gameplay {

PlayerController::PlayerController(Scene::SceneNode* playerNode)
    : m_playerNode(playerNode)
    , m_moveSpeed(kDefaultMoveSpeed)
    , m_jumpSpeed(kDefaultJumpSpeed)
    , m_gravity(kDefaultGravity)
    , m_verticalVelocity(0.0f)
    , m_isGrounded(false) {
}

void PlayerController::Update(const float deltaTime, const Core::InputState& input, Scene::Camera& camera) {
    if (m_playerNode == nullptr) {
        return;
    }

    HandleJump(input);
    HandleMovement(deltaTime, input, camera);
    HandleGravity(deltaTime);

    const glm::vec3 pos = m_playerNode->GetWorldPosition();
    if (pos.y < kVoidResetY) {
        m_playerNode->SetWorldPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        m_verticalVelocity = 0.0f;
    }
}

void PlayerController::HandleJump(const Core::InputState& input) {
    if (input.jump && m_isGrounded) {
        m_verticalVelocity = m_jumpSpeed;
        m_isGrounded = false;
    }
}

void PlayerController::HandleMovement(const float deltaTime, const Core::InputState& input, Scene::Camera& camera) {
    const glm::vec3 camPos = camera.GetPosition();
    glm::vec3 moveDir(0.0f);

    const glm::vec3 playerPos = m_playerNode->GetWorldPosition();
    glm::vec3 toCam = camPos - playerPos;
    toCam.y = 0.0f;
    if (glm::length(toCam) > 0.001f) {
        toCam = glm::normalize(toCam);
    }
    const glm::vec3 camForward = -toCam;
    const glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (input.forward) moveDir += camForward;
    if (input.backward) moveDir -= camForward;
    if (input.left) moveDir -= camRight;
    if (input.right) moveDir += camRight;

    if (glm::length(moveDir) > 0.001f) {
        moveDir = glm::normalize(moveDir);
        const glm::vec3 delta = moveDir * m_moveSpeed * deltaTime;
        const glm::vec3 worldPos = m_playerNode->GetWorldPosition();
        const glm::vec3 newWorldPos = glm::vec3(worldPos.x + delta.x, worldPos.y, worldPos.z + delta.z);
        m_playerNode->SetWorldPosition(newWorldPos);
    }
}

void PlayerController::HandleGravity(const float deltaTime) {
    m_verticalVelocity -= m_gravity * deltaTime;
    const glm::vec3 worldPos = m_playerNode->GetWorldPosition();
    glm::vec3 newWorldPos = worldPos;
    newWorldPos.y += m_verticalVelocity * deltaTime;
    m_playerNode->SetWorldPosition(newWorldPos);

    // No hardcoded global floor — grounded comes from collision contacts in Game::Update
}

} // namespace Gameplay
