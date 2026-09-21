#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Scene {

class Transform {
public:
    Transform();
    explicit Transform(const glm::vec3& position);

    const glm::vec3& GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& position);

    const glm::vec3& GetRotation() const { return m_rotation; } // Euler degrees: pitch, yaw, roll
    void SetRotation(const glm::vec3& rotationDegrees);

    const glm::vec3& GetScale() const { return m_scale; }
    void SetScale(const glm::vec3& scale);

    void Translate(const glm::vec3& delta);
    void Rotate(const glm::vec3& deltaDegrees);

    glm::mat4 GetMatrix() const;
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation; // degrees
    glm::vec3 m_scale;
};

} // namespace Scene
