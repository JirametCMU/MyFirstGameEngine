#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include "Engine/Math.hpp"

namespace Engine {

struct Transform2D {
    sf::Vector2f position{0.0f, 0.0f};
    float rotation{0.0f};           // degrees
    sf::Vector2f scale{1.0f, 1.0f};

    void ApplyTo(sf::Transformable& drawable) const noexcept {
        drawable.setPosition(position);
        drawable.setRotation(rotation);
        
        // Clamp scale to a minimum of 1e-6f to prevent SFML transform breakage
        sf::Vector2f safeScale = scale;
        if (safeScale.x > -1e-6f && safeScale.x <= 0.0f) safeScale.x = -1e-6f;
        else if (safeScale.x >= 0.0f && safeScale.x < 1e-6f) safeScale.x = 1e-6f;
        
        if (safeScale.y > -1e-6f && safeScale.y <= 0.0f) safeScale.y = -1e-6f;
        else if (safeScale.y >= 0.0f && safeScale.y < 1e-6f) safeScale.y = 1e-6f;
        
        drawable.setScale(safeScale);
    }
};

/**
 * PreviousTransform2D — Stores the transform state from the previous fixed timestep.
 * 
 * Essential for rendering interpolation. During OnRender, the system reads both
 * PreviousTransform2D and Transform2D and lerps between them based on the accumulator alpha.
 */
struct PreviousTransform2D : Transform2D {};

[[nodiscard]] inline Transform2D Lerp(
    const Transform2D& a, const Transform2D& b, float t) noexcept
{
    Transform2D result;
    result.position = Engine::Math::Lerp(a.position, b.position, t);
    
    float delta = b.rotation - a.rotation;
    while (delta > 180.0f)  delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;
    result.rotation = a.rotation + delta * t;
    
    result.scale = Engine::Math::Lerp(a.scale, b.scale, t);
    
    return result;
}

[[nodiscard]] inline Transform2D Lerp(
    const PreviousTransform2D& a, const Transform2D& b, float t) noexcept
{
    return Lerp(static_cast<const Transform2D&>(a), b, t);
}

} // namespace Engine
