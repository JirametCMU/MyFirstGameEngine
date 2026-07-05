#include "Game/MyGameApp.hpp"
#include "Engine/PathService.hpp"

// Standard entry point for the executable. Initializes the global PathService 
// so that asset paths resolve correctly, then runs the game loop.
int main(int argc, char* argv[]) {
    Engine::PathService::Initialize(argc, argv);
    Game::MyGameApp app;
    app.Run();
    return 0;
}
