#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Engine/Transform2D.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

TEST_CASE("Transform2D default construct", "[Transform2D]") {
    Engine::Transform2D t;
    REQUIRE(t.position.x == 0.0f);
    REQUIRE(t.position.y == 0.0f);
    REQUIRE(t.rotation == 0.0f);
    REQUIRE(t.scale.x == 1.0f);
    REQUIRE(t.scale.y == 1.0f);
}

TEST_CASE("Transform2D ApplyTo on sf::RectangleShape", "[Transform2D]") {
    Engine::Transform2D t;
    t.position = {10.0f, 20.0f};
    t.rotation = 45.0f;
    t.scale = {2.0f, 3.0f};

    sf::RectangleShape shape;
    t.ApplyTo(shape);

    REQUIRE(shape.getPosition().x == 10.0f);
    REQUIRE(shape.getPosition().y == 20.0f);
    REQUIRE(shape.getRotation() == 45.0f);
    REQUIRE(shape.getScale().x == 2.0f);
    REQUIRE(shape.getScale().y == 3.0f);
}

TEST_CASE("Transform2D Lerp position", "[Transform2D]") {
    Engine::Transform2D a;
    a.position = {0.0f, 0.0f};
    
    Engine::Transform2D b;
    b.position = {10.0f, 10.0f};

    Engine::Transform2D mid = Engine::Lerp(a, b, 0.5f);
    REQUIRE_THAT(mid.position.x, Catch::Matchers::WithinAbs(5.0f, 1e-5f));
    REQUIRE_THAT(mid.position.y, Catch::Matchers::WithinAbs(5.0f, 1e-5f));
}

TEST_CASE("Transform2D Lerp rotation shortest path", "[Transform2D]") {
    Engine::Transform2D a;
    a.rotation = 350.0f;
    
    Engine::Transform2D b;
    b.rotation = 10.0f;

    Engine::Transform2D mid = Engine::Lerp(a, b, 0.5f);
    
    // Normalize mid.rotation to [0, 360) for testing
    float rot = std::fmod(mid.rotation, 360.0f);
    if (rot < 0) rot += 360.0f;

    REQUIRE_THAT(rot, Catch::Matchers::WithinAbs(0.0f, 1e-5f));
}

TEST_CASE("Transform2D Lerp scale", "[Transform2D]") {
    Engine::Transform2D a;
    a.scale = {1.0f, 1.0f};
    
    Engine::Transform2D b;
    b.scale = {3.0f, 3.0f};

    Engine::Transform2D mid = Engine::Lerp(a, b, 0.5f);
    REQUIRE_THAT(mid.scale.x, Catch::Matchers::WithinAbs(2.0f, 1e-5f));
    REQUIRE_THAT(mid.scale.y, Catch::Matchers::WithinAbs(2.0f, 1e-5f));
}
