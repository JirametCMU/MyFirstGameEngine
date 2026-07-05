#include "Engine/FPSCounter.hpp"
#include "Engine/Log.hpp"
#include <sstream>
#include <iomanip>

namespace Engine {

    /**
     * Configures the FPS text with an externally-provided font.
     * The font must outlive this FPSCounter (guaranteed when owned by FontManager
     * in the same Application, with correct member declaration order).
     */
    void FPSCounter::SetFont(const sf::Font& font)
    {
        m_Text.setFont(font);
        m_Text.setCharacterSize(16);
        m_Text.setFillColor(sf::Color::Yellow);
        m_Text.setOutlineColor(sf::Color::Black);
        m_Text.setOutlineThickness(1.0f);
        m_Text.setString("FPS: 0");

        m_FontReady = true;
        ENGINE_INFO("[FPSCounter] Font set. FPS counter ready.");
    }

    /**
     * Accumulates frames and elapsed time. Every UpdateInterval (0.5s),
     * recalculates the average FPS and updates the displayed string.
     * This smoothing prevents the counter from flickering wildly each frame.
     */
    void FPSCounter::Update(float deltaTime)
    {
        if (!m_FontReady || !m_Visible)
        {
            return;
        }

        m_FrameCount++;
        m_ElapsedTime += deltaTime;

        // Recalculate FPS at the update interval
        if (m_ElapsedTime >= UpdateInterval)
        {
            m_CurrentFPS = static_cast<float>(m_FrameCount) / m_ElapsedTime;
            m_FrameCount = 0;
            m_ElapsedTime = 0.0f;

            // Format the FPS string with 1 decimal place
            std::ostringstream oss;
            oss << "FPS: " << std::fixed << std::setprecision(1) << m_CurrentFPS;
            m_Text.setString(oss.str());
        }
    }

    /**
     * Renders the FPS text in the top-right corner using a temporary screen-space view.
     * This ensures the counter stays fixed on screen regardless of the game camera position
     * or the letterbox viewport. The original view is saved and restored after drawing.
     */
    void FPSCounter::Render(sf::RenderWindow& window)
    {
        if (!m_FontReady || !m_Visible)
        {
            return;
        }

        // Save the current view
        auto previousView = window.getView();

        // Switch to a pixel-perfect screen-space view (full window size)
        auto windowSize = window.getSize();
        sf::View uiView{sf::FloatRect{
            0.0f, 0.0f,
            static_cast<float>(windowSize.x),
            static_cast<float>(windowSize.y)
        }};
        window.setView(uiView);

        // Position in the top-right corner with a small margin
        constexpr float margin = 10.0f;
        auto textBounds = m_Text.getLocalBounds();
        m_Text.setPosition(
            static_cast<float>(windowSize.x) - textBounds.width - margin,
            margin
        );

        window.draw(m_Text);

        // Restore the previous view
        window.setView(previousView);
    }

    /** Toggles visibility on/off. Logs the state change. */
    void FPSCounter::Toggle()
    {
        m_Visible = !m_Visible;
        ENGINE_INFO("[FPSCounter] {}", (m_Visible ? "Enabled" : "Disabled"));
    }

} // namespace Engine
