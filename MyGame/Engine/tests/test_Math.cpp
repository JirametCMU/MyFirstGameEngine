#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Engine/Math.hpp"

using namespace Engine::Math;

TEST_CASE("NormalizeOrZero", "[Math]") {
    sf::Vector2f v1{3.0f, 4.0f};
    auto n1 = NormalizeOrZero(v1);
    REQUIRE_THAT(n1.x, Catch::Matchers::WithinAbs(0.6f, 1e-5f));
    REQUIRE_THAT(n1.y, Catch::Matchers::WithinAbs(0.8f, 1e-5f));

    sf::Vector2f v2{0.0f, 0.0f};
    auto n2 = NormalizeOrZero(v2);
    REQUIRE(n2.x == 0.0f);
    REQUIRE(n2.y == 0.0f);
}

TEST_CASE("Normalize", "[Math]") {
    sf::Vector2f v1{3.0f, 4.0f};
    auto n1 = Normalize(v1, {1.0f, 0.0f});
    REQUIRE_THAT(n1.x, Catch::Matchers::WithinAbs(0.6f, 1e-5f));
    REQUIRE_THAT(n1.y, Catch::Matchers::WithinAbs(0.8f, 1e-5f));

    sf::Vector2f v2{0.0f, 0.0f};
    auto n2 = Normalize(v2, {1.0f, 0.0f});
    REQUIRE(n2.x == 1.0f);
    REQUIRE(n2.y == 0.0f);
}

TEST_CASE("NormalizeDigitalDirection", "[Math]") {
    auto n1 = NormalizeDigitalDirection({1.0f, 1.0f});
    REQUIRE_THAT(Length(n1), Catch::Matchers::WithinAbs(1.0f, 1e-5f));

    auto n2 = NormalizeDigitalDirection({0.0f, 0.0f});
    REQUIRE(n2.x == 0.0f);
    REQUIRE(n2.y == 0.0f);
    
    auto n3 = NormalizeDigitalDirection({1.0f, 0.0f});
    REQUIRE_THAT(n3.x, Catch::Matchers::WithinAbs(1.0f, 1e-5f));
    REQUIRE_THAT(n3.y, Catch::Matchers::WithinAbs(0.0f, 1e-5f));
}

TEST_CASE("Lerp", "[Math]") {
    auto l1 = Lerp(0.0f, 10.0f, 0.5f);
    REQUIRE_THAT(l1, Catch::Matchers::WithinAbs(5.0f, 1e-5f));

    auto v1 = Lerp(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{10.0f, 20.0f}, 0.5f);
    REQUIRE_THAT(v1.x, Catch::Matchers::WithinAbs(5.0f, 1e-5f));
    REQUIRE_THAT(v1.y, Catch::Matchers::WithinAbs(10.0f, 1e-5f));
}

TEST_CASE("Clamp", "[Math]") {
    REQUIRE(Clamp(15.0f, 0.0f, 10.0f) == 10.0f);
    REQUIRE(Clamp(-5.0f, 0.0f, 10.0f) == 0.0f);
    REQUIRE(Clamp(5.0f, 0.0f, 10.0f) == 5.0f);
}

TEST_CASE("ClampMagnitude", "[Math]") {
    auto v1 = ClampMagnitude({10.0f, 0.0f}, 5.0f);
    REQUIRE_THAT(v1.x, Catch::Matchers::WithinAbs(5.0f, 1e-5f));
    REQUIRE_THAT(v1.y, Catch::Matchers::WithinAbs(0.0f, 1e-5f));
    
    auto v2 = ClampMagnitude({3.0f, 4.0f}, 10.0f);
    REQUIRE_THAT(v2.x, Catch::Matchers::WithinAbs(3.0f, 1e-5f));
    REQUIRE_THAT(v2.y, Catch::Matchers::WithinAbs(4.0f, 1e-5f));
}

TEST_CASE("Length and LengthSquared ordering", "[Math]") {
    sf::Vector2f a{3.0f, 4.0f}; // len = 5, sq = 25
    sf::Vector2f b{6.0f, 8.0f}; // len = 10, sq = 100
    
    REQUIRE((Length(a) < Length(b)) == (LengthSquared(a) < LengthSquared(b)));
}

TEST_CASE("ApproximatelyEqual", "[Math]") {
    REQUIRE(ApproximatelyEqual(1.0f, 1.0f + 1e-6f, 1e-5f));
    REQUIRE_FALSE(ApproximatelyEqual(1.0f, 1.0f + 1e-4f, 1e-5f));
}

TEST_CASE("Dot", "[Math]") {
    REQUIRE_THAT(Dot({1.0f, 0.0f}, {0.0f, 1.0f}), Catch::Matchers::WithinAbs(0.0f, 1e-5f));
    REQUIRE_THAT(Dot({1.0f, 0.0f}, {1.0f, 0.0f}), Catch::Matchers::WithinAbs(1.0f, 1e-5f));
    REQUIRE_THAT(Dot({1.0f, 1.0f}, {2.0f, 3.0f}), Catch::Matchers::WithinAbs(5.0f, 1e-5f));
}
