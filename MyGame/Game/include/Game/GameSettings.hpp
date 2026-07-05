#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <Engine/DisplaySyncController.hpp>
#include <Engine/InputManager.hpp>

namespace Game {

    /** Configuration settings related to display and vsync. */
    struct DisplaySettings {
        Engine::SyncMode syncMode = Engine::SyncMode::Off;
        unsigned int fpsCap = 120;
    };

    /** Keyboard bindings mapped to specific Engine::GameAction enumerations. */
    struct ControlSettings {
        sf::Keyboard::Key moveUp = sf::Keyboard::W;
        sf::Keyboard::Key moveDown = sf::Keyboard::S;
        sf::Keyboard::Key moveLeft = sf::Keyboard::A;
        sf::Keyboard::Key moveRight = sf::Keyboard::D;
        sf::Keyboard::Key jump = sf::Keyboard::Space;
        sf::Keyboard::Key shoot = sf::Keyboard::LControl;
        sf::Keyboard::Key confirm = sf::Keyboard::Return;
        sf::Keyboard::Key pause = sf::Keyboard::Escape;
        sf::Keyboard::Key toggleDebug = sf::Keyboard::R;
        sf::Keyboard::Key toggleVSync = sf::Keyboard::V;
    };

    /** Tweakable gameplay variables (e.g., speed, size, tick rate). */
    struct GameplaySettings {
        float playerSpeed = 250.0f;
        float playerSize = 60.0f;
        int fixedTimestepHz = 60;
    };

    /** Root container for all serializable game configuration parameters. */
    struct GameSettings {
        DisplaySettings display;
        ControlSettings controls;
        GameplaySettings gameplay;
    };

}
