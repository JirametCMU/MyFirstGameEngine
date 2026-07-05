#include "Game/MainMenuScene.hpp"
#include "Game/SceneId.hpp"
#include "Engine/Log.hpp"
#include "Game/ECS/Components.hpp"
#include "Game/ECS/InputSystem.hpp"
#include "Game/ECS/UIAnimationSystem.hpp"
#include "Game/ECS/RenderSystem.hpp"

namespace Game {

    MainMenuScene::MainMenuScene(Engine::SceneManager& sceneManager,
                                 const Engine::InputManager& input,
                                 Engine::FontManager& fonts,
                                 sf::Vector2f designSize)
        : m_Context{input, fonts, &sceneManager, designSize}
    {
    }

    // Initializes the main menu ECS world. Sets up systems (input, animation, rendering)
    // and spawns text entities for the title and pulsing prompt.
    void MainMenuScene::OnEnter()
    {
        ENGINE_INFO("[MainMenuScene] Entered main menu.");

        m_SystemRunner = Engine::SystemRunner();
        m_SystemRunner.AddSystem(std::make_unique<InputSystem>());
        m_SystemRunner.AddSystem(std::make_unique<UIAnimationSystem>());
        m_SystemRunner.AddSystem(std::make_unique<RenderSystem>(m_DrawablePool));

        if (!m_Context.fonts.HasDefault())
        {
            ENGINE_WARN("[MainMenuScene] Warning: No fonts loaded in FontManager. Menu text will not render.");
        }

        // Title text
        auto title = m_World.CreateEntity();
        m_World.AddComponent<Engine::Transform2D>(title, {{m_Context.designSize.x / 2.0f, m_Context.designSize.y * 0.35f}, 0.0f, {1.0f, 1.0f}});
        m_World.AddComponent<TextVisual>(title, {"My Cool C++ Game", 48, sf::Color::White});
        m_DrawablePool.CreateText(title);

        m_DrawablePool.WithText(title, [&](sf::Text& text) {
            if (m_Context.fonts.HasDefault()) {
                text.setFont(m_Context.fonts.GetDefault());
            }
            text.setString("My Cool C++ Game");
            text.setCharacterSize(48);
            text.setStyle(sf::Text::Bold);
            auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        });

        // Prompt text
        auto prompt = m_World.CreateEntity();
        m_World.AddComponent<Engine::Transform2D>(prompt, {{m_Context.designSize.x / 2.0f, m_Context.designSize.y * 0.55f}, 0.0f, {1.0f, 1.0f}});
        m_World.AddComponent<TextVisual>(prompt, {"Press Enter to Play", 24, sf::Color(200, 200, 200)});
        m_World.AddComponent<PulseAnimation>(prompt, {0.0f, 1.0f, 128.0f, 255.0f});
        m_DrawablePool.CreateText(prompt);

        m_DrawablePool.WithText(prompt, [&](sf::Text& text) {
            if (m_Context.fonts.HasDefault()) {
                text.setFont(m_Context.fonts.GetDefault());
            }
            text.setString("Press Enter to Play");
            text.setCharacterSize(24);
            auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        });
    }

    // Ticks the input and UI animation systems.
    void MainMenuScene::OnUpdate(float deltaTime)
    {
        m_SystemRunner.OnUpdate(m_World, deltaTime, m_Context);
    }

    // Renders the text entities to the screen.
    void MainMenuScene::OnRender(sf::RenderWindow& window, float alpha)
    {
        m_SystemRunner.OnRender(m_World, window, alpha, m_Context);
    }

    // Cleans up the menu ECS world before transitioning to the gameplay scene.
    void MainMenuScene::OnExit()
    {
        ENGINE_INFO("[MainMenuScene] Exiting main menu.");
        m_World = Engine::World();
        m_SystemRunner = Engine::SystemRunner();
        m_DrawablePool = Engine::DrawablePool();
    }

} // namespace Game
