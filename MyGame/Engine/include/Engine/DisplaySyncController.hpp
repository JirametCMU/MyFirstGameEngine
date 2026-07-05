#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

namespace Engine {
    /** Represents different monitor synchronization strategies. */
    enum class SyncMode { Off, VSync, GSync };

    /** Configuration for frame pacing and vertical synchronization. */
    struct DisplaySyncSettings {
        SyncMode mode = SyncMode::Off;
        unsigned int fpsCap = 120; // 0 = unlimited
    };

    namespace EngineConfig {
        constexpr SyncMode kDefaultSyncMode = SyncMode::Off;
        constexpr unsigned int kDefaultFpsCap = 120;
    }

    /**
     * DisplaySyncController — Manages frame rate limits and VSync settings.
     * 
     * Ensures that SFML's window settings are updated consistently. 
     * If VSync is enabled, manual framerate limits are disabled, and vice versa.
     */
    class DisplaySyncController {
    public:
        /** Applies the given synchronization settings to the provided render window. */
        void Apply(sf::RenderWindow& window, const DisplaySyncSettings& settings);

        /** Retrieves the currently active synchronization settings. */
        [[nodiscard]] const DisplaySyncSettings& GetSettings() const noexcept;
    private:
        DisplaySyncSettings m_Settings{};
        void ApplyToWindow(sf::RenderWindow& window);
    };

} // namespace Engine
