#pragma once

#include "Engine/ECS/ISystem.hpp"
#include "Engine/ECS/DrawablePool.hpp"
#include "Engine/Transform2D.hpp"
#include "Game/ECS/Components.hpp"
#include "Engine/ResourceTypes.hpp"

namespace Game {

/**
 * RenderSystem — Handles drawing of visual components.
 * 
 * Iterates over entities with visual components and applies interpolated 
 * transforms to bridge the gap between the fixed physics timestep and 
 * variable display framerate. Pushes updates to the Engine's DrawablePool.
 */
class RenderSystem : public Engine::ISystem {
public:
    explicit RenderSystem(Engine::DrawablePool& pool) : m_Pool(pool) {}

    void OnRender(Engine::World& world, sf::RenderWindow& window, float alpha, const Engine::SystemContext& context) override {
        world.Each<Engine::Transform2D, Engine::PreviousTransform2D, RectangleShapeVisual>(
            [&](Engine::Entity e, Engine::Transform2D& current, Engine::PreviousTransform2D& prev, RectangleShapeVisual& visual) {
                Engine::Transform2D renderTransform = Engine::Lerp(prev, current, alpha);
                m_Pool.WithRectangle(e, [&](sf::RectangleShape& shape) {
                    shape.setFillColor(visual.fill);
                    renderTransform.ApplyTo(shape);
                    window.draw(shape);
                });
            }
        );

        world.Each<Engine::Transform2D, TextVisual>(
            [&](Engine::Entity e, Engine::Transform2D& transform, TextVisual& visual) {
                m_Pool.WithText(e, [&](sf::Text& text) {
                    if (context.fonts.HasDefault()) {
                        text.setFont(context.fonts.GetDefault());
                    }
                    text.setString(visual.text);
                    text.setCharacterSize(visual.charSize);
                    text.setFillColor(visual.fill);
                    
                    transform.ApplyTo(text);
                    window.draw(text);
                });
            }
        );
    }

private:
    Engine::DrawablePool& m_Pool;
};

} // namespace Game
