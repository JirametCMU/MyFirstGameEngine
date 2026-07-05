#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Engine/InputManager.hpp"
#include "Engine/ResourceTypes.hpp"
#include "Engine/FPSCounter.hpp"
#include "Engine/DisplaySyncController.hpp"

namespace Engine {

    /**
     * Application — The core engine class that manages the game lifecycle.
     *
     * Creates an SFML window, runs the main game loop, handles events,
     * and provides a virtual resolution system with letterbox scaling.
     * Owns all engine subsystems: InputManager and ResourceManagers (Textures, Fonts, Sounds).
     * Game classes inherit from this and override OnStart/OnUpdate/OnFixedUpdate/OnRender/OnShutdown.
     *
     * Shortcut keys (handled by Engine):
     *   R — Toggle FPS counter overlay
     *   V — Toggle V-Sync on/off
     */
    class Application {
    public:
        Application(std::string name, unsigned int width = 800, unsigned int height = 600);
        virtual ~Application();

        // Prevent copying
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Run the main game loop
        void Run();

    protected:
        // Lifecycle methods to be overridden by the Game
        virtual void OnStart() {}
        virtual void OnUpdate(float /*deltaTime*/) {}
        virtual void OnFixedUpdate(float /*fixedDeltaTime*/) {}
        virtual void OnRender(sf::RenderWindow&, float /*alpha*/) {}
        virtual void OnShutdown() {}

        // Exit the application
        void Close();

        // Get window reference
        sf::RenderWindow& GetWindow() { return m_Window; }

        // Get the logical design resolution
        sf::Vector2f GetDesignSize() const { return {m_DesignWidth, m_DesignHeight}; }

        // Get the input manager for querying keyboard and mouse state
        [[nodiscard]] auto GetInput() const -> const InputManager& { return m_Input; }

        // Get the input manager for configuring action bindings
        [[nodiscard]] auto GetInput() -> InputManager& { return m_Input; }

        // --- Resource Managers ---
        // Get the texture manager for loading and caching image assets
        [[nodiscard]] auto GetTextures() -> TextureManager& { return m_Textures; }

        // Get the font manager for loading and caching font assets
        [[nodiscard]] auto GetFonts() -> FontManager& { return m_Fonts; }

        // Get the sound buffer manager for loading and caching audio assets
        [[nodiscard]] auto GetSounds() -> SoundBufferManager& { return m_Sounds; }

        // Get the display sync controller for pacing settings
        [[nodiscard]] auto GetDisplaySync() -> DisplaySyncController& { return m_DisplaySync; }

        // Set the fixed timestep frequency
        void SetFixedTimestepHz(int hz) {
            if (hz > 0) m_TargetFixedTimestep = 1.0f / static_cast<float>(hz);
        }

    private:
        // Recalculate the viewport to maintain aspect ratio (letterbox)
        void ApplyLetterbox();

        std::string m_Name;
        sf::RenderWindow m_Window;
        sf::View m_GameView;
        float m_DesignWidth;
        float m_DesignHeight;
        bool m_HasFocus = true;
        float m_TargetFixedTimestep = 1.0f / 60.0f;

        // Engine subsystems
        // NOTE: Declaration order matters for destruction safety.
        // FontManager must be declared BEFORE FPSCounter so that fonts
        // outlive the sf::Text objects that reference them.
        InputManager m_Input;
        TextureManager m_Textures;
        FontManager m_Fonts;
        SoundBufferManager m_Sounds;
        FPSCounter m_FPSCounter;
        DisplaySyncController m_DisplaySync;
    };

} // namespace Engine
