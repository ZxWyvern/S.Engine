#pragma once

#include "scene/transform.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Scene {

class Camera {
public:
    Camera();

    void SetPerspective(float fovDegrees, float aspect, float nearPlane, float farPlane);
    void SetPosition(const glm::vec3& position) { m_position = position; }
    const glm::vec3& GetPosition() const { return m_position; }
    void SetTarget(const glm::vec3& target) { m_target = target; }
    void SetUp(const glm::vec3& up) { m_up = up; }

    // Copy transform of a Scene entity (for follow cam)
    void LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    float GetFov() const { return m_fovDegrees; }
    float GetAspect() const { return m_aspect; }

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_fovDegrees;
    float m_aspect;
    float m_nearPlane;
    float m_farPlane;
};

} // namespace Scene
