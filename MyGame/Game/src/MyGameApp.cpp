#include "Game/MyGameApp.hpp"
#include "Game/MainMenuScene.hpp"
#include "Game/GameplayScene.hpp"
#include "Engine/Log.hpp"
#include "Engine/PathService.hpp"

namespace Game {

    MyGameApp::MyGameApp()
        : Engine::Application("My C++ Game", 800, 600)
    {
    }

    /**
     * Sets up fonts, input action bindings, and scenes.
     * Fonts are auto-discovered from Assets/Fonts/ so the user can swap fonts
     * by simply dropping a new .ttf/.otf file into that folder.
     */
    void MyGameApp::OnStart()
    {
        ENGINE_INFO("[Game] Initializing...");

        // --- Load settings (Req 2.1) ---
        auto settingsPath = Engine::PathService::Resolve("Assets/Config/settings.json");
        m_Settings = SettingsManager::Load(settingsPath);
        // Save back defaults if file didn't exist
        SettingsManager::Save(m_Settings, settingsPath);

        // Apply display settings
        DisplaySettingsApplicator::Apply(m_Settings.display, GetWindow(), GetDisplaySync());

        // Apply gameplay fixed timestep
        SetFixedTimestepHz(m_Settings.gameplay.fixedTimestepHz);

        // --- Load all fonts from the Assets/Fonts/ directory (Req 1.6) ---
        // Any .ttf or .otf file placed there is automatically loaded.
        // The first font (alphabetically) becomes the default used everywhere.
        auto fontsPath = Engine::PathService::Resolve("Assets/Fonts/").string();
        GetFonts().LoadDirectory(fontsPath, {".ttf", ".otf"});

        // --- Configure input action bindings (Req 1.2 & 2.1) ---
        auto& input = GetInput();
        InputSettingsApplicator::Apply(m_Settings.controls, input);
        ENGINE_INFO("[Game] Input action bindings configured.");

        // --- Register scenes (Req 1.4 & 2.2) ---
        auto designSize = GetDesignSize();

        m_SceneManager.AddScene(SceneRegistry::Name(SceneId::Menu),
            std::make_unique<MainMenuScene>(m_SceneManager, GetInput(), GetFonts(), designSize));

        m_SceneManager.AddScene(SceneRegistry::Name(SceneId::Gameplay),
            std::make_unique<GameplayScene>(m_SceneManager, GetInput(), GetFonts(), designSize, m_Settings.gameplay));

        // Start on the main menu
        m_SceneManager.SwitchTo(SceneRegistry::Name(SceneId::Menu));
    }

    /**
     * Delegates real-time updates (inputs, animation) to the active scene.
     */
    void MyGameApp::OnUpdate(float deltaTime)
    {
        m_SceneManager.Update(deltaTime);
    }

    /**
     * Delegates the fixed-timestep update (physics) to the active scene.
     * Called at exactly 60 Hz by the Engine's accumulator loop (Req 1.1).
     */
    void MyGameApp::OnFixedUpdate(float fixedDeltaTime)
    {
        m_SceneManager.FixedUpdate(fixedDeltaTime);
    }

    /** Delegates rendering to the active scene. */
    void MyGameApp::OnRender(sf::RenderWindow& window, float alpha)
    {
        m_SceneManager.Render(window, alpha);
    }

    /** Called when the game is shutting down — log cleanup message. */
    void MyGameApp::OnShutdown()
    {
        ENGINE_INFO("[Game] Saving state and shutting down...");
        
        // Save current display sync mode which may have been toggled at runtime
        m_Settings.display.syncMode = GetDisplaySync().GetSettings().mode;
        auto settingsPath = Engine::PathService::Resolve("Assets/Config/settings.json");
        SettingsManager::Save(m_Settings, settingsPath);
    }

} // namespace Game
