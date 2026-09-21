#include "renderer/renderer.h"

#include "platform/logger.h"
#include "renderer/mesh.h"
#include "renderer/shader.h"
#include "scene/scene.h"
#include "scene/scene_node.h"

#include <glm/gtc/type_ptr.hpp>

namespace Renderer {

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    if (m_isInitialized) {
        return true;
    }
    if (!LoadPsxShader()) {
        return false;
    }
    m_isInitialized = true;
    Platform::Logger::Info("Renderer initialized");
    return true;
}

void Renderer::Shutdown() {
    m_psxShader.reset();
    m_isInitialized = false;
    m_uniformsCached = false;
}

bool Renderer::LoadPsxShader() {
    m_psxShader = std::make_unique<Shader>();
    // Try two locations: executable dir and source shaders/
    const char* paths[][2] = {
        {"shaders/psx.vert", "shaders/psx.frag"},
        {"../shaders/psx.vert", "../shaders/psx.frag"},
        {"../../shaders/psx.vert", "../../shaders/psx.frag"},
    };
    bool loaded = false;
    for (auto& p : paths) {
        if (m_psxShader->LoadFromFiles(p[0], p[1])) {
            loaded = true;
            Platform::Logger::Info(std::string("Loaded PSX shaders from ") + p[0]);
            break;
        }
    }
    if (!loaded) {
        Platform::Logger::Error("Failed to load PSX shaders from any known path");
        m_psxShader.reset();
        return false;
    }
    return true;
}

void Renderer::SetupPsxUniforms(const glm::mat4& view, const glm::mat4& projection) {
    if (m_psxShader == nullptr) {
        return;
    }
    m_psxShader->Bind();
    m_psxShader->SetMat4("uView", glm::value_ptr(view));
    m_psxShader->SetMat4("uProjection", glm::value_ptr(projection));

    // These do not change per-frame if config is static — cache after first upload
    // but re-upload is cheap; we guard with m_uniformsCached to honor RULES §4
    if (!m_uniformsCached) {
        m_psxShader->SetFloat("uSnapScale", kSnapScale);
        m_psxShader->SetFloat("uColorLevels", kColorLevels);
        m_psxShader->SetFloat("uFogNear", kFogNear);
        m_psxShader->SetFloat("uFogFar", kFogFar);
        m_psxShader->SetVec3("uFogColor", 0.08f, 0.08f, 0.10f);
        m_psxShader->SetVec3("uLightDir", -0.4f, -1.0f, -0.3f);
        m_uniformsCached = true;
    }
}

void Renderer::BeginFrame(const Scene::Camera& camera) {
    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 proj = camera.GetProjectionMatrix();
    SetupPsxUniforms(view, proj);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderScene(const Scene::Scene& scene) {
    if (m_psxShader == nullptr) {
        return;
    }
    m_psxShader->Bind();

    const auto nodes = scene.GetAllNodes();
    for (const Scene::SceneNode* node : nodes) {
        // TODO(debt): batch draw calls — current linear draw call per node; fine for v1 but will not scale — plan instancing/batch
        if (!node->HasMesh()) {
            continue;
        }
        const glm::mat4 model = node->GetWorldMatrix();
        m_psxShader->SetMat4("uModel", glm::value_ptr(model));
        const glm::vec3 col = node->GetColor();
        m_psxShader->SetVec3("uObjectColor", glm::value_ptr(col));

        // No allocation here — Mesh::Draw binds and draws directly
        node->GetMesh()->Draw();
    }
}

} // namespace Renderer
