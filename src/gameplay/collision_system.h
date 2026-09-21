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

struct Contact {
    Scene::SceneNode* node{nullptr};
    glm::vec3 normal{0.0f};
    float penetration{0.0f};
};

class CollisionSystem {
public:
    CollisionSystem() = default;

    // Resolves collisions and returns contacts; uses provided scratch buffer to avoid per-frame alloc.
    // Caller should reuse the same vector across frames.
    const std::vector<Contact>& ResolvePlayerCollisions(Scene::SceneNode* playerNode,
                                                        const Scene::Scene& scene,
                                                        std::vector<Scene::SceneNode*>& nodeBuffer);

    // Convenience overload that returns bool for callers that only need existence check (non-hot path)
    bool ResolvePlayerCollisions(Scene::SceneNode* playerNode, const Scene::Scene& scene);

    void SetPlayerExtents(const glm::vec3& extents) { m_playerExtents = extents; }
    const glm::vec3& GetPlayerExtents() const { return m_playerExtents; }
    void Reserve(size_t capacity) { m_contacts.reserve(capacity); m_nodeBuffer.reserve(capacity); }

    static Aabb MakeAabb(const glm::vec3& center, const glm::vec3& extents);
    // AABB in world space. Assumes axis-aligned (no rotated boxes) — documented limitation per P1 3.3.
    static Aabb GetWorldAabb(const Scene::SceneNode* node);

    const std::vector<Contact>& GetLastContacts() const { return m_contacts; }

private:
    glm::vec3 m_playerExtents{0.4f, 0.5f, 0.4f};
    std::vector<Contact> m_contacts;
    std::vector<Scene::SceneNode*> m_nodeBuffer;
    static constexpr float kEpsilon = 0.001f;
};

} // namespace Gameplay
