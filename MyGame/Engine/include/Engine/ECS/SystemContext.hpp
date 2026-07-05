#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf {
    class Font;
}

namespace Engine {
    
    class InputManager;
    class SceneManager;
    template <typename T> class ResourceManager;
    using FontManager = ResourceManager<sf::Font>;

    /**
     * SystemContext — Provides dependency injection for ECS systems.
     * 
     * Instead of accessing global singletons (which harms testability and 
     * predictability), systems receive this context object every frame.
     * It contains references to core engine services like Input and Resource Managers.
     */
    struct SystemContext {
        const InputManager& input;
        FontManager& fonts;
        SceneManager* sceneManager{nullptr}; // Game-only transitions
        sf::Vector2f designSize{0.f, 0.f};
    };

} // namespace Engine
