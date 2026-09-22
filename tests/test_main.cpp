#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static int g_pass = 0;
static int g_fail = 0;

void Check(bool condition, const char* expr, const char* file, int line, const char* msg = "") {
    if (condition) {
        ++g_pass;
        return;
    }
    ++g_fail;
    std::cerr << "FAIL " << file << ":" << line << "  " << expr;
    if (msg[0] != '\0') std::cerr << "  -- " << msg;
    std::cerr << "\n";
}

#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_MSG(expr, msg) Check((expr), #expr, __FILE__, __LINE__, msg)

int ReportAndReturn() {
    std::cout << "Tests: " << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}

void RunInputTests();
void RunTransformTests();
void RunSceneTests();
void RunCollisionTests();

int main() {
    RunInputTests();
    RunTransformTests();
    RunSceneTests();
    RunCollisionTests();
    return ReportAndReturn();
}
