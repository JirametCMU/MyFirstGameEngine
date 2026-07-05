#include <catch2/catch_test_macros.hpp>
#include "Game/SettingsManager.hpp"
#include <filesystem>
#include <fstream>

TEST_CASE("SettingsManager loads default when file missing", "[Settings]") {
    std::filesystem::path path = "non_existent_settings.json";
    Game::GameSettings settings = Game::SettingsManager::Load(path);
    
    // Check some defaults
    REQUIRE(settings.display.fpsCap == 120);
    REQUIRE(settings.gameplay.playerSpeed == 250.0f);
}

TEST_CASE("SettingsManager validates and clamps out-of-range values", "[Settings]") {
    std::filesystem::path path = "test_settings.json";
    
    std::ofstream out(path);
    out << R"({
        "display": { "fpsCap": 1000 },
        "gameplay": { "playerSpeed": -50.0, "fixedTimestepHz": 500 }
    })";
    out.close();

    Game::GameSettings settings = Game::SettingsManager::Load(path);
    
    REQUIRE(settings.display.fpsCap == 120); // Clamped
    REQUIRE(settings.gameplay.playerSpeed == 1.0f); // Clamped
    REQUIRE(settings.gameplay.fixedTimestepHz == 240); // Clamped
    
    std::filesystem::remove(path);
}
