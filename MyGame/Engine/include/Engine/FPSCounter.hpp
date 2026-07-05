#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace Engine {

    /**
     * FPSCounter — A debug overlay that displays the current frames-per-second.
     *
     * How it works:
     *   - Accumulates frame count and elapsed time each frame via Update().
     *   - Every 0.5 seconds, recalculates the average FPS and updates the display text.
     *   - Renders using a dedicated screen-space UI view, so the counter always stays
     *     fixed in the top-right corner regardless of game camera movement or letterboxing.
     *   - Can be toggled on/off with Toggle(). Hidden by default.
     *
     * The font is supplied externally via SetFont(). The FPSCounter does NOT load
     * fonts itself — the Application provides the font from its FontManager after
     * the game loads assets in OnStart(). This keeps font management centralized
     * and cross-platform.
     */
    class FPSCounter {
    public:
        FPSCounter() = default;

        /**
         * Provides the font for the FPS text display and configures text styling.
         * Must be called before the counter can render. The referenced font
         * must outlive this FPSCounter instance.
         *
         * @param font A reference to an sf::Font managed by the FontManager.
         */
        void SetFont(const sf::Font& font);

        /**
         * Accumulates frame time. Called once per frame with the frame's delta time.
         * Recalculates the displayed FPS value every 0.5 seconds for a stable readout.
         */
        void Update(float deltaTime);

        /**
         * Renders the FPS text in the top-right corner of the window.
         * Uses a temporary screen-space view so the text is not affected by the game camera.
         * Does nothing if the counter is hidden or no font has been set.
         */
        void Render(sf::RenderWindow& window);

        /** Toggles the FPS counter visibility on/off. */
        void Toggle();

        /** Returns true if the FPS counter is currently visible. */
        [[nodiscard]] auto IsVisible() const -> bool { return m_Visible; }

    private:
        sf::Text m_Text;

        bool m_FontReady = false;
        bool m_Visible = true;

        int m_FrameCount = 0;
        float m_ElapsedTime = 0.0f;
        float m_CurrentFPS = 0.0f;

        /** How often (in seconds) the displayed FPS value is recalculated. */
        static constexpr float UpdateInterval = 0.5f;
    };

} // namespace Engine
