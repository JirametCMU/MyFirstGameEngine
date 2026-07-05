#pragma once

#include <Engine/Scene.hpp>
#include <Engine/InputManager.hpp>
#include <SFML/Graphics.hpp>
#include "Game/GameSettings.hpp"
#include <Engine/Transform2D.hpp>

namespace Game {

    /**
     * GameplayScene — The main gameplay screen.
     *
     * Contains the player square and movement logic.
     * Uses the InputManager's action-based queries for movement
     * (GameAction::MoveLeft, MoveRight, MoveUp, MoveDown)
     * and one-shot actions (Jump, Shoot).
     */
    class GameplayScene : public Engine::Scene {
    public:
        /**
         * @param input      Reference to the InputManager for reading player input.
         * @param designSize The logical resolution for initial positioning.
         * @param settings   The gameplay settings.
         */
        GameplayScene(const Engine::InputManager& input, sf::Vector2f designSize, const GameplaySettings& settings);

        void OnEnter() override;
        void OnUpdate(float deltaTime) override;
        void OnFixedUpdate(float fixedDeltaTime) override;
        void OnRender(sf::RenderWindow& window, float alpha) override;
        void OnExit() override;

    private:
        const Engine::InputManager& m_Input;
        sf::Vector2f m_DesignSize;
        GameplaySettings m_Settings;

        sf::RectangleShape m_Player;
        Engine::Transform2D m_PreviousTransform;
        Engine::Transform2D m_CurrentTransform;
    };

} // namespace Game
