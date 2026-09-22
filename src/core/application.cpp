#include "core/application.h"

#include "core/engine_config.h"
#include "foundation/logger.h"
#include "platform/input.h"
#include "platform/window.h"
#include "renderer/render_target.h"
#include "renderer/renderer.h"
#include "scene/scene.h"

#include <SDL.h>
#include <glm/glm.hpp>

namespace Core {

Application::Application() = default;

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (m_isInitialized) {
        return true;
    }

    m_window = std::make_unique<Platform::Window>(
        EngineConfig::kWindowWidth,
        EngineConfig::kWindowHeight,
        EngineConfig::kWindowTitle);

    if (!m_window->IsValid()) {
        Foundation::Logger::Error("Window initialization failed");
        return false;
    }

    m_input = std::make_unique<Platform::Input>();
    m_time = std::make_unique<Time>();

    m_renderTarget = std::make_unique<Renderer::RenderTarget>();
    if (!m_renderTarget->Initialize(EngineConfig::kInternalResolutionWidth,
                                    EngineConfig::kInternalResolutionHeight)) {
        Foundation::Logger::Error("RenderTarget initialization failed");
        return false;
    }

    m_renderer = std::make_unique<Renderer::Renderer>();
    if (!m_renderer->Initialize()) {
        Foundation::Logger::Error("Renderer initialization failed");
        return false;
    }

    m_scene = std::make_unique<Scene::Scene>();

    if (m_game != nullptr) {
        if (!m_game->Initialize(*m_scene, m_renderer->GetMeshRegistry())) {
            Foundation::Logger::Error("IGame initialization failed");
            return false;
        }
    } else {
        Foundation::Logger::Warning("No IGame injected — running with empty scene");
    }

    m_isInitialized = true;
    Foundation::Logger::Info("Application initialized (WASD move, Space jump, R reset, ESC quit)");
    return true;
}

void Application::SetGame(std::unique_ptr<IGame> game) {
    m_game = std::move(game);
}

void Application::Run() {
    if (!m_isInitialized) {
        Foundation::Logger::Error("Run called before Initialize");
        return;
    }

    while (!m_window->ShouldClose()) {
        m_time->Tick();
        const float deltaTime = m_time->GetDeltaTime();

        m_window->PollEvents();
        m_input->Update();

        Update(deltaTime);
        Render();
    }
}

InputState Application::BuildInputState() const {
    InputState state;
    if (m_input == nullptr) return state;
    state.forward = m_input->IsKeyDown(SDL_SCANCODE_W);
    state.backward = m_input->IsKeyDown(SDL_SCANCODE_S);
    state.left = m_input->IsKeyDown(SDL_SCANCODE_A);
    state.right = m_input->IsKeyDown(SDL_SCANCODE_D);
    state.jump = m_input->IsKeyPressed(SDL_SCANCODE_SPACE);
    state.jumpHeld = m_input->IsKeyDown(SDL_SCANCODE_SPACE);
    state.reset = m_input->IsKeyPressed(SDL_SCANCODE_R);
    return state;
}

void Application::Update(const float deltaTime) {
    if (m_scene == nullptr) {
        return;
    }

    if (m_game != nullptr) {
        const InputState inputState = BuildInputState();
        auto& camera = m_scene->GetCamera();
        m_game->Update(deltaTime, inputState, camera);
    }
}

void Application::Render() {
    if (m_window == nullptr || m_renderTarget == nullptr || m_renderer == nullptr || m_scene == nullptr) {
        return;
    }

    // Phase B: render to low-res target (320x240), then upscale with nearest-neighbor
    m_renderTarget->Bind();
    m_renderer->BeginFrame(m_scene->GetCamera());
    m_renderer->RenderScene(*m_scene);
    m_renderer->EndFrame();
    m_renderTarget->Unbind(m_window->GetWidth(), m_window->GetHeight());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_renderTarget->BlitToScreen(m_window->GetWidth(), m_window->GetHeight());

    m_window->SwapBuffers();
}

void Application::Shutdown() {
    m_game.reset();
    m_scene.reset();
    m_renderer.reset();
    m_renderTarget.reset();
    m_time.reset();
    m_input.reset();
    m_window.reset();
    m_isInitialized = false;
}

} // namespace Core
