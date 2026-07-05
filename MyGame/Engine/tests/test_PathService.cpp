#include <catch2/catch_test_macros.hpp>
#include <Engine/PathService.hpp>

TEST_CASE("PathService Path Resolution", "[PathService]") {
    // Resolve a simple relative path
    auto resolved = Engine::PathService::Resolve("Assets/Config/settings.json");
    
    // Check that it's an absolute path (or at least valid path)
    REQUIRE(!resolved.empty());
    REQUIRE(resolved.string().find("Assets") != std::string::npos);
}
