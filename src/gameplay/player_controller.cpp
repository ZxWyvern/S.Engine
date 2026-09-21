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

    // Void reset: if player falls below threshold, snap back to spawn
    const glm::vec3 pos = m_playerNode->GetTransform().GetPosition();
    if (pos.y < kVoidResetY) {
        m_playerNode->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
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
    // Camera-relative movement: forward/right derived from camera's horizontal plane
    const glm::vec3 camPos = camera.GetPosition();
    // Derive yaw from camera looking at player; simpler: use camera's view direction projected to XZ
    // We keep it cheap and use transform forward projected; actual desired is camera-relative WASD
    glm::vec3 moveDir(0.0f);

    // Use a simple yaw derived from camera position relative to player for third-person
    const glm::vec3 playerPos = m_playerNode->GetTransform().GetPosition();
    glm::vec3 toCam = camPos - playerPos;
    toCam.y = 0.0f;
    if (glm::length(toCam) > 0.001f) {
        toCam = glm::normalize(toCam);
    }
    const glm::vec3 camForward = -toCam; // direction camera faces projected to ground
    const glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (input.forward) moveDir += camForward;
    if (input.backward) moveDir -= camForward;
    if (input.left) moveDir -= camRight;
    if (input.right) moveDir += camRight;

    if (glm::length(moveDir) > 0.001f) {
        moveDir = glm::normalize(moveDir);
        // Delta-time based: RULES §4 mandatory
        const glm::vec3 delta = moveDir * m_moveSpeed * deltaTime;
        m_playerNode->GetTransform().Translate(glm::vec3(delta.x, 0.0f, delta.z));
    }
}

void PlayerController::HandleGravity(const float deltaTime) {
    // Delta-time based gravity integration
    m_verticalVelocity -= m_gravity * deltaTime;
    glm::vec3 pos = m_playerNode->GetTransform().GetPosition();
    pos.y += m_verticalVelocity * deltaTime;

    // Simple ground clamp at kGroundHeight — actual collision will override via CollisionSystem
    if (pos.y <= kGroundHeight) {
        pos.y = kGroundHeight;
        if (m_verticalVelocity < 0.0f) {
            m_verticalVelocity = 0.0f;
        }
        m_isGrounded = true;
    } else {
        // Will be corrected by collision; keep provisional grounded false until collision confirms
        // But don't overwrite true if we were grounded and just stepping off edge — collision will clear it
        if (m_verticalVelocity < -0.1f) {
            // still mark airborne until collision resolves
        }
    }

    m_playerNode->GetTransform().SetPosition(pos);
}

} // namespace Gameplay
