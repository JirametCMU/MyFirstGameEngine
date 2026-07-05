#pragma once
#include "Game/GameSettings.hpp"
#include <Engine/InputManager.hpp>
#include <Engine/DisplaySyncController.hpp>
#include <filesystem>

namespace Game {

    /** Handles loading and saving GameSettings from/to disk using JSON serialization. */
    class SettingsManager {
    public:
        static GameSettings Load(const std::filesystem::path& path);
        static void Save(const GameSettings& settings, const std::filesystem::path& path);
    };

    /** Applies loaded DisplaySettings directly to the SFML window and sync controller. */
    class DisplaySettingsApplicator {
    public:
        static void Apply(const DisplaySettings& settings, sf::RenderWindow& window, Engine::DisplaySyncController& syncController);
    };

    /** Translates parsed ControlSettings into action bindings inside the Engine's InputManager. */
    class InputSettingsApplicator {
    public:
        static void Apply(const ControlSettings& settings, Engine::InputManager& inputManager);
    };

}
