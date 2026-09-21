#include "scene/transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Scene {

Transform::Transform()
    : m_position(0.0f)
    , m_rotation(0.0f)
    , m_scale(1.0f) {
}

Transform::Transform(const glm::vec3& position)
    : m_position(position)
    , m_rotation(0.0f)
    , m_scale(1.0f) {
}

void Transform::SetPosition(const glm::vec3& position) {
    m_position = position;
}

void Transform::SetRotation(const glm::vec3& rotationDegrees) {
    m_rotation = rotationDegrees;
}

void Transform::SetScale(const glm::vec3& scale) {
    m_scale = scale;
}

void Transform::Translate(const glm::vec3& delta) {
    m_position += delta;
}

void Transform::Rotate(const glm::vec3& deltaDegrees) {
    m_rotation += deltaDegrees;
}

glm::mat4 Transform::GetMatrix() const {
    glm::mat4 m(1.0f);
    m = glm::translate(m, m_position);
    m = glm::rotate(m, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, m_scale);
    return m;
}

glm::vec3 Transform::GetForward() const {
    const float yaw = glm::radians(m_rotation.y);
    const float pitch = glm::radians(m_rotation.x);
    glm::vec3 f;
    f.x = glm::sin(yaw) * glm::cos(pitch);
    f.y = -glm::sin(pitch);
    f.z = -glm::cos(yaw) * glm::cos(pitch);
    return glm::normalize(f);
}

glm::vec3 Transform::GetRight() const {
    return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Transform::GetUp() const {
    return glm::normalize(glm::cross(GetRight(), GetForward()));
}

} // namespace Scene
