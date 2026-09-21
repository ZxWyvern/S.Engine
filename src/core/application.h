#pragma once

#include "core/i_game.h"
#include "core/input_state.h"
#include "core/time.h"

#include <glm/vec3.hpp>
#include <memory>

namespace Platform {
class Window;
class Input;
}
namespace Renderer {
class Renderer;
class RenderTarget;
}
namespace Scene {
class Scene;
}

namespace Core {

class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool Initialize();
    void SetGame(std::unique_ptr<IGame> game);
    void Run();
    void Shutdown();

private:
    void Update(float deltaTime);
    void Render();
    void UpdateCamera(float deltaTime);
    InputState BuildInputState() const;

    std::unique_ptr<Platform::Window> m_window;
    std::unique_ptr<Platform::Input> m_input;
    std::unique_ptr<Renderer::Renderer> m_renderer;
    std::unique_ptr<Renderer::RenderTarget> m_renderTarget;
    std::unique_ptr<Scene::Scene> m_scene;
    std::unique_ptr<IGame> m_game;
    std::unique_ptr<Time> m_time;

    bool m_isInitialized{false};
    glm::vec3 m_cameraOffset{0.0f, 4.0f, 7.0f};
};

} // namespace Core
