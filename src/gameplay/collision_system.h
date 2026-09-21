#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace Scene {
class SceneNode;
class Scene;
}

namespace Gameplay {

struct Aabb {
    glm::vec3 min{0.0f};
    glm::vec3 max{0.0f};

    bool Intersects(const Aabb& other) const;
    glm::vec3 GetCenter() const { return (min + max) * 0.5f; }
    glm::vec3 GetExtents() const { return (max - min) * 0.5f; }
};

class CollisionSystem {
public:
    CollisionSystem() = default;

    // Returns true if collision resolved (player was intersecting a static collider)
    bool ResolvePlayerCollisions(Scene::SceneNode* playerNode, const Scene::Scene& scene);

    void SetPlayerExtents(const glm::vec3& extents) { m_playerExtents = extents; }
    const glm::vec3& GetPlayerExtents() const { return m_playerExtents; }

    static Aabb MakeAabb(const glm::vec3& center, const glm::vec3& extents);

private:
    static Aabb GetWorldAabb(const Scene::SceneNode* node);

    glm::vec3 m_playerExtents{0.4f, 0.5f, 0.4f};
    static constexpr float kEpsilon = 0.001f;
};

} // namespace Gameplay
