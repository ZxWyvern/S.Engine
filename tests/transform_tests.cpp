#include "scene/transform.h"

#include <cmath>
#include <glm/vec3.hpp>

extern void Check(bool, const char*, const char*, int, const char*);
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_MSG(expr, msg) Check((expr), #expr, __FILE__, __LINE__, msg)

static bool VecNear(const glm::vec3& a, const glm::vec3& b, float eps = 0.001f) {
    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps;
}

void RunTransformTests() {
    // Local transform matrix: position
    {
        Scene::Transform t;
        t.SetPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        const glm::mat4 m = t.GetMatrix();
        // translation in column 3
        const glm::vec3 pos(m[3][0], m[3][1], m[3][2]);
        CHECK_MSG(VecNear(pos, glm::vec3(1,2,3)), "GetMatrix must encode position in translation column");
    }

    // Scale
    {
        Scene::Transform t;
        t.SetScale(glm::vec3(2.0f, 3.0f, 4.0f));
        const glm::mat4 m = t.GetMatrix();
        // With identity rotation, diagonal should be scale
        CHECK_MSG(std::abs(m[0][0] - 2.0f) < 0.001f, "Scale X on diagonal");
        CHECK_MSG(std::abs(m[1][1] - 3.0f) < 0.001f, "Scale Y on diagonal");
        CHECK_MSG(std::abs(m[2][2] - 4.0f) < 0.001f, "Scale Z on diagonal");
    }

    // Translate mutates position
    {
        Scene::Transform t;
        t.SetPosition(glm::vec3(1,0,0));
        t.Translate(glm::vec3(2,3,4));
        CHECK_MSG(VecNear(t.GetPosition(), glm::vec3(3,3,4)), "Translate must add delta");
    }

    // GetForward at yaw=90 should point +X (yaw around Y)
    {
        Scene::Transform t;
        t.SetRotation(glm::vec3(0, 90, 0));
        const glm::vec3 f = t.GetForward();
        CHECK_MSG(VecNear(f, glm::vec3(1,0,0)), "Forward at yaw=90 must be +X");
    }

    // GetForward pitch 90 looks down? Check normalized length
    {
        Scene::Transform t;
        t.SetRotation(glm::vec3(45, 0, 0));
        const glm::vec3 f = t.GetForward();
        CHECK_MSG(std::abs(glm::length(f) - 1.0f) < 0.001f, "Forward must remain normalized");
    }
}
