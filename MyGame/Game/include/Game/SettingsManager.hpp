#pragma once
#include "Game/GameSettings.hpp"
#include <Engine/InputManager.hpp>
#include <Engine/DisplaySyncController.hpp>
#include <filesystem>

namespace Game {

    class SettingsManager {
    public:
        static GameSettings Load(const std::filesystem::path& path);
        static void Save(const GameSettings& settings, const std::filesystem::path& path);
    };

    class DisplaySettingsApplicator {
    public:
        static void Apply(const DisplaySettings& settings, sf::RenderWindow& window, Engine::DisplaySyncController& syncController);
    };

    class InputSettingsApplicator {
    public:
        static void Apply(const ControlSettings& settings, Engine::InputManager& inputManager);
    };

}
