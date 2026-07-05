#include "Engine/DisplaySyncController.hpp"
#include "Engine/Log.hpp"

namespace Engine {

    void DisplaySyncController::Apply(sf::RenderWindow& window, const DisplaySyncSettings& settings) {
        m_Settings = settings;
        ApplyToWindow(window);
    }

    const DisplaySyncSettings& DisplaySyncController::GetSettings() const noexcept {
        return m_Settings;
    }

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
