#pragma once

#include "Engine/ECS/World.hpp"
#include "Engine/ECS/SystemContext.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <memory>

namespace Engine {

/**
 * Base class for all ECS Systems.
 * 
 * A System encapsulates a specific behavior or logic (e.g., Movement, Rendering).
 * Systems are stateless; they operate on the components attached to entities in the World.
 */
class ISystem {
public:
    virtual ~ISystem() = default;

    /** Called once per fixed timestep for deterministic simulation (e.g., physics, movement). */
    virtual void OnFixedUpdate(World& world, float fixedDeltaTime, const SystemContext& context) {}
    
    /** Called every frame for real-time logic (e.g., input handling, animations). */
    virtual void OnUpdate(World& world, float deltaTime, const SystemContext& context) {}
    
    /** Called every frame for rendering, with an interpolation alpha for smooth movement. */
    virtual void OnRender(World& world, sf::RenderWindow& window, float alpha, const SystemContext& context) {}
};

/**
 * Executes a collection of systems in a deterministic order.
 */
class SystemRunner {
public:
    void AddSystem(std::unique_ptr<ISystem> system) {
        m_Systems.push_back(std::move(system));
    }

    void OnFixedUpdate(World& world, float fixedDeltaTime, const SystemContext& context) {
        for (auto& sys : m_Systems) {
            sys->OnFixedUpdate(world, fixedDeltaTime, context);
        }
    }

    void OnUpdate(World& world, float deltaTime, const SystemContext& context) {
        for (auto& sys : m_Systems) {
            sys->OnUpdate(world, deltaTime, context);
        }
    }

    void OnRender(World& world, sf::RenderWindow& window, float alpha, const SystemContext& context) {
        for (auto& sys : m_Systems) {
            sys->OnRender(world, window, alpha, context);
        }
    }

private:
    std::vector<std::unique_ptr<ISystem>> m_Systems;
};

} // namespace Engine
