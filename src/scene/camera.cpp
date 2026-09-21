#include "scene/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Scene {

Camera::Camera()
    : m_position(0.0f, 2.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_fovDegrees(60.0f)
    , m_aspect(320.0f / 240.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f) {
}

void Camera::SetPerspective(const float fovDegrees, const float aspect, const float nearPlane, const float farPlane) {
    m_fovDegrees = fovDegrees;
    m_aspect = aspect;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

void Camera::LookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    m_position = eye;
    m_target = center;
    m_up = up;
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fovDegrees), m_aspect, m_nearPlane, m_farPlane);
}

} // namespace Scene
