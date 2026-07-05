#include "Engine/InputManager.hpp"

namespace Engine {

    /**
     * Initializes all key and mouse button states to false (not pressed).
     * The arrays are value-initialized via the default member initializer {}.
     */
    InputManager::InputManager() = default;

    /**
     * Snapshots the current input state each frame.
     * If the window is not focused, clears all inputs so game stops responding.
     */
    void InputManager::Update(bool windowHasFocus)
    {
        // Shift current state into previous state
        m_PreviousKeys = m_CurrentKeys;
        m_PreviousMouseButtons = m_CurrentMouseButtons;

        // If the window is tabbed out, ignore all inputs and clear state
        if (!windowHasFocus)
        {
            m_CurrentKeys.fill(false);
            m_CurrentMouseButtons.fill(false);
            return;
        }

        // Poll all keyboard keys
        for (std::size_t i = 0; i < KeyCount; ++i)
        {
            m_CurrentKeys[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
        }

        // Poll all mouse buttons
        for (std::size_t i = 0; i < MouseButtonCount; ++i)
        {
            m_CurrentMouseButtons[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
        }
    }

    // --- Keyboard ---
    
    /** Key is currently held down this frame. */
    auto InputManager::IsKeyHeld(sf::Keyboard::Key key) const -> bool
    {
        auto index = static_cast<int>(key);
        if (index < 0 || static_cast<std::size_t>(index) >= KeyCount)
        {
            return false;
        }
        return m_CurrentKeys[static_cast<std::size_t>(index)];
    }

    /** Key was NOT pressed last frame, but IS pressed this frame. */
    auto InputManager::IsKeyJustPressed(sf::Keyboard::Key key) const -> bool
    {
        auto index = static_cast<int>(key);
        if (index < 0 || static_cast<std::size_t>(index) >= KeyCount)
        {
            return false;
        }
        auto idx = static_cast<std::size_t>(index);
        return m_CurrentKeys[idx] && !m_PreviousKeys[idx];
    }

    /** Key WAS pressed last frame, but is NOT pressed this frame. */
    auto InputManager::IsKeyJustReleased(sf::Keyboard::Key key) const -> bool
    {
        auto index = static_cast<int>(key);
        if (index < 0 || static_cast<std::size_t>(index) >= KeyCount)
        {
            return false;
        }
        auto idx = static_cast<std::size_t>(index);
        return !m_CurrentKeys[idx] && m_PreviousKeys[idx];
    }

    // --- Mouse ---

    /** Mouse button is currently held down this frame. */
    auto InputManager::IsMouseButtonHeld(sf::Mouse::Button button) const -> bool
    {
        auto index = static_cast<int>(button);
        if (index < 0 || static_cast<std::size_t>(index) >= MouseButtonCount)
        {
            return false;
        }
        return m_CurrentMouseButtons[static_cast<std::size_t>(index)];
    }

    /** Mouse button was NOT pressed last frame, but IS pressed this frame. */
    auto InputManager::IsMouseButtonJustPressed(sf::Mouse::Button button) const -> bool
    {
        auto index = static_cast<int>(button);
        if (index < 0 || static_cast<std::size_t>(index) >= MouseButtonCount)
        {
            return false;
        }
        auto idx = static_cast<std::size_t>(index);
        return m_CurrentMouseButtons[idx] && !m_PreviousMouseButtons[idx];
    }

    /** Mouse button WAS pressed last frame, but is NOT pressed this frame. */
    auto InputManager::IsMouseButtonJustReleased(sf::Mouse::Button button) const -> bool
    {
        auto index = static_cast<int>(button);
        if (index < 0 || static_cast<std::size_t>(index) >= MouseButtonCount)
        {
            return false;
        }
        auto idx = static_cast<std::size_t>(index);
        return !m_CurrentMouseButtons[idx] && m_PreviousMouseButtons[idx];
    }

    // --- Action Mappings ---

    void InputManager::BindAction(GameAction action, sf::Keyboard::Key key)
    {
        m_ActionBindings[action] = key;
    }

    auto InputManager::IsActionHeld(GameAction action) const -> bool
    {
        auto it = m_ActionBindings.find(action);
        if (it == m_ActionBindings.end())
        {
            return false;
        }
        return IsKeyHeld(it->second);
    }

    auto InputManager::IsActionJustPressed(GameAction action) const -> bool
    {
        auto it = m_ActionBindings.find(action);
        if (it == m_ActionBindings.end())
        {
            return false;
        }
        return IsKeyJustPressed(it->second);
    }

} // namespace Engine
