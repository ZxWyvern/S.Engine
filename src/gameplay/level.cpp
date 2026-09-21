#include "gameplay/level.h"

#include "core/logger.h"
#include "renderer/mesh.h"
#include "scene/scene_node.h"

#include <glm/vec3.hpp>

namespace Gameplay {

Level::Level(Scene::Scene& scene, Renderer::MeshRegistry& meshRegistry)
    : m_scene(scene)
    , m_meshRegistry(meshRegistry) {
}

void Level::Build() {
    m_scene.Clear();

    if (!m_cubeHandle.IsValid()) {
        m_cubeHandle = m_meshRegistry.CreateCube(1.0f);
    }
    if (!m_planeHandle.IsValid()) {
        m_planeHandle = m_meshRegistry.CreatePlane(kFloorSize, kFloorSize);
    }

    m_floorNode = m_scene.CreateNode("Floor");
    m_floorNode->SetMeshHandle(m_planeHandle);
    m_floorNode->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    m_floorNode->SetColor(glm::vec3(0.45f, 0.42f, 0.38f));
    m_floorNode->SetExtents(glm::vec3(kFloorSize * 0.5f, 0.1f, kFloorSize * 0.5f));
    m_floorNode->SetIsStatic(true);

    auto addWall = [&](const std::string& name, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color) {
        Scene::SceneNode* wall = m_scene.CreateNode(name);
        wall->SetMeshHandle(m_cubeHandle);
        wall->GetTransform().SetPosition(pos);
        wall->GetTransform().SetScale(scale);
        wall->SetColor(color);
        wall->SetExtents(scale * 0.5f);
        wall->SetIsStatic(true);
        return wall;
    };

    const float hs = kFloorSize * 0.5f;
    const float hw = kWallThickness * 0.5f;
    const float wh = kWallHeight * 0.5f;
    addWall("Wall_North", glm::vec3(0.0f, wh, -hs - hw), glm::vec3(kFloorSize + 2.0f * kWallThickness, kWallHeight, kWallThickness), glm::vec3(0.55f, 0.50f, 0.48f));
    addWall("Wall_South", glm::vec3(0.0f, wh,  hs + hw), glm::vec3(kFloorSize + 2.0f * kWallThickness, kWallHeight, kWallThickness), glm::vec3(0.55f, 0.50f, 0.48f));
    addWall("Wall_East",  glm::vec3( hs + hw, wh, 0.0f), glm::vec3(kWallThickness, kWallHeight, kFloorSize), glm::vec3(0.55f, 0.50f, 0.48f));
    addWall("Wall_West",  glm::vec3(-hs - hw, wh, 0.0f), glm::vec3(kWallThickness, kWallHeight, kFloorSize), glm::vec3(0.55f, 0.50f, 0.48f));

    addWall("Pillar_A", glm::vec3(4.0f, wh, 2.0f), glm::vec3(1.0f, kWallHeight, 1.0f), glm::vec3(0.6f, 0.55f, 0.5f));
    addWall("Pillar_B", glm::vec3(-3.0f, wh, -4.0f), glm::vec3(1.0f, kWallHeight, 1.0f), glm::vec3(0.6f, 0.55f, 0.5f));
    addWall("Block_1", glm::vec3(-1.0f, 0.5f, 3.0f), glm::vec3(2.0f, 1.0f, 0.5f), glm::vec3(0.7f, 0.65f, 0.55f));
    addWall("Block_2", glm::vec3(2.0f, 0.5f, -2.0f), glm::vec3(0.5f, 1.0f, 2.0f), glm::vec3(0.7f, 0.65f, 0.55f));

    auto addDecor = [&](const std::string& name, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color) {
        Scene::SceneNode* n = m_scene.CreateNode(name);
        n->SetMeshHandle(m_cubeHandle);
        n->GetTransform().SetPosition(pos);
        n->GetTransform().SetScale(scale);
        n->SetColor(color);
        n->SetIsStatic(false);
        return n;
    };
    addDecor("Decor_1", glm::vec3(1.5f, 0.4f, 1.5f), glm::vec3(0.8f), glm::vec3(0.9f, 0.4f, 0.3f));
    addDecor("Decor_2", glm::vec3(-2.5f, 0.3f, 2.0f), glm::vec3(0.6f), glm::vec3(0.3f, 0.9f, 0.4f));

    m_playerNode = m_scene.CreateNode("Player");
    m_playerNode->SetMeshHandle(m_cubeHandle);
    m_playerNode->GetTransform().SetPosition(glm::vec3(0.0f, 0.6f, 0.0f));
    m_playerNode->GetTransform().SetScale(glm::vec3(0.8f, 1.0f, 0.8f));
    m_playerNode->SetColor(glm::vec3(0.2f, 0.7f, 1.0f));
    m_playerNode->SetExtents(glm::vec3(0.4f, 0.5f, 0.4f));
    m_playerNode->SetIsStatic(false);

    m_goalNode = m_scene.CreateNode("Goal");
    m_goalNode->SetMeshHandle(m_cubeHandle);
    m_goalNode->GetTransform().SetPosition(glm::vec3(7.0f, 0.6f, 7.0f));
    m_goalNode->GetTransform().SetScale(glm::vec3(1.0f, 1.2f, 1.0f));
    m_goalNode->SetColor(glm::vec3(1.0f, 0.9f, 0.2f));
    m_goalNode->SetExtents(glm::vec3(0.5f, 0.6f, 0.5f));
    m_goalNode->SetIsStatic(false);

    auto& cam = m_scene.GetCamera();
    cam.SetPerspective(60.0f, static_cast<float>(320) / 240.0f, 0.1f, 100.0f);
    cam.LookAt(glm::vec3(0.0f, 4.0f, 8.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    Core::Logger::Info("Level built: " + std::to_string(m_scene.GetAllNodes().size()) + " nodes");
}

bool Level::CheckWinCondition() const {
    if (m_playerNode == nullptr || m_goalNode == nullptr) {
        return false;
    }
    const glm::vec3 pPos = m_playerNode->GetWorldPosition();
    const glm::vec3 gPos = m_goalNode->GetWorldPosition();
    const glm::vec3 diff = pPos - gPos;
    const float distSq = glm::dot(diff, diff);
    const float radius = kGoalRadius + 0.4f;
    return distSq <= radius * radius;
}

bool Level::CheckVoidCondition() const {
    if (m_playerNode == nullptr) {
        return false;
    }
    return m_playerNode->GetWorldPosition().y < -10.0f;
}

} // namespace Gameplay
