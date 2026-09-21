#include "gameplay/level.h"

#include "core/logger.h"
#include "scene/scene_node.h"

#include <glm/vec3.hpp>

namespace Gameplay {

Level::Level(Scene::Scene& scene)
    : m_scene(scene) {
}

void Level::Build() {
    m_scene.Clear();

    // Shared meshes — created once and reused (no per-node allocation duplication)
    m_cubeMesh = std::make_shared<Renderer::Mesh>(Renderer::Mesh::CreateCube(1.0f));
    m_planeMesh = std::make_shared<Renderer::Mesh>(Renderer::Mesh::CreatePlane(kFloorSize, kFloorSize));

    // Floor (static collider)
    m_floorNode = m_scene.CreateNode("Floor");
    m_floorNode->SetMesh(m_planeMesh);
    m_floorNode->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    m_floorNode->SetColor(glm::vec3(0.45f, 0.42f, 0.38f));
    // Floor extents: flat box from y=0 down a little so player stands on top
    m_floorNode->SetExtents(glm::vec3(kFloorSize * 0.5f, 0.1f, kFloorSize * 0.5f));
    m_floorNode->SetIsStatic(true);
    // Nudge floor transform so its AABB center matches visual: y= -0.05 would center; we keep at 0 and extents cover it
    // Collision uses center+extents, so floor at y=0 with extents.y=0.1 covers y in [-0.1, 0.1]
    // Player needs y ~0.5 to stand; PlayerController clamps to 0.5 and collision resolves to 0.1+0.5 =0.6.

    auto addWall = [&](const std::string& name, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color) {
        Scene::SceneNode* wall = m_scene.CreateNode(name);
        wall->SetMesh(m_cubeMesh);
        wall->GetTransform().SetPosition(pos);
        wall->GetTransform().SetScale(scale);
        wall->SetColor(color);
        // Extents derived from scale * 0.5 for cube mesh of size 1
        wall->SetExtents(scale * 0.5f);
        wall->SetIsStatic(true);
        return wall;
    };

    // Perimeter walls
    const float hs = kFloorSize * 0.5f;
    const float hw = kWallThickness * 0.5f;
    const float wh = kWallHeight * 0.5f;
    // North / South
    addWall("Wall_North", glm::vec3(0.0f, wh, -hs - hw), glm::vec3(kFloorSize + 2.0f * kWallThickness, kWallHeight, kWallThickness), glm::vec3(0.55f, 0.50f, 0.48f));
    addWall("Wall_South", glm::vec3(0.0f, wh,  hs + hw), glm::vec3(kFloorSize + 2.0f * kWallThickness, kWallHeight, kWallThickness), glm::vec3(0.55f, 0.50f, 0.48f));
    // East / West
    addWall("Wall_East",  glm::vec3( hs + hw, wh, 0.0f), glm::vec3(kWallThickness, kWallHeight, kFloorSize), glm::vec3(0.55f, 0.50f, 0.48f));
    addWall("Wall_West",  glm::vec3(-hs - hw, wh, 0.0f), glm::vec3(kWallThickness, kWallHeight, kFloorSize), glm::vec3(0.55f, 0.50f, 0.48f));

    // Interior decorations that are also colliders
    addWall("Pillar_A", glm::vec3(4.0f, wh, 2.0f), glm::vec3(1.0f, kWallHeight, 1.0f), glm::vec3(0.6f, 0.55f, 0.5f));
    addWall("Pillar_B", glm::vec3(-3.0f, wh, -4.0f), glm::vec3(1.0f, kWallHeight, 1.0f), glm::vec3(0.6f, 0.55f, 0.5f));
    // A couple more blocks
    addWall("Block_1", glm::vec3(-1.0f, 0.5f, 3.0f), glm::vec3(2.0f, 1.0f, 0.5f), glm::vec3(0.7f, 0.65f, 0.55f));
    addWall("Block_2", glm::vec3(2.0f, 0.5f, -2.0f), glm::vec3(0.5f, 1.0f, 2.0f), glm::vec3(0.7f, 0.65f, 0.55f));

    // Decorative non-colliding cubes (visual only)
    auto addDecor = [&](const std::string& name, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color) {
        Scene::SceneNode* n = m_scene.CreateNode(name);
        n->SetMesh(m_cubeMesh);
        n->GetTransform().SetPosition(pos);
        n->GetTransform().SetScale(scale);
        n->SetColor(color);
        n->SetIsStatic(false);
        return n;
    };
    addDecor("Decor_1", glm::vec3(1.5f, 0.4f, 1.5f), glm::vec3(0.8f), glm::vec3(0.9f, 0.4f, 0.3f));
    addDecor("Decor_2", glm::vec3(-2.5f, 0.3f, 2.0f), glm::vec3(0.6f), glm::vec3(0.3f, 0.9f, 0.4f));

    // Player (dynamic)
    m_playerNode = m_scene.CreateNode("Player");
    m_playerNode->SetMesh(m_cubeMesh);
    m_playerNode->GetTransform().SetPosition(glm::vec3(0.0f, 0.6f, 0.0f));
    m_playerNode->GetTransform().SetScale(glm::vec3(0.8f, 1.0f, 0.8f));
    m_playerNode->SetColor(glm::vec3(0.2f, 0.7f, 1.0f));
    m_playerNode->SetExtents(glm::vec3(0.4f, 0.5f, 0.4f));
    m_playerNode->SetIsStatic(false);

    // Goal
    m_goalNode = m_scene.CreateNode("Goal");
    m_goalNode->SetMesh(m_cubeMesh);
    m_goalNode->GetTransform().SetPosition(glm::vec3(7.0f, 0.6f, 7.0f));
    m_goalNode->GetTransform().SetScale(glm::vec3(1.0f, 1.2f, 1.0f));
    m_goalNode->SetColor(glm::vec3(1.0f, 0.9f, 0.2f));
    m_goalNode->SetExtents(glm::vec3(0.5f, 0.6f, 0.5f));
    m_goalNode->SetIsStatic(false);

    // Camera setup (third-person)
    auto& cam = m_scene.GetCamera();
    cam.SetPerspective(60.0f, static_cast<float>(320) / 240.0f, 0.1f, 100.0f);
    // Initial camera behind player
    cam.LookAt(glm::vec3(0.0f, 4.0f, 8.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    Core::Logger::Info("Level built: " + std::to_string(m_scene.GetAllNodes().size()) + " nodes");
}

bool Level::CheckWinCondition() const {
    if (m_playerNode == nullptr || m_goalNode == nullptr) {
        return false;
    }
    const glm::vec3 pPos = m_playerNode->GetTransform().GetPosition();
    const glm::vec3 gPos = m_goalNode->GetTransform().GetPosition();
    const glm::vec3 diff = pPos - gPos;
    const float distSq = glm::dot(diff, diff);
    const float radius = kGoalRadius + 0.4f;
    return distSq <= radius * radius;
}

bool Level::CheckVoidCondition() const {
    if (m_playerNode == nullptr) {
        return false;
    }
    return m_playerNode->GetTransform().GetPosition().y < -10.0f;
}

} // namespace Gameplay
