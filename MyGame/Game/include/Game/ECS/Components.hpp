#pragma once

#include <cstdint>
#include <string>
#include <SFML/Graphics/Color.hpp>

namespace Game {

struct PlayerTag {
    uint8_t pad{0}; // non-empty so it's a standard layout type
};

struct MoveSpeed {
    float unitsPerSecond{0.0f};
};

struct RectangleShapeVisual {
    float width{0.0f};
    float height{0.0f};
    sf::Color fill{sf::Color::White};
};

struct TextVisual {
    std::string text;
    unsigned charSize{30};
    sf::Color fill{sf::Color::White};
};

struct PulseAnimation {
    float timer{0.0f};
    float speed{1.0f};
    float minAlpha{0.0f};
    float maxAlpha{255.0f};
};

} // namespace Game
