#include "Game/GameplayScene.hpp"
#include "Engine/Log.hpp"
#include "Game/ECS/Components.hpp"
#include "Game/ECS/MovementSystem.hpp"
#include "Game/ECS/RenderSystem.hpp"
#include "Game/ECS/InputSystem.hpp"

namespace Game {

    GameplayScene::GameplayScene(Engine::SceneManager& sceneManager,
                                 const Engine::InputManager& input, 
                                 Engine::FontManager& fonts,
                                 sf::Vector2f designSize, 
                                 const GameplaySettings& settings)
        : m_Context{input, fonts, &sceneManager, designSize}
        , m_Settings(settings)
    {
    }

    void GameplayScene::OnEnter()
    {
        ENGINE_INFO("[GameplayScene] Starting game simulation...");

        m_SystemRunner = Engine::SystemRunner();
        m_SystemRunner.AddSystem(std::make_unique<InputSystem>());
        m_SystemRunner.AddSystem(std::make_unique<MovementSystem>());
        m_SystemRunner.AddSystem(std::make_unique<RenderSystem>(m_DrawablePool));

        auto player = m_World.CreateEntity();
        
        float size = m_Settings.playerSize;
        Engine::Transform2D startTransform{{m_Context.designSize.x / 2.0f, m_Context.designSize.y / 2.0f}, 0.0f, {1.0f, 1.0f}};
        Engine::PreviousTransform2D startPrevTransform;
        startPrevTransform.position = startTransform.position;
        startPrevTransform.rotation = startTransform.rotation;
        startPrevTransform.scale = startTransform.scale;

        m_World.AddComponent<Engine::Transform2D>(player, startTransform);
        m_World.AddComponent<Engine::PreviousTransform2D>(player, startPrevTransform);
        m_World.AddComponent<MoveSpeed>(player, {m_Settings.playerSpeed});
        m_World.AddComponent<RectangleShapeVisual>(player, {size, size, sf::Color::Cyan});
        m_World.AddComponent<PlayerTag>(player, {});

        m_DrawablePool.CreateRectangle(player, size, size);
    }

    void GameplayScene::OnUpdate(float deltaTime)
    {
        m_SystemRunner.OnUpdate(m_World, deltaTime, m_Context);
    }

    void GameplayScene::OnFixedUpdate(float fixedDeltaTime)
    {
        m_SystemRunner.OnFixedUpdate(m_World, fixedDeltaTime, m_Context);
    }

    void GameplayScene::OnRender(sf::RenderWindow& window, float alpha)
    {
        m_SystemRunner.OnRender(m_World, window, alpha, m_Context);
    }

    void GameplayScene::OnExit()
    {
        ENGINE_INFO("[GameplayScene] Exiting gameplay.");
        m_World = Engine::World(); 
        m_SystemRunner = Engine::SystemRunner();
        m_DrawablePool = Engine::DrawablePool();
    }

} // namespace Game
