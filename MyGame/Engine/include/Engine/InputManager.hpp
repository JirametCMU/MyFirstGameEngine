#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <array>
#include <unordered_map>

namespace Engine {

    /**
     * GameAction — Logical gameplay actions decoupled from physical keys.
     *
     * Instead of checking raw keys in gameplay code, systems query these
     * abstract actions. The mapping from GameAction → sf::Keyboard::Key
     * is configured via InputManager::BindAction(), supporting future
     * custom key rebinding without touching gameplay logic.
     */
    enum class GameAction {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        Jump,
        Shoot,
        Confirm,
        Pause,
        ToggleDebugOverlay,
        ToggleVSync
    };

    /**
     * InputManager — A frame-aware input subsystem that wraps SFML's raw input.
     *
     * Tracks the state of every keyboard key and mouse button across two frames
     * (previous and current), enabling detection of:
     *   - IsKeyHeld:         Key is currently pressed down.
     *   - IsKeyJustPressed:  Key transitioned from released → pressed THIS frame.
     *   - IsKeyJustReleased: Key transitioned from pressed → released THIS frame.
     *   - Same three states for mouse buttons.
     *
     * Additionally supports action-based queries:
     *   - BindAction():         Maps a GameAction to a physical key.
     *   - IsActionHeld():       Returns true while the bound key is held.
     *   - IsActionJustPressed(): Returns true on the first frame the bound key is pressed.
     *
     * Usage: Call Update() once per frame BEFORE any game logic reads input.
     * The Engine's main loop handles this automatically.
     */
    class InputManager {
    public:
        InputManager();

        /**
         * Snapshots the current keyboard and mouse state.
         * Must be called exactly once per frame, before OnFixedUpdate().
         * If windowHasFocus is false, it ignores inputs and clears the state.
         */
        void Update(bool windowHasFocus);

        // --- Keyboard (raw key queries) ---

        /** Returns true every frame while the key is held down. */
        [[nodiscard]] auto IsKeyHeld(sf::Keyboard::Key key) const -> bool;

        /** Returns true only on the first frame the key is pressed (released → pressed). */
        [[nodiscard]] auto IsKeyJustPressed(sf::Keyboard::Key key) const -> bool;

        /** Returns true only on the frame the key is released (pressed → released). */
        [[nodiscard]] auto IsKeyJustReleased(sf::Keyboard::Key key) const -> bool;

        // --- Mouse ---

        /** Returns true every frame while the mouse button is held down. */
        [[nodiscard]] auto IsMouseButtonHeld(sf::Mouse::Button button) const -> bool;

        /** Returns true only on the first frame the mouse button is pressed. */
        [[nodiscard]] auto IsMouseButtonJustPressed(sf::Mouse::Button button) const -> bool;

        /** Returns true only on the frame the mouse button is released. */
        [[nodiscard]] auto IsMouseButtonJustReleased(sf::Mouse::Button button) const -> bool;

        // --- Action Mappings (Req 1.2) ---

        /**
         * Binds a logical game action to a physical keyboard key.
         * Overwrites any previous binding for the same action.
         * @param action The logical action to bind.
         * @param key    The physical key to map it to.
         */
        void BindAction(GameAction action, sf::Keyboard::Key key);

        /**
         * Returns true every frame while the key bound to the action is held down.
         * Returns false if the action has no binding.
         */
        [[nodiscard]] auto IsActionHeld(GameAction action) const -> bool;

        /**
         * Returns true only on the first frame the key bound to the action is pressed.
         * Returns false if the action has no binding.
         */
        [[nodiscard]] auto IsActionJustPressed(GameAction action) const -> bool;

    private:
        static constexpr auto KeyCount = static_cast<std::size_t>(sf::Keyboard::KeyCount);
        static constexpr auto MouseButtonCount = static_cast<std::size_t>(sf::Mouse::ButtonCount);

        std::array<bool, KeyCount> m_CurrentKeys{};
        std::array<bool, KeyCount> m_PreviousKeys{};

        std::array<bool, MouseButtonCount> m_CurrentMouseButtons{};
        std::array<bool, MouseButtonCount> m_PreviousMouseButtons{};

        /** Registry mapping logical GameActions to physical keyboard keys. */
        std::unordered_map<GameAction, sf::Keyboard::Key> m_ActionBindings;
    };

} // namespace Engine
