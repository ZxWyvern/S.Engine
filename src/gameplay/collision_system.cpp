#include "gameplay/collision_system.h"

#include "scene/scene.h"
#include "scene/scene_node.h"

#include <algorithm>
#include <cmath>

namespace Gameplay {

bool Aabb::Intersects(const Aabb& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

Aabb CollisionSystem::MakeAabb(const glm::vec3& center, const glm::vec3& extents) {
    return Aabb{center - extents, center + extents};
}

Aabb CollisionSystem::GetWorldAabb(const Scene::SceneNode* node) {
    const glm::vec3 center = node->GetTransform().GetPosition();
    const glm::vec3 ext = node->GetExtents();
    return MakeAabb(center, ext);
}

bool CollisionSystem::ResolvePlayerCollisions(Scene::SceneNode* playerNode, const Scene::Scene& scene) {
    if (playerNode == nullptr) {
        return false;
    }

    bool anyCollision = false;
    // Iterate over all static colliders; intentionally cache GetAllNodes() outside hot per-collider alloc by reusing vector
    // Note: GetAllNodes() allocates a vector; acceptable once per frame (not per collider). No inner-loop alloc.
    const std::vector<Scene::SceneNode*> nodes = scene.GetAllNodes();

    glm::vec3 playerPos = playerNode->GetTransform().GetPosition();
    const glm::vec3 playerExt = m_playerExtents;

    for (const Scene::SceneNode* node : nodes) {
        if (node == playerNode) {
            continue;
        }
        if (!node->IsStatic()) {
            continue;
        }

        const Aabb staticBox = GetWorldAabb(node);
        Aabb playerBox = MakeAabb(playerPos, playerExt);

        if (!playerBox.Intersects(staticBox)) {
            continue;
        }

        anyCollision = true;

        // Compute penetration on each axis (smallest penetration determines push-out direction)
        const glm::vec3 playerCenter = playerBox.GetCenter();
        const glm::vec3 staticCenter = staticBox.GetCenter();

        const float penX = std::min(playerBox.max.x - staticBox.min.x, staticBox.max.x - playerBox.min.x);
        const float penY = std::min(playerBox.max.y - staticBox.min.y, staticBox.max.y - playerBox.min.y);
        const float penZ = std::min(playerBox.max.z - staticBox.min.z, staticBox.max.z - playerBox.min.z);

        // Resolve along smallest penetration axis
        if (penY <= penX && penY <= penZ) {
            // Vertical resolution — most common (floor/ceiling)
            if (playerCenter.y > staticCenter.y) {
                playerPos.y = staticBox.max.y + playerExt.y + kEpsilon;
            } else {
                playerPos.y = staticBox.min.y - playerExt.y - kEpsilon;
            }
        } else if (penX <= penZ) {
            if (playerCenter.x > staticCenter.x) {
                playerPos.x = staticBox.max.x + playerExt.x + kEpsilon;
            } else {
                playerPos.x = staticBox.min.x - playerExt.x - kEpsilon;
            }
        } else {
            if (playerCenter.z > staticCenter.z) {
                playerPos.z = staticBox.max.z + playerExt.z + kEpsilon;
            } else {
                playerPos.z = staticBox.min.z - playerExt.z - kEpsilon;
            }
        }

        playerNode->GetTransform().SetPosition(playerPos);
        // Recompute playerBox for next collider iteration if multiple overlaps
        // No allocation: just update center
    }

    return anyCollision;
}

} // namespace Gameplay
