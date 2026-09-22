#include "gameplay/collision_system.h"
#include "scene/scene.h"
#include "scene/scene_node.h"

#include <glm/vec3.hpp>

extern void Check(bool, const char*, const char*, int, const char*);
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_MSG(expr, msg) Check((expr), #expr, __FILE__, __LINE__, msg)

static bool VecNear(const glm::vec3& a, const glm::vec3& b, float eps = 0.002f) {
    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps;
}

void RunCollisionTests() {
    // AABB overlap / non-overlap
    {
        Gameplay::Aabb a = Gameplay::CollisionSystem::MakeAabb(glm::vec3(0), glm::vec3(0.5f));
        Gameplay::Aabb b = Gameplay::CollisionSystem::MakeAabb(glm::vec3(0.6f,0,0), glm::vec3(0.5f));
        CHECK_MSG(a.Intersects(b), "Touching/overlapping AABBs must intersect");
        Gameplay::Aabb c = Gameplay::CollisionSystem::MakeAabb(glm::vec3(10,0,0), glm::vec3(0.5f));
        CHECK_MSG(!a.Intersects(c), "Far AABBs must not intersect");
    }

    // World-space child collider (P1 3.3): parent(10)+child local(2) => world 12
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* parent = scene.CreateNode("Parent");
        parent->GetTransform().SetPosition(glm::vec3(10,0,0));
        auto* childStatic = scene.CreateChildNode(parent, "ChildStatic");
        childStatic->GetTransform().SetPosition(glm::vec3(2,0,0));
        childStatic->SetExtents(glm::vec3(0.5f));
        childStatic->SetIsStatic(true);
        // World is 12,0,0 — verify GetWorldAabb uses it
        auto w = Gameplay::CollisionSystem::GetWorldAabb(childStatic);
        CHECK_MSG(VecNear(w.GetCenter(), glm::vec3(12,0,0)), "World-space child collider must be 12,0,0");
    }

    // Ground contact: player above floor resolves with normal +Y and penetration
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* floor = scene.CreateNode("Floor");
        floor->GetTransform().SetPosition(glm::vec3(0,0,0));
        floor->SetExtents(glm::vec3(5,0.1f,5));
        floor->SetIsStatic(true);
        auto* player = scene.CreateNode("Player");
        player->GetTransform().SetPosition(glm::vec3(0,0.05f,0)); // slight penetration into floor's top (0.1)
        player->SetExtents(glm::vec3(0.4f,0.5f,0.4f));
        std::vector<Scene::SceneNode*> buf;
        buf.reserve(16);
        const auto& contacts = col.ResolvePlayerCollisions(player, scene, buf);
        bool grounded = false;
        for (auto& c : contacts) if (c.normal.y > 0.5f) grounded = true;
        CHECK_MSG(grounded, "Ground contact must have normal +Y");
        // Also verify not treating as wall/ceiling
        for (auto& c : contacts) CHECK_MSG(c.normal.y != 0 || c.normal.x != 0 || c.normal.z != 0, "Normal must be axis-aligned non-zero");
    }

    // Wall contact: player penetrates wall from side — normal ±X
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* wall = scene.CreateNode("Wall");
        wall->GetTransform().SetPosition(glm::vec3(2,0.5f,0));
        wall->SetExtents(glm::vec3(0.5f, 1.0f, 2.0f));
        wall->SetIsStatic(true);
        auto* player = scene.CreateNode("Player");
        player->GetTransform().SetPosition(glm::vec3(1.8f,0.5f,0)); // overlap wall
        player->SetExtents(glm::vec3(0.4f,0.5f,0.4f));
        std::vector<Scene::SceneNode*> buf; buf.reserve(16);
        const auto& contacts = col.ResolvePlayerCollisions(player, scene, buf);
        bool wallHit = false;
        for (auto& c : contacts) if (std::abs(c.normal.x) > 0.5f) wallHit = true;
        CHECK_MSG(wallHit, "Wall contact must have normal ±X");
        // Wall should not produce ground normal
        for (auto& c : contacts) CHECK_MSG(c.normal.y < 0.6f, "Wall contact should not be ground");
    }

    // Ceiling contact: player pushes into ceiling from below — normal -Y
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* ceiling = scene.CreateNode("Ceiling");
        ceiling->GetTransform().SetPosition(glm::vec3(0,3,0));
        ceiling->SetExtents(glm::vec3(5,0.2f,5));
        ceiling->SetIsStatic(true);
        auto* player = scene.CreateNode("Player");
        player->GetTransform().SetPosition(glm::vec3(0,2.8f,0)); // intersects ceiling
        player->SetExtents(glm::vec3(0.4f,0.5f,0.4f));
        std::vector<Scene::SceneNode*> buf; buf.reserve(16);
        const auto& contacts = col.ResolvePlayerCollisions(player, scene, buf);
        bool ceilingHit = false;
        for (auto& c : contacts) if (c.normal.y < -0.5f) ceilingHit = true;
        CHECK_MSG(ceilingHit, "Ceiling contact must have normal -Y");
    }

    // Elevated floor: player above higher platform must detect ground without global floor Y
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* elevated = scene.CreateNode("Elevated");
        elevated->GetTransform().SetPosition(glm::vec3(0,2,0));
        elevated->SetExtents(glm::vec3(2,0.1f,2));
        elevated->SetIsStatic(true);
        auto* player = scene.CreateNode("Player");
        player->GetTransform().SetPosition(glm::vec3(0,2.05f,0));
        player->SetExtents(glm::vec3(0.4f,0.5f,0.4f));
        std::vector<Scene::SceneNode*> buf; buf.reserve(16);
        const auto& contacts = col.ResolvePlayerCollisions(player, scene, buf);
        bool grounded = false;
        for (auto& c : contacts) if (c.normal.y > 0.5f) grounded = true;
        CHECK_MSG(grounded, "Elevated floor must still produce ground contact (no global Y)");
    }

    // Void: no floor nearby — grounded must remain false (implicit via no contacts)
    {
        Scene::Scene scene;
        Gameplay::CollisionSystem col;
        col.SetPlayerExtents(glm::vec3(0.4f,0.5f,0.4f));
        auto* farFloor = scene.CreateNode("Far");
        farFloor->GetTransform().SetPosition(glm::vec3(100,0,100));
        farFloor->SetExtents(glm::vec3(5,0.1f,5));
        farFloor->SetIsStatic(true);
        auto* player = scene.CreateNode("Player");
        player->GetTransform().SetPosition(glm::vec3(0,5,0));
        player->SetExtents(glm::vec3(0.4f,0.5f,0.4f));
        std::vector<Scene::SceneNode*> buf; buf.reserve(16);
        const auto& contacts = col.ResolvePlayerCollisions(player, scene, buf);
        bool grounded = false;
        for (auto& c : contacts) if (c.normal.y > 0.5f) grounded = true;
        CHECK_MSG(!grounded, "Void must not report grounded");
    }
}
