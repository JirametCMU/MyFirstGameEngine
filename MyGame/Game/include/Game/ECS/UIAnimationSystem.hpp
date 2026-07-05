#pragma once

#include "Engine/ECS/ISystem.hpp"
#include "Game/ECS/Components.hpp"
#include <cmath>

namespace Game {

/**
 * UIAnimationSystem — Drives UI-specific animations.
 * 
 * Iterates over entities with PulseAnimation and TextVisual components
 * to modulate their alpha over time using a sine wave.
 */
class UIAnimationSystem : public Engine::ISystem {
public:
    void OnUpdate(Engine::World& world, float deltaTime, const Engine::SystemContext& context) override {
        world.Each<PulseAnimation, TextVisual>(
            [&](Engine::Entity e, PulseAnimation& anim, TextVisual& visual) {
                anim.timer += deltaTime * anim.speed;
                float alpha = (std::sin(anim.timer * 3.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
                auto currentAlpha = static_cast<sf::Uint8>(anim.minAlpha + (anim.maxAlpha - anim.minAlpha) * alpha);
                
                visual.fill.a = currentAlpha;
            }
        );
    }
};

} // namespace Game
