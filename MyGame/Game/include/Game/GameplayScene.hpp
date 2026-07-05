#pragma once

#include "Engine/Scene.hpp"
#include "Engine/InputManager.hpp"
#include "Engine/ResourceTypes.hpp"
#include "Engine/SceneManager.hpp"
#include <SFML/Graphics.hpp>
#include "Game/GameSettings.hpp"
#include "Engine/ECS/World.hpp"
#include "Engine/ECS/ISystem.hpp"
#include "Engine/ECS/DrawablePool.hpp"
#include "Engine/ECS/SystemContext.hpp"

namespace Game {

    /**
     * GameplayScene — The main gameplay screen.
     *
     * Orchestrates the ECS world for the gameplay state. Spawns the player entity
     * and sets up the Systems (Input, Movement, Render) to drive gameplay logic.
     */
    class GameplayScene : public Engine::Scene {
    public:
        GameplayScene(Engine::SceneManager& sceneManager,
                      const Engine::InputManager& input, 
                      Engine::FontManager& fonts,
                      sf::Vector2f designSize, 
                      const GameplaySettings& settings);

        void OnEnter() override;
        void OnUpdate(float deltaTime) override;
        void OnFixedUpdate(float fixedDeltaTime) override;
        void OnRender(sf::RenderWindow& window, float alpha) override;
        void OnExit() override;

    private:
        Engine::World m_World;
        Engine::SystemRunner m_SystemRunner;
        Engine::DrawablePool m_DrawablePool;
        Engine::SystemContext m_Context;

        GameplaySettings m_Settings;
    };

} // namespace Game
