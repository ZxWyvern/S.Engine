#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <memory>

#include "renderer/shader.h"

namespace Scene {
class Scene;
class Camera;
}

namespace Renderer {

class Shader;

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool Initialize();
    void Shutdown();

    void BeginFrame(const Scene::Camera& camera);
    void RenderScene(const Scene::Scene& scene);
    void EndFrame() const {}

    bool IsInitialized() const { return m_isInitialized; }
    Shader* GetPsxShader() { return m_psxShader.get(); }

    // Named constants instead of magic numbers per RULES 2.2
    static constexpr float kSnapScale = 160.0f;
    static constexpr float kColorLevels = 32.0f;
    static constexpr float kFogNear = 8.0f;
    static constexpr float kFogFar = 22.0f;

private:
    bool LoadPsxShader();
    void SetupPsxUniforms(const glm::mat4& view, const glm::mat4& projection);
    void RenderNodeRecursive(const Scene::Scene& scene) const;

    std::unique_ptr<Shader> m_psxShader;
    bool m_isInitialized{false};

    // Cached uniform locations / values to avoid per-frame string lookups
    // We cache the values and use glGetUniformLocation once per init where possible,
    // but Shader wrapper already does lookup; still we avoid uploading unchanged uniforms per-mesh.
    bool m_uniformsCached{false};
};

} // namespace Renderer
