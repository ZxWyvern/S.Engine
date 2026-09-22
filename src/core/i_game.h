#pragma once

#include "core/input_state.h"

namespace Scene {
class Scene;
class Camera;
}
namespace Renderer {
class MeshRegistry;
}

namespace Core {

class IGame {
public:
    virtual ~IGame() = default;
    virtual bool Initialize(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry) = 0;
    virtual void Update(float deltaTime, const InputState& input, Scene::Camera& camera) = 0;
    virtual void Shutdown() = 0;
};

} // namespace Core
