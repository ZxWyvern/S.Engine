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
    // World-space: uses accumulated world position per P1 3.3 invariant
    // Limitation: assumes axis-aligned — rotated boxes not supported (extents not rotated)
    const glm::vec3 center = node->GetWorldPosition();
    const glm::vec3 ext = node->GetExtents();
    return MakeAabb(center, ext);
}

const std::vector<Contact>& CollisionSystem::ResolvePlayerCollisions(
    Scene::SceneNode* playerNode,
    const Scene::Scene& scene,
    std::vector<Scene::SceneNode*>& nodeBuffer) {

    m_contacts.clear();

    if (playerNode == nullptr) {
        return m_contacts;
    }

    scene.GetAllNodesInto(nodeBuffer);

    glm::vec3 playerPos = playerNode->GetWorldPosition();
    const glm::vec3 playerExt = m_playerExtents;

    for (const Scene::SceneNode* node : nodeBuffer) {
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

        const glm::vec3 playerCenter = playerBox.GetCenter();
        const glm::vec3 staticCenter = staticBox.GetCenter();

        const float penX = std::min(playerBox.max.x - staticBox.min.x, staticBox.max.x - playerBox.min.x);
        const float penY = std::min(playerBox.max.y - staticBox.min.y, staticBox.max.y - playerBox.min.y);
        const float penZ = std::min(playerBox.max.z - staticBox.min.z, staticBox.max.z - playerBox.min.z);

        glm::vec3 normal(0.0f);
        if (penY <= penX && penY <= penZ) {
            if (playerCenter.y > staticCenter.y) {
                playerPos.y = staticBox.max.y + playerExt.y + kEpsilon;
                normal = glm::vec3(0.0f, 1.0f, 0.0f);
            } else {
                playerPos.y = staticBox.min.y - playerExt.y - kEpsilon;
                normal = glm::vec3(0.0f, -1.0f, 0.0f);
            }
            m_contacts.push_back(Contact{const_cast<Scene::SceneNode*>(node), normal, penY});
        } else if (penX <= penZ) {
            if (playerCenter.x > staticCenter.x) {
                playerPos.x = staticBox.max.x + playerExt.x + kEpsilon;
                normal = glm::vec3(1.0f, 0.0f, 0.0f);
            } else {
                playerPos.x = staticBox.min.x - playerExt.x - kEpsilon;
                normal = glm::vec3(-1.0f, 0.0f, 0.0f);
            }
            m_contacts.push_back(Contact{const_cast<Scene::SceneNode*>(node), normal, penX});
        } else {
            if (playerCenter.z > staticCenter.z) {
                playerPos.z = staticBox.max.z + playerExt.z + kEpsilon;
                normal = glm::vec3(0.0f, 0.0f, 1.0f);
            } else {
                playerPos.z = staticBox.min.z - playerExt.z - kEpsilon;
                normal = glm::vec3(0.0f, 0.0f, -1.0f);
            }
            m_contacts.push_back(Contact{const_cast<Scene::SceneNode*>(node), normal, penZ});
        }

        playerNode->GetTransform().SetPosition(playerPos - node->GetWorldPosition() + node->GetTransform().GetPosition());
        // Recompute world pos for next collider if multiple overlaps and node has parent offset
        playerPos = playerNode->GetWorldPosition();
    }

    // If player has a parent, SetPosition above used parent-relative; fix by updating world correctly
    // Simpler: always set world position via helper
    if (!m_contacts.empty() && playerNode->GetParent() != nullptr) {
        // Ensure final world position is playerPos
        const glm::vec3 parentWorld = playerNode->GetParent()->GetWorldPosition();
        playerNode->GetTransform().SetPosition(playerPos - parentWorld);
    } else if (!m_contacts.empty()) {
        playerNode->GetTransform().SetPosition(playerPos);
    }

    return m_contacts;
}

bool CollisionSystem::ResolvePlayerCollisions(Scene::SceneNode* playerNode, const Scene::Scene& scene) {
    scene.GetAllNodesInto(m_nodeBuffer);
    const auto& contacts = ResolvePlayerCollisions(playerNode, scene, m_nodeBuffer);
    return !contacts.empty();
}

} // namespace Gameplay
