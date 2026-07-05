#pragma once

#include "Engine/Entity.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <unordered_map>

namespace Engine {

/**
 * DrawablePool — Manages non-POD SFML drawable objects outside the standard ECS pools.
 * 
 * Standard ECS component pools require trivial copiers and dense memory layouts.
 * SFML drawables like sf::RectangleShape and sf::Text have deep heap allocations,
 * virtual tables, and complex constructors. By keeping them in this separate pool,
 * we adhere to ECS purity while still associating visual elements with Entity handles.
 */
class DrawablePool {
public:
    void CreateRectangle(Entity e, float width, float height) {
        auto& rect = m_Rectangles[e.index];
        rect.setSize({width, height});
        rect.setOrigin(width / 2.f, height / 2.f);
    }

    void CreateText(Entity e) {
        m_Texts[e.index] = sf::Text();
    }

    template<typename Callable>
    void WithRectangle(Entity e, Callable&& callable) {
        if (auto it = m_Rectangles.find(e.index); it != m_Rectangles.end()) {
            callable(it->second);
        }
    }

    template<typename Callable>
    void WithText(Entity e, Callable&& callable) {
        if (auto it = m_Texts.find(e.index); it != m_Texts.end()) {
            callable(it->second);
        }
    }

    void Remove(Entity e) {
        m_Rectangles.erase(e.index);
        m_Texts.erase(e.index);
    }

private:
    std::unordered_map<uint32_t, sf::RectangleShape> m_Rectangles;
    std::unordered_map<uint32_t, sf::Text> m_Texts;
};

} // namespace Engine
