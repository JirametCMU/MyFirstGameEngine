#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <algorithm>

namespace Engine::Math {

    inline constexpr float kEpsilon = 1e-6f;

    /** Clamps a value strictly between min and max. */
    [[nodiscard]] constexpr float Clamp(float value, float min, float max) noexcept {
        return std::clamp(value, min, max);
    }

    /** Linearly interpolates between a and b by t (0.0 to 1.0). */
    [[nodiscard]] constexpr float Lerp(float a, float b, float t) noexcept {
        return a + (b - a) * t;
    }

    /** Returns the Euclidean length of a 2D vector. */
    [[nodiscard]] inline float Length(sf::Vector2f v) noexcept {
        return std::hypot(v.x, v.y);
    }

    /** Returns the squared length of a 2D vector (avoids square root). */
    [[nodiscard]] inline float LengthSquared(sf::Vector2f v) noexcept {
        return v.x * v.x + v.y * v.y;
    }

    /** 
     * Normalizes a vector. If the vector is too small, returns {0,0}.
     */
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

    /** 
     * Normalizes a vector. If the vector is too small, returns the specified fallback vector.
     */
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

    /** Normalizes a digital 8-way input vector (e.g. WASD keys). */
    [[nodiscard]] inline sf::Vector2f NormalizeDigitalDirection(sf::Vector2f v) noexcept {
        return NormalizeOrZero(v);
    }

    /** Linearly interpolates between two vectors. */
    [[nodiscard]] inline sf::Vector2f Lerp(
        sf::Vector2f a, sf::Vector2f b, float t) noexcept
    {
        return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
    }

    /** Clamps a vector's magnitude to maxLength. */
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

    /** Checks if two floats are approximately equal within a tolerance. */
    [[nodiscard]] inline bool ApproximatelyEqual(
        float a, float b, float epsilon = 1e-5f) noexcept
    {
        return std::fabs(a - b) <= epsilon;
    }

    /** Computes the dot product of two 2D vectors. */
    [[nodiscard]] inline float Dot(sf::Vector2f a, sf::Vector2f b) noexcept {
        return a.x * b.x + a.y * b.y;
    }

} // namespace Engine::Math
