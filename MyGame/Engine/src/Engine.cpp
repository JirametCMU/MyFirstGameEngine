#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include <iostream>
#include <algorithm>

namespace Engine {

    // Initializes the core application, setting up the render window and default configuration.
    Application::Application(std::string name, unsigned int width, unsigned int height)
        : m_Name(std::move(name))
        , m_DesignWidth(static_cast<float>(width))
        , m_DesignHeight(static_cast<float>(height))
    {
        Log::Init();
        ENGINE_INFO("[Engine] Initializing: {}", m_Name);

        // Create a resizable window
        m_Window.create(
            sf::VideoMode(width, height),
            m_Name,
            sf::Style::Default
        );

        // Apply default display sync settings (Phase 1 hardcoded defaults)
        DisplaySyncSettings displaySettings;
        displaySettings.mode = EngineConfig::kDefaultSyncMode;
        displaySettings.fpsCap = EngineConfig::kDefaultFpsCap;
        m_DisplaySync.Apply(m_Window, displaySettings);

        // Set up the game view with the logical (design) resolution
        m_GameView.setSize(m_DesignWidth, m_DesignHeight);
        m_GameView.setCenter(m_DesignWidth / 2.0f, m_DesignHeight / 2.0f);

        ApplyLetterbox();
    }

    Application::~Application()
    {
        ENGINE_INFO("[Engine] Shutdown complete.");
    }

    // Flags the main window to close, terminating the engine loop gracefully.
    void Application::Close()
    {
        m_Window.close();
    }

    // Computes and applies letterbox/pillarbox viewport settings based on the current window size
    // to maintain the original aspect ratio specified by the design width and height.
    void Application::ApplyLetterbox()
    {
        sf::Vector2u windowSize = m_Window.getSize();
        float windowWidth  = static_cast<float>(windowSize.x);
        float windowHeight = static_cast<float>(windowSize.y);

        float designRatio = m_DesignWidth / m_DesignHeight;
        float windowRatio = windowWidth / windowHeight;

        float viewportWidth  = 1.0f;
        float viewportHeight = 1.0f;
        float viewportX = 0.0f;
        float viewportY = 0.0f;

        if (windowRatio > designRatio)
        {
            // Window is wider than design -> black bars on left/right (pillarbox)
            viewportWidth = designRatio / windowRatio;
            viewportX = (1.0f - viewportWidth) / 2.0f;
        }
        else if (windowRatio < designRatio)
        {
            // Window is taller than design -> black bars on top/bottom (letterbox)
            viewportHeight = windowRatio / designRatio;
            viewportY = (1.0f - viewportHeight) / 2.0f;
        }

        m_GameView.setViewport(sf::FloatRect(viewportX, viewportY, viewportWidth, viewportHeight));
        m_Window.setView(m_GameView);
    }

    // The primary execution loop of the Engine.
    // Handles window events, fixed/variable timesteps, rendering, and input polling.
    void Application::Run()
    {
        // Let the game initialize (load assets, bind inputs, register scenes)
        OnStart();

        // After OnStart(), fonts may have been loaded into the FontManager.
        // Provide the default font to the FPS counter overlay.
        if (m_Fonts.HasDefault())
        {
            m_FPSCounter.SetFont(m_Fonts.GetDefault());
        }
        else
        {
            ENGINE_WARN("[Engine] No fonts loaded. FPS counter will not render.\n         Place a .ttf or .otf file in Assets/Fonts/ to enable it.");
        }

        sf::Clock clock;
        float accumulator = 0.0f;

        // Main game loop with fixed-timestep accumulator
        while (m_Window.isOpen())
        {
            sf::Event event;
            while (m_Window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    Close();
                }
                else if (event.type == sf::Event::Resized)
                {
                    ApplyLetterbox();
                }
                else if (event.type == sf::Event::GainedFocus)
                {
                    m_HasFocus = true;
                    clock.restart(); // Prevent large delta after focus
                }
                else if (event.type == sf::Event::LostFocus)
                {
                    m_HasFocus = false;
                }
            }

            float deltaTime = clock.restart().asSeconds();

            // Clamp large delta spikes to prevent spiral-of-death
            if (deltaTime > 0.25f) deltaTime = 0.25f;

            accumulator += deltaTime;

            // Update input state (snapshot current frame vs previous frame)
            // Passes window focus state so inputs are ignored when tabbed out.
            m_Input.Update(m_HasFocus);

            // --- Engine shortcut keys ---

            // Toggle FPS counter
            if (m_Input.IsActionJustPressed(Engine::GameAction::ToggleDebugOverlay))
            {
                m_FPSCounter.Toggle();
            }

            // Toggle V-Sync
            if (m_Input.IsActionJustPressed(Engine::GameAction::ToggleVSync))
            {
                auto settings = m_DisplaySync.GetSettings();
                settings.mode = (settings.mode == SyncMode::VSync) ? SyncMode::Off : SyncMode::VSync;
                m_DisplaySync.Apply(m_Window, settings);
            }

            // Update FPS counter (uses real frame delta for accurate measurement)
            m_FPSCounter.Update(deltaTime);

            // Real-time update for inputs, animations, etc (1-to-1 with input polling)
            OnUpdate(deltaTime);

            // Fixed-timestep physics/logic updates at exactly target Hz
            constexpr int kMaxPhysicsSubsteps = 5;

            int substepCount = 0;
            while (accumulator >= m_TargetFixedTimestep && substepCount < kMaxPhysicsSubsteps)
            {
                OnFixedUpdate(m_TargetFixedTimestep);
                accumulator -= m_TargetFixedTimestep;
                ++substepCount;
            }

            // Compute alpha for render interpolation
            float alpha = accumulator / m_TargetFixedTimestep;

            // Clear with black (fills the letterbox bars)
            m_Window.clear(sf::Color::Black);

            // Apply game view and render game content
            m_Window.setView(m_GameView);
            OnRender(m_Window, alpha);

            // Render FPS overlay (uses its own screen-space view)
            m_FPSCounter.Render(m_Window);

            m_Window.display();
        }

        OnShutdown();
    }

} // namespace Engine
