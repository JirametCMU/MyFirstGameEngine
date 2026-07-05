#include "Game/MainMenuScene.hpp"
#include "Game/SceneId.hpp"
#include "Engine/Log.hpp"
#include <cmath>

namespace Game {

    MainMenuScene::MainMenuScene(Engine::SceneManager& sceneManager,
                                 const Engine::InputManager& input,
                                 Engine::FontManager& fonts,
                                 sf::Vector2f designSize)
        : m_SceneManager(sceneManager)
        , m_Input(input)
        , m_Fonts(fonts)
        , m_DesignSize(designSize)
    {
    }

    void MainMenuScene::OnEnter()
    {
        ENGINE_INFO("[MainMenuScene] Entered main menu.");
        m_PulseTimer = 0.0f;

        // Check if any font is available from the FontManager
        if (!m_Fonts.HasDefault())
        {
            ENGINE_WARN("[MainMenuScene] Warning: No fonts loaded in FontManager. Menu text will not render.");
            m_FontReady = false;
            return;
        }

        const auto& font = m_Fonts.GetDefault();
        m_FontReady = true;

        // Title text
        m_TitleText.setFont(font);
        m_TitleText.setString("My Cool C++ Game");
        m_TitleText.setCharacterSize(48);
        m_TitleText.setFillColor(sf::Color::White);
        m_TitleText.setStyle(sf::Text::Bold);
        auto titleBounds = m_TitleText.getLocalBounds();
        m_TitleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                              titleBounds.top + titleBounds.height / 2.0f);
        m_TitleText.setPosition(m_DesignSize.x / 2.0f, m_DesignSize.y * 0.35f);

        // Prompt text
        m_PromptText.setFont(font);
        m_PromptText.setString("Press Enter to Play");
        m_PromptText.setCharacterSize(24);
        m_PromptText.setFillColor(sf::Color(200, 200, 200));
        auto promptBounds = m_PromptText.getLocalBounds();
        m_PromptText.setOrigin(promptBounds.left + promptBounds.width / 2.0f,
                               promptBounds.top + promptBounds.height / 2.0f);
        m_PromptText.setPosition(m_DesignSize.x / 2.0f, m_DesignSize.y * 0.55f);
    }

    void MainMenuScene::OnUpdate(float deltaTime)
    {
        // Animate the prompt text with a gentle pulse
        m_PulseTimer += deltaTime;
        if (m_FontReady)
        {
            float alpha = (std::sin(m_PulseTimer * 3.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
            auto baseAlpha = static_cast<sf::Uint8>(128.0f + 127.0f * alpha);
            m_PromptText.setFillColor(sf::Color(200, 200, 200, baseAlpha));
        }

        // Transition to gameplay when Confirm is pressed
        if (m_Input.IsActionJustPressed(Engine::GameAction::Confirm))
        {
            m_SceneManager.SwitchTo(SceneRegistry::Name(SceneId::Gameplay));
        }
    }

    void MainMenuScene::OnRender(sf::RenderWindow& window, float /*alpha*/)
    {
        if (!m_FontReady)
        {
            return;
        }

        window.draw(m_TitleText);
        window.draw(m_PromptText);
    }

    void MainMenuScene::OnExit()
    {
        ENGINE_INFO("[MainMenuScene] Exiting main menu.");
    }

} // namespace Game
