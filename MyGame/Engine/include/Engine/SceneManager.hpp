#pragma once

#include "Engine/Scene.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace Engine {

    /**
     * SceneManager — Manages game scene lifecycle and transitions.
     *
     * Holds a registry of named scenes and maintains a pointer to the
     * currently active scene. Supports adding scenes by name and switching
     * between them. On transition, the old scene's OnExit() is called
     * followed by the new scene's OnEnter().
     *
     * Usage:
     *   SceneManager scenes;
     *   scenes.AddScene("menu", std::make_unique<MainMenuScene>(...));
     *   scenes.AddScene("gameplay", std::make_unique<GameplayScene>(...));
     *   scenes.SwitchTo("menu");
     */
    class SceneManager {
    public:
        SceneManager() = default;
        ~SceneManager();

        // Prevent copying
        SceneManager(const SceneManager&) = delete;
        auto operator=(const SceneManager&) -> SceneManager& = delete;

        /**
         * Registers a scene under a unique name.
         * Ownership is transferred to the SceneManager.
         * @param name  A unique identifier for the scene (e.g., "menu", "gameplay").
         * @param scene The scene instance to register.
         */
        void AddScene(const std::string& name, std::unique_ptr<Scene> scene);

        /**
         * Transitions to the scene registered under the given name.
         * Calls OnExit() on the current scene (if any), then OnEnter() on the new one.
         * @param name The name of the scene to switch to.
         */
        void SwitchTo(const std::string& name);

        /**
         * Updates the active scene every real-time frame (for inputs/animations).
         * @param deltaTime The elapsed time since last frame.
         */
        void Update(float deltaTime);

        /**
         * Updates the active scene at the fixed timestep (for physics).
         * @param fixedDeltaTime The fixed timestep duration.
         */
        void FixedUpdate(float fixedDeltaTime);

        /**
         * Renders the active scene.
         * @param window The SFML render window.
         * @param alpha The interpolation factor for rendering.
         */
        void Render(sf::RenderWindow& window, float alpha);

        /** Returns the name of the currently active scene, or empty if none. */
        [[nodiscard]] auto GetCurrentSceneName() const -> const std::string& { return m_CurrentName; }

    private:
        std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
        Scene* m_ActiveScene = nullptr;
        std::string m_CurrentName;
    };

} // namespace Engine
