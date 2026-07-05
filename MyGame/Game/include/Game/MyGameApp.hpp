#pragma once

#include <Engine/Engine.hpp>
#include <Engine/SceneManager.hpp>
#include "Game/GameSettings.hpp"
#include "Game/SettingsManager.hpp"
#include "Game/SceneId.hpp"

namespace Game {

    /**
     * MyGameApp — The top-level game application entry point.
     *
     * Responsibilities (What it has to do):
     * - Serves as the root orchestrator bridging the Engine's main loop with Game-specific logic.
     * - Bootstraps the environment (reads settings.json, discovers fonts in Assets/Fonts).
     * - Configures global subsystems (DisplaySyncController limits, InputManager key bindings).
     *
     * Current Functionality (What it does now):
     * - Inherits from Engine::Application and drives the core loop (Update, FixedUpdate, Render).
     * - Instantiates the SceneManager and registers high-level states (MainMenuScene, GameplayScene).
     * - Acts as a lightweight proxy, delegating all per-frame updates directly to the active Scene.
     */
    class MyGameApp : public Engine::Application {
    public:
        MyGameApp();

    protected:
        void OnStart() override;
        void OnUpdate(float deltaTime) override;
        void OnFixedUpdate(float fixedDeltaTime) override;
        void OnRender(sf::RenderWindow& window, float alpha) override;
        void OnShutdown() override;

    private:
        Engine::SceneManager m_SceneManager;
        GameSettings m_Settings;
    };

} // namespace Game
