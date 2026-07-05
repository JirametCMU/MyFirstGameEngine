#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

namespace Engine {

    /**
     * Scene — Abstract base interface for all game scenes/screens.
     *
     * Each scene represents a distinct game state (e.g., Main Menu, Gameplay,
     * Pause Screen). The SceneManager owns the active scene and calls these
     * lifecycle methods at the appropriate times:
     *
     *   OnEnter()       — Called once when the scene becomes active.
     *   OnUpdate()      — Called every frame for real-time logic (e.g. input processing).
     *   OnFixedUpdate() — Called at a fixed timestep (60 Hz) for deterministic physics.
     *   OnRender()      — Called every frame for drawing.
     *   OnExit()   — Called once when transitioning away from this scene.
     */
    class Scene {
    public:
        virtual ~Scene() = default;

        /** Called once when the scene becomes the active scene. */
        virtual void OnEnter() = 0;

        /**
         * Called every frame for real-time game logic, animations, and input polling.
         * Using this ensures input 'JustPressed' events are never missed, because
         * real-time updates happen exactly 1-to-1 with input polling frames.
         * @param deltaTime The time elapsed since the last real frame.
         */
        virtual void OnUpdate(float /*deltaTime*/) {}

        /**
         * Called at a fixed rate (60 Hz) for deterministic game logic and physics.
         * @param fixedDeltaTime The fixed timestep duration (1/60th of a second).
         */
        virtual void OnFixedUpdate(float /*fixedDeltaTime*/) {}

        /**
         * Called every frame for rendering.
         * @param window The SFML render window to draw into.
         * @param alpha The interpolation factor for smooth rendering.
         */
        virtual void OnRender(sf::RenderWindow& window, float alpha) = 0;

        /** Called once when the scene is about to be replaced by another scene. */
        virtual void OnExit() = 0;
    };

} // namespace Engine
