#pragma once

#include "core/input_state.h"
#include "scene/scene_node.h"

#include <glm/vec3.hpp>

namespace Scene {
class Camera;
}

namespace Gameplay {

class PlayerController {
public:
    explicit PlayerController(Scene::SceneNode* playerNode);

    void Update(float deltaTime, const Core::InputState& input, Scene::Camera& camera);

    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
    void SetJumpSpeed(float speed) { m_jumpSpeed = speed; }
    void SetGravity(float gravity) { m_gravity = gravity; }

    bool IsGrounded() const { return m_isGrounded; }
    void SetGrounded(bool grounded) { m_isGrounded = grounded; }
    float GetVerticalVelocity() const { return m_verticalVelocity; }
    void SetVerticalVelocity(float velocity) { m_verticalVelocity = velocity; }
    void AddVerticalVelocity(float delta) { m_verticalVelocity += delta; }

    // Named constants instead of magic numbers per RULES 2.2
    static constexpr float kDefaultMoveSpeed = 4.0f;
    static constexpr float kDefaultJumpSpeed = 6.0f;
    static constexpr float kDefaultGravity = 18.0f;
    static constexpr float kGroundHeight = 0.5f;
    static constexpr float kVoidResetY = -10.0f;

private:
    void HandleMovement(float deltaTime, const Core::InputState& input, Scene::Camera& camera);
    void HandleGravity(float deltaTime);
    void HandleJump(const Core::InputState& input);

    Scene::SceneNode* m_playerNode;
    float m_moveSpeed;
    float m_jumpSpeed;
    float m_gravity;
    float m_verticalVelocity;
    bool m_isGrounded;
};

} // namespace Gameplay
