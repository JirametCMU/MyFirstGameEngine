#include "Game/MyGameApp.hpp"
#include "Engine/PathService.hpp"

int main(int argc, char* argv[]) {
    Engine::PathService::Initialize(argc, argv);
    Game::MyGameApp app;
    app.Run();
    return 0;
}
