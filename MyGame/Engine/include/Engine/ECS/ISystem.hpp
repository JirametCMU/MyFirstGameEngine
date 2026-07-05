#pragma once

#include "Engine/ECS/World.hpp"
#include "Engine/ECS/SystemContext.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <memory>

namespace Engine {

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void OnFixedUpdate(World& world, float fixedDeltaTime, const SystemContext& context) {}
    virtual void OnUpdate(World& world, float deltaTime, const SystemContext& context) {}
    virtual void OnRender(World& world, sf::RenderWindow& window, float alpha, const SystemContext& context) {}
};

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
