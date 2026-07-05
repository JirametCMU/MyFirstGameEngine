#pragma once

#include "Engine/Scene.hpp"
#include "Engine/SceneManager.hpp"
#include "Engine/InputManager.hpp"
#include "Engine/ResourceTypes.hpp"
#include <SFML/Graphics.hpp>
#include "Engine/ECS/World.hpp"
#include "Engine/ECS/ISystem.hpp"
#include "Engine/ECS/DrawablePool.hpp"
#include "Engine/ECS/SystemContext.hpp"

namespace Game {

    class MainMenuScene : public Engine::Scene {
    public:
        MainMenuScene(Engine::SceneManager& sceneManager,
                      const Engine::InputManager& input,
                      Engine::FontManager& fonts,
                      sf::Vector2f designSize);

        void OnEnter() override;
        void OnUpdate(float deltaTime) override;
        void OnRender(sf::RenderWindow& window, float alpha) override;
        void OnExit() override;

    private:
        Engine::World m_World;
        Engine::SystemRunner m_SystemRunner;
        Engine::DrawablePool m_DrawablePool;
        Engine::SystemContext m_Context;
    };

} // namespace Game
