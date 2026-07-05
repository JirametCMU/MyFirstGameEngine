#include "Engine/DisplaySyncController.hpp"
#include "Engine/Log.hpp"

namespace Engine {

    // Applies settings and immediately configures the SFML window.
    void DisplaySyncController::Apply(sf::RenderWindow& window, const DisplaySyncSettings& settings) {
        m_Settings = settings;
        ApplyToWindow(window);
    }

    // Returns the currently active display settings.
    const DisplaySyncSettings& DisplaySyncController::GetSettings() const noexcept {
        return m_Settings;
    }

    // Internal helper that resolves the conflict between SFML's VSync and FPS limits.
    // SFML requires framerate limits to be set to 0 when VSync is on.
    void DisplaySyncController::ApplyToWindow(sf::RenderWindow& window) {
        switch (m_Settings.mode) {
        case SyncMode::VSync:
            window.setFramerateLimit(0);              // MUST clear cap first
            window.setVerticalSyncEnabled(true);
            ENGINE_INFO("[DisplaySync] V-Sync Enabled (FPS cap disabled)");
            break;
        case SyncMode::GSync:
            ENGINE_WARN("[DisplaySync] Warning: GSync not fully supported yet. Falling back to Off with FPS cap.");
            // Fallthrough intentionally
        case SyncMode::Off:
        default:
            window.setVerticalSyncEnabled(false);     // MUST disable V-Sync first
            window.setFramerateLimit(m_Settings.fpsCap);
            if (m_Settings.fpsCap == 0) {
                ENGINE_INFO("[DisplaySync] Sync Off, FPS Unlimited");
            } else {
                ENGINE_INFO("[DisplaySync] Sync Off, FPS Capped to {}", m_Settings.fpsCap);
            }
            break;
        }
    }

} // namespace Engine
