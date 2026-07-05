#pragma once

#include "Engine/ECS/ISystem.hpp"
#include "Engine/InputManager.hpp"
#include "Engine/SceneManager.hpp"
#include "Engine/Log.hpp"
#include "Game/SceneId.hpp"

namespace Game {

/**
 * InputSystem — Handles non-continuous, one-shot input actions.
 * 
 * Processes player actions like jumping, shooting, or menu confirmations.
 * Decoupled from physical keys, it reads high-level GameActions from the Context.
 */
class InputSystem : public Engine::ISystem {
public:
    void OnUpdate(Engine::World& world, float deltaTime, const Engine::SystemContext& context) override {
        if (context.input.IsActionJustPressed(Engine::GameAction::Jump)) {
            ENGINE_INFO("Action Triggered: Jump");
        }
        if (context.input.IsActionJustPressed(Engine::GameAction::Shoot)) {
            ENGINE_INFO("Action Triggered: Shoot");
        }
        if (context.input.IsActionJustPressed(Engine::GameAction::Confirm)) {
            if (context.sceneManager && context.sceneManager->GetCurrentSceneName() == SceneRegistry::Name(SceneId::Menu)) {
                context.sceneManager->SwitchTo(SceneRegistry::Name(SceneId::Gameplay));
            }
        }
        if (context.input.IsActionJustPressed(Engine::GameAction::Pause)) {
            ENGINE_INFO("Action Triggered: Pause");
        }
    }
};

} // namespace Game
