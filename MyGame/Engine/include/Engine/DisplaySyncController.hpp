#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

namespace Engine {
    enum class SyncMode { Off, VSync, GSync };
    struct DisplaySyncSettings {
        SyncMode mode = SyncMode::Off;
        unsigned int fpsCap = 120; // 0 = unlimited
    };

    namespace EngineConfig {
        constexpr SyncMode kDefaultSyncMode = SyncMode::Off;
        constexpr unsigned int kDefaultFpsCap = 120;
    }

    class DisplaySyncController {
    public:
        void Apply(sf::RenderWindow& window, const DisplaySyncSettings& settings);
        [[nodiscard]] const DisplaySyncSettings& GetSettings() const noexcept;
    private:
        DisplaySyncSettings m_Settings{};
        void ApplyToWindow(sf::RenderWindow& window);
    };

} // namespace Engine
