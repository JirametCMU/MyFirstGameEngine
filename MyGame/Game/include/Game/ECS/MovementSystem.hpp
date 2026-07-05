#pragma once

#include "Engine/ECS/ISystem.hpp"
#include "Engine/Transform2D.hpp"
#include "Game/ECS/Components.hpp"
#include "Engine/InputManager.hpp"
#include "Engine/Math.hpp"

namespace Game {

class MovementSystem : public Engine::ISystem {
public:
    void OnFixedUpdate(Engine::World& world, float fixedDeltaTime, const Engine::SystemContext& context) override {
        sf::Vector2f inputDirection{0.0f, 0.0f};
        if (context.input.IsActionHeld(Engine::GameAction::MoveUp))    inputDirection.y -= 1.0f;
        if (context.input.IsActionHeld(Engine::GameAction::MoveDown))  inputDirection.y += 1.0f;
        if (context.input.IsActionHeld(Engine::GameAction::MoveLeft))  inputDirection.x -= 1.0f;
        if (context.input.IsActionHeld(Engine::GameAction::MoveRight)) inputDirection.x += 1.0f;

        inputDirection = Engine::Math::NormalizeDigitalDirection(inputDirection);

        world.Each<Engine::Transform2D, Engine::PreviousTransform2D, MoveSpeed>(
            [&](Engine::Entity e, Engine::Transform2D& current, Engine::PreviousTransform2D& prev, MoveSpeed& speed) {
                static_cast<Engine::Transform2D&>(prev) = current;

                if (world.HasComponent<PlayerTag>(e)) {
                    current.position += inputDirection * speed.unitsPerSecond * fixedDeltaTime;
                }
            }
        );
    }
};

} // namespace Game
