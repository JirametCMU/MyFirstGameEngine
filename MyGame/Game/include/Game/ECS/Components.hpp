#pragma once

#include <cstdint>
#include <string>
#include <SFML/Graphics/Color.hpp>

namespace Game {

/**
 * PlayerTag — Tag component identifying the entity controlled by the player.
 */
struct PlayerTag {
    uint8_t pad{0}; // non-empty so it's a standard layout type
};

/**
 * MoveSpeed — Defines the base movement velocity multiplier for an entity.
 */
struct MoveSpeed {
    float unitsPerSecond{0.0f};
};

/**
 * RectangleShapeVisual — Holds data to render a basic colored rectangle.
 */
struct RectangleShapeVisual {
    float width{0.0f};
    float height{0.0f};
    sf::Color fill{sf::Color::White};
};

/**
 * TextVisual — Holds data to render UI text.
 */
struct TextVisual {
    std::string text;
    unsigned charSize{30};
    sf::Color fill{sf::Color::White};
};

/**
 * PulseAnimation — Component to animate opacity over time.
 * Used for UI elements like "Press Enter to Play".
 */
struct PulseAnimation {
    float timer{0.0f};
    float speed{1.0f};
    float minAlpha{0.0f};
    float maxAlpha{255.0f};
};

} // namespace Game
