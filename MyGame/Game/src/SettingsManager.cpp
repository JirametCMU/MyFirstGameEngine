#include "Game/SettingsManager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Engine/Log.hpp"

using json = nlohmann::json;

namespace Game {

    static sf::Keyboard::Key StringToKey(const std::string& str) {
        if (str == "W") return sf::Keyboard::W;
        if (str == "S") return sf::Keyboard::S;
        if (str == "A") return sf::Keyboard::A;
        if (str == "D") return sf::Keyboard::D;
        if (str == "Space") return sf::Keyboard::Space;
        if (str == "LControl") return sf::Keyboard::LControl;
        if (str == "Return") return sf::Keyboard::Return;
        if (str == "Escape") return sf::Keyboard::Escape;
        if (str == "R") return sf::Keyboard::R;
        if (str == "V") return sf::Keyboard::V;
        return sf::Keyboard::Unknown;
    }

    static std::string KeyToString(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::W: return "W";
            case sf::Keyboard::S: return "S";
            case sf::Keyboard::A: return "A";
            case sf::Keyboard::D: return "D";
            case sf::Keyboard::Space: return "Space";
            case sf::Keyboard::LControl: return "LControl";
            case sf::Keyboard::Return: return "Return";
            case sf::Keyboard::Escape: return "Escape";
            case sf::Keyboard::R: return "R";
            case sf::Keyboard::V: return "V";
            default: return "Unknown";
        }
    }

    // Loads and parses the game settings from a JSON file.
    // If the file is missing or invalid, it returns default values and logs a warning.
    GameSettings SettingsManager::Load(const std::filesystem::path& path) {
        GameSettings settings;
        std::ifstream file(path);
        if (!file.is_open()) {
            ENGINE_WARN("[Settings] Config file not found at {}, using defaults.", path.string());
            return settings;
        }

        try {
            json j = json::parse(file);
            GameSettings loadedSettings = settings;

            if (j.contains("display")) {
                auto& d = j["display"];
                if (d.contains("syncMode")) {
                    std::string mode = d["syncMode"];
                    if (mode == "VSync") loadedSettings.display.syncMode = Engine::SyncMode::VSync;
                    else if (mode == "GSync") loadedSettings.display.syncMode = Engine::SyncMode::GSync;
                    else loadedSettings.display.syncMode = Engine::SyncMode::Off;
                }
                if (d.contains("fpsCap")) {
                    loadedSettings.display.fpsCap = d["fpsCap"];
                    // Validate
                    if (loadedSettings.display.fpsCap != 0 && (loadedSettings.display.fpsCap < 30 || loadedSettings.display.fpsCap > 240)) {
                        ENGINE_WARN("[Settings] Warning: fpsCap {} out of range (30-240). Clamping to 120.", loadedSettings.display.fpsCap);
                        loadedSettings.display.fpsCap = 120;
                    }
                }
            }

            if (j.contains("controls")) {
                auto& c = j["controls"];
                if (c.contains("moveUp")) loadedSettings.controls.moveUp = StringToKey(c["moveUp"]);
                if (c.contains("moveDown")) loadedSettings.controls.moveDown = StringToKey(c["moveDown"]);
                if (c.contains("moveLeft")) loadedSettings.controls.moveLeft = StringToKey(c["moveLeft"]);
                if (c.contains("moveRight")) loadedSettings.controls.moveRight = StringToKey(c["moveRight"]);
                if (c.contains("jump")) loadedSettings.controls.jump = StringToKey(c["jump"]);
                if (c.contains("shoot")) loadedSettings.controls.shoot = StringToKey(c["shoot"]);
                if (c.contains("confirm")) loadedSettings.controls.confirm = StringToKey(c["confirm"]);
                if (c.contains("pause")) loadedSettings.controls.pause = StringToKey(c["pause"]);
                if (c.contains("toggleDebug")) loadedSettings.controls.toggleDebug = StringToKey(c["toggleDebug"]);
                if (c.contains("toggleVSync")) loadedSettings.controls.toggleVSync = StringToKey(c["toggleVSync"]);
            }

            if (j.contains("gameplay")) {
                auto& g = j["gameplay"];
                if (g.contains("playerSpeed")) loadedSettings.gameplay.playerSpeed = g["playerSpeed"];
                if (g.contains("playerSize")) loadedSettings.gameplay.playerSize = g["playerSize"];
                if (g.contains("fixedTimestepHz")) loadedSettings.gameplay.fixedTimestepHz = g["fixedTimestepHz"];
                
                // Validate gameplay settings
                if (loadedSettings.gameplay.playerSpeed < 1.0f) loadedSettings.gameplay.playerSpeed = 1.0f;
                if (loadedSettings.gameplay.playerSpeed > 10000.0f) loadedSettings.gameplay.playerSpeed = 10000.0f;
                
                if (loadedSettings.gameplay.playerSize < 1.0f) loadedSettings.gameplay.playerSize = 1.0f;
                if (loadedSettings.gameplay.playerSize > 1000.0f) loadedSettings.gameplay.playerSize = 1000.0f;
                
                if (loadedSettings.gameplay.fixedTimestepHz < 10) loadedSettings.gameplay.fixedTimestepHz = 10;
                if (loadedSettings.gameplay.fixedTimestepHz > 240) loadedSettings.gameplay.fixedTimestepHz = 240;
            }
            
            settings = loadedSettings;

        } catch (const std::exception& e) {
            ENGINE_ERROR("[Settings] Failed to parse {}: {}", path.string(), e.what());
        }

        return settings;
    }

    // Serializes the current GameSettings back into the specified JSON file.
    // Automatically creates parent directories if they don't exist.
    void SettingsManager::Save(const GameSettings& settings, const std::filesystem::path& path) {
        json j;
        j["display"]["syncMode"] = settings.display.syncMode == Engine::SyncMode::VSync ? "VSync" : 
                                   (settings.display.syncMode == Engine::SyncMode::GSync ? "GSync" : "Off");
        j["display"]["fpsCap"] = settings.display.fpsCap;

        j["controls"]["moveUp"] = KeyToString(settings.controls.moveUp);
        j["controls"]["moveDown"] = KeyToString(settings.controls.moveDown);
        j["controls"]["moveLeft"] = KeyToString(settings.controls.moveLeft);
        j["controls"]["moveRight"] = KeyToString(settings.controls.moveRight);
        j["controls"]["jump"] = KeyToString(settings.controls.jump);
        j["controls"]["shoot"] = KeyToString(settings.controls.shoot);
        j["controls"]["confirm"] = KeyToString(settings.controls.confirm);
        j["controls"]["pause"] = KeyToString(settings.controls.pause);
        j["controls"]["toggleDebug"] = KeyToString(settings.controls.toggleDebug);
        j["controls"]["toggleVSync"] = KeyToString(settings.controls.toggleVSync);

        j["gameplay"]["playerSpeed"] = settings.gameplay.playerSpeed;
        j["gameplay"]["playerSize"] = settings.gameplay.playerSize;
        j["gameplay"]["fixedTimestepHz"] = settings.gameplay.fixedTimestepHz;

        // Ensure directory exists
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }

        std::ofstream file(path);
        if (file.is_open()) {
            file << j.dump(2);
        } else {
            ENGINE_ERROR("[Settings] Failed to open {} for saving.", path.string());
        }
    }

    // Bridges the gap between the Game's data structs and the Engine's DisplaySyncController.
    void DisplaySettingsApplicator::Apply(const DisplaySettings& settings, sf::RenderWindow& window, Engine::DisplaySyncController& syncController) {
        Engine::DisplaySyncSettings engineSettings;
        engineSettings.mode = settings.syncMode;
        engineSettings.fpsCap = settings.fpsCap;
        syncController.Apply(window, engineSettings);
    }

    // Bridges the gap between the Game's keybindings and the Engine's InputManager.
    void InputSettingsApplicator::Apply(const ControlSettings& settings, Engine::InputManager& inputManager) {
        inputManager.BindAction(Engine::GameAction::MoveUp, settings.moveUp);
        inputManager.BindAction(Engine::GameAction::MoveDown, settings.moveDown);
        inputManager.BindAction(Engine::GameAction::MoveLeft, settings.moveLeft);
        inputManager.BindAction(Engine::GameAction::MoveRight, settings.moveRight);
        inputManager.BindAction(Engine::GameAction::Jump, settings.jump);
        inputManager.BindAction(Engine::GameAction::Shoot, settings.shoot);
        inputManager.BindAction(Engine::GameAction::Confirm, settings.confirm);
        inputManager.BindAction(Engine::GameAction::Pause, settings.pause);
        inputManager.BindAction(Engine::GameAction::ToggleDebugOverlay, settings.toggleDebug);
        inputManager.BindAction(Engine::GameAction::ToggleVSync, settings.toggleVSync);
    }
}
