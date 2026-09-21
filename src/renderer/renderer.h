#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>

#include "renderer/mesh.h"
#include "renderer/shader.h"

namespace Scene {
class Scene;
class Camera;
class SceneNode;
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
    MeshRegistry& GetMeshRegistry() { return m_meshRegistry; }
    const MeshRegistry& GetMeshRegistry() const { return m_meshRegistry; }

    static constexpr float kSnapScale = 160.0f;
    static constexpr float kColorLevels = 32.0f;
    static constexpr float kFogNear = 8.0f;
    static constexpr float kFogFar = 22.0f;

private:
    bool LoadPsxShader();
    void SetupPsxUniforms(const glm::mat4& view, const glm::mat4& projection);

    std::unique_ptr<Shader> m_psxShader;
    MeshRegistry m_meshRegistry;
    mutable std::vector<Scene::SceneNode*> m_nodeBuffer;
    bool m_isInitialized{false};
    bool m_uniformsCached{false};
};

} // namespace Renderer
