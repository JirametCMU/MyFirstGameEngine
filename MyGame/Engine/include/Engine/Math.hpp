#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <algorithm>

namespace Engine::Math {

    inline constexpr float kEpsilon = 1e-6f;

    [[nodiscard]] constexpr float Clamp(float value, float min, float max) noexcept {
        return std::clamp(value, min, max);
    }

    [[nodiscard]] constexpr float Lerp(float a, float b, float t) noexcept {
        return a + (b - a) * t;
    }

    [[nodiscard]] inline float Length(sf::Vector2f v) noexcept {
        return std::hypot(v.x, v.y);
    }

    [[nodiscard]] inline float LengthSquared(sf::Vector2f v) noexcept {
        return v.x * v.x + v.y * v.y;
    }

    [[nodiscard]] inline sf::Vector2f NormalizeOrZero(
        sf::Vector2f v,
        float epsilon = kEpsilon) noexcept
    {
        const float lenSq = LengthSquared(v);
        if (lenSq <= epsilon * epsilon) {
            return {0.0f, 0.0f};
        }
        const float invLen = 1.0f / std::sqrt(lenSq);
        return {v.x * invLen, v.y * invLen};
    }

    [[nodiscard]] inline sf::Vector2f Normalize(
        sf::Vector2f v, sf::Vector2f fallback,
        float epsilon = kEpsilon) noexcept
    {
        const float lenSq = LengthSquared(v);
        if (lenSq <= epsilon * epsilon) {
            return fallback;
        }
        const float invLen = 1.0f / std::sqrt(lenSq);
        return {v.x * invLen, v.y * invLen};
    }

    [[nodiscard]] inline sf::Vector2f NormalizeDigitalDirection(sf::Vector2f v) noexcept {
        return NormalizeOrZero(v);
    }

    [[nodiscard]] inline sf::Vector2f Lerp(
        sf::Vector2f a, sf::Vector2f b, float t) noexcept
    {
        return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
    }

    [[nodiscard]] inline sf::Vector2f ClampMagnitude(
        sf::Vector2f v, float maxLength, float epsilon = kEpsilon) noexcept
    {
        const float lenSq = LengthSquared(v);
        const float maxSq = maxLength * maxLength;
        if (lenSq <= maxSq || lenSq <= epsilon * epsilon) {
            return v;
        }
        return v * (maxLength / std::sqrt(lenSq));
    }

    [[nodiscard]] inline bool ApproximatelyEqual(
        float a, float b, float epsilon = 1e-5f) noexcept
    {
        return std::fabs(a - b) <= epsilon;
    }

    [[nodiscard]] inline float Dot(sf::Vector2f a, sf::Vector2f b) noexcept {
        return a.x * b.x + a.y * b.y;
    }

} // namespace Engine::Math
