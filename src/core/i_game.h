#pragma once

#include "core/input_state.h"

#include <glm/vec3.hpp>

namespace Platform {
class Input;
}
namespace Scene {
class Scene;
class Camera;
}

namespace Core {

class IGame {
public:
    virtual ~IGame() = default;
    virtual bool Initialize(Scene::Scene& scene) = 0;
    virtual void Update(float deltaTime, const InputState& input, Scene::Camera& camera) = 0;
    virtual bool HasWon() const = 0;
    virtual glm::vec3 GetPlayerPosition() const = 0;
    virtual void Reset(Scene::Scene& scene) = 0;
};

} // namespace Core
