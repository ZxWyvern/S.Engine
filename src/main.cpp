#include <SDL.h>

#include "core/application.h"
#include "core/logger.h"
#include "gameplay/game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Core::Application app;

    // Game layer injected into Engine Core via IGame — layer boundary intact per RULES 2.1
    // Core owns Scene; Game::Initialize will build level against Core's Scene
    auto game = std::make_unique<Gameplay::Game>();
    app.SetGame(std::move(game));

    if (!app.Initialize()) {
        Core::Logger::Error("Failed to initialize application");
        return 1;
    }
    app.Run();
    app.Shutdown();
    return 0;
}
