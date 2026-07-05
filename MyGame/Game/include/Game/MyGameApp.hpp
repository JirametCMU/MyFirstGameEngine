#pragma once

#include <Engine/Engine.hpp>
#include <Engine/SceneManager.hpp>
#include "Game/GameSettings.hpp"
#include "Game/SettingsManager.hpp"
#include "Game/SceneId.hpp"

namespace Game {

    /**
     * MyGameApp — The top-level game application.
     *
     * Inherits from Engine::Application and owns the SceneManager.
     * Sets up input action bindings, registers scenes (MainMenu, Gameplay),
     * and delegates fixed-update and render calls to the active scene.
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
