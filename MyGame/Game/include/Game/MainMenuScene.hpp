#pragma once

#include <Engine/Scene.hpp>
#include <Engine/SceneManager.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/ResourceTypes.hpp>
#include <SFML/Graphics.hpp>

namespace Game {

    /**
     * MainMenuScene — A simple title screen that transitions to gameplay.
     *
     * Displays the game title and a "Press Enter to Play" prompt.
     * When the player presses the Enter/Return key, the SceneManager
     * switches to the "gameplay" scene.
     *
     * Fonts are obtained from the Engine's FontManager rather than being
     * loaded from hardcoded OS paths. This enables cross-platform font
     * management and hot-swapping by simply replacing files in Assets/Fonts/.
     */
    class MainMenuScene : public Engine::Scene {
    public:
        /**
         * @param sceneManager Reference to the SceneManager for triggering transitions.
         * @param input        Reference to the InputManager for reading player input.
         * @param fonts        Reference to the FontManager for obtaining cached fonts.
         * @param designSize   The logical resolution for positioning UI elements.
         */
        MainMenuScene(Engine::SceneManager& sceneManager,
                      const Engine::InputManager& input,
                      Engine::FontManager& fonts,
                      sf::Vector2f designSize);

        void OnEnter() override;
        void OnUpdate(float deltaTime) override;
        void OnRender(sf::RenderWindow& window, float alpha) override;
        void OnExit() override;

    private:
        Engine::SceneManager& m_SceneManager;
        const Engine::InputManager& m_Input;
        Engine::FontManager& m_Fonts;
        sf::Vector2f m_DesignSize;

        sf::Text m_TitleText;
        sf::Text m_PromptText;
        bool m_FontReady = false;

        // Pulsing animation for the prompt text
        float m_PulseTimer = 0.0f;
    };

} // namespace Game
