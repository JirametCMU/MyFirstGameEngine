#include <catch2/catch_test_macros.hpp>
#include <Engine/InputManager.hpp>

TEST_CASE("InputManager Edge Detection", "[InputManager]") {
    Engine::InputManager input;

    // Simulate window not having focus
    input.Update(false);
    REQUIRE(input.IsKeyJustPressed(sf::Keyboard::Space) == false);

    // Initial state: nothing pressed (assuming Update calls sf::Keyboard::isKeyPressed, which we can't fully mock here easily without mocking sfml, but we can verify it doesn't crash)
    // For unit testing InputManager cleanly, we would typically inject a mock for SFML state.
    // Given the current architecture, we just ensure it initializes and runs Update without crashing.
    input.Update(true);
    
    // Test action bindings
    input.BindAction(Engine::GameAction::Jump, sf::Keyboard::Space);
    // Since we cannot press a key programmatically in sf::Keyboard::isKeyPressed easily,
    // we just test the binding logic itself doesn't crash or behave weirdly.
    REQUIRE(input.IsActionHeld(Engine::GameAction::Jump) == false);
}
