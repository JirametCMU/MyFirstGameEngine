#include "Game/GameplayScene.hpp"
#include "Engine/Math.hpp"
#include "Engine/Log.hpp"

namespace Game {

    GameplayScene::GameplayScene(const Engine::InputManager& input, sf::Vector2f designSize, const GameplaySettings& settings)
        : m_Input(input)
        , m_DesignSize(designSize)
        , m_Settings(settings)
    {
    }

    void GameplayScene::OnEnter()
    {
        ENGINE_INFO("[GameplayScene] Starting game simulation...");

        float size = m_Settings.playerSize;
        m_Player.setSize({size, size});
        m_Player.setFillColor(sf::Color::Cyan);
        m_Player.setOrigin(size / 2.0f, size / 2.0f);
        m_CurrentTransform.position = {m_DesignSize.x / 2.0f, m_DesignSize.y / 2.0f};
        m_CurrentTransform.ApplyTo(m_Player);
        m_PreviousTransform = m_CurrentTransform;
    }

    /**
     * Real-time update for one-shot actions to ensure inputs are never missed.
     */
    void GameplayScene::OnUpdate(float deltaTime)
    {
        // --- One-shot actions ---
        if (m_Input.IsActionJustPressed(Engine::GameAction::Jump))
        {
            ENGINE_INFO("[GameplayScene] Jump! (fires only once per press)");
        }
        if (m_Input.IsActionJustPressed(Engine::GameAction::Shoot))
        {
            ENGINE_INFO("[GameplayScene] Shoot! (fires only once per press)");
        }
    }

    /**
     * Fixed-timestep update for deterministic physics and movement.
     */
    void GameplayScene::OnFixedUpdate(float fixedDeltaTime)
    {
        m_PreviousTransform = m_CurrentTransform;

        sf::Vector2f direction{0.0f, 0.0f};

        // --- Movement using action mappings (fires every frame while held) ---
        if (m_Input.IsActionHeld(Engine::GameAction::MoveUp))
        {
            direction.y -= 1.0f;
        }
        if (m_Input.IsActionHeld(Engine::GameAction::MoveDown))
        {
            direction.y += 1.0f;
        }
        if (m_Input.IsActionHeld(Engine::GameAction::MoveLeft))
        {
            direction.x -= 1.0f;
        }
        if (m_Input.IsActionHeld(Engine::GameAction::MoveRight))
        {
            direction.x += 1.0f;
        }

        direction = Engine::Math::NormalizeDigitalDirection(direction);
        m_CurrentTransform.position += direction * m_Settings.playerSpeed * fixedDeltaTime;
        m_CurrentTransform.ApplyTo(m_Player);
    }

    void GameplayScene::OnRender(sf::RenderWindow& window, float alpha)
    {
        const Engine::Transform2D renderTransform = Engine::Lerp(m_PreviousTransform, m_CurrentTransform, alpha);
        renderTransform.ApplyTo(m_Player);
        window.draw(m_Player);
    }

    void GameplayScene::OnExit()
    {
        ENGINE_INFO("[GameplayScene] Exiting gameplay.");
    }

} // namespace Game
