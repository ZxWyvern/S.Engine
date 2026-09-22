#include "scene/scene.h"
#include "scene/scene_node.h"

#include <cmath>
#include <glm/vec3.hpp>

extern void Check(bool, const char*, const char*, int, const char*);
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_MSG(expr, msg) Check((expr), #expr, __FILE__, __LINE__, msg)

static bool VecNear(const glm::vec3& a, const glm::vec3& b, float eps = 0.001f) {
    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps;
}

void RunSceneTests() {
    // Parent-child world transform per P1 3.3 invariant
    {
        Scene::Scene scene;
        auto* parent = scene.CreateNode("Parent");
        parent->GetTransform().SetPosition(glm::vec3(10.0f, 0.0f, 0.0f));
        auto* child = scene.CreateChildNode(parent, "Child");
        child->GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
        const glm::vec3 world = child->GetWorldPosition();
        CHECK_MSG(VecNear(world, glm::vec3(12,0,0)), "Child world must be parent(10)+local(2)=12");
    }

    // World matrix includes parent offset
    {
        Scene::Scene scene;
        auto* parent = scene.CreateNode("P");
        parent->GetTransform().SetPosition(glm::vec3(5,0,0));
        auto* child = scene.CreateChildNode(parent, "C");
        child->GetTransform().SetPosition(glm::vec3(1,0,0));
        CHECK_MSG(VecNear(child->GetWorldPosition(), glm::vec3(6,0,0)), "World matrix chain");
    }

    // SetWorldPosition handles parented case
    {
        Scene::Scene scene;
        auto* parent = scene.CreateNode("P");
        parent->GetTransform().SetPosition(glm::vec3(10,0,0));
        auto* child = scene.CreateChildNode(parent, "C");
        child->SetWorldPosition(glm::vec3(15,0,0));
        CHECK_MSG(VecNear(child->GetWorldPosition(), glm::vec3(15,0,0)), "SetWorldPosition must respect parent offset");
        CHECK_MSG(VecNear(child->GetTransform().GetPosition(), glm::vec3(5,0,0)), "Local must be world-parent");
    }

    // Hierarchy traversal: GetAllNodesInto reusable buffer
    {
        Scene::Scene scene;
        auto* a = scene.CreateNode("A");
        scene.CreateChildNode(a, "A1");
        scene.CreateChildNode(a, "A2");
        scene.CreateNode("B");
        std::vector<Scene::SceneNode*> buf;
        buf.reserve(8);
        scene.GetAllNodesInto(buf);
        CHECK_MSG(buf.size() == 4, "Traversal must find 4 nodes");
        // Buffer reuse: capacity preserved
        const size_t capBefore = buf.capacity();
        scene.GetAllNodesInto(buf);
        CHECK_MSG(buf.size() == 4, "Second traversal same count");
        CHECK_MSG(buf.capacity() == capBefore, "Buffer capacity must be preserved (no realloc)");
    }

    // Clear empties
    {
        Scene::Scene scene;
        scene.CreateNode("A");
        scene.CreateNode("B");
        scene.Clear();
        std::vector<Scene::SceneNode*> buf;
        scene.GetAllNodesInto(buf);
        CHECK_MSG(buf.empty(), "Clear must empty scene");
    }
}
