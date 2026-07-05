#pragma once

#include <cstdint>

namespace Engine {

/**
 * Entity — An opaque handle representing a unique game object.
 * 
 * Uses a generational index to allow safe reuse of entity slots.
 * If an entity is destroyed and its slot is reused, the generation
 * is incremented so old handles safely invalidate.
 */
struct Entity {
    uint32_t index{0};      ///< Index into the component storage arrays.
    uint32_t generation{0}; ///< Version number to detect stale handles.

    /**
     * Checks if this handle has a non-zero generation.
     * Note: This only means it's structurally valid, not necessarily alive in the World.
     */
    [[nodiscard]] constexpr bool IsValid() const noexcept {
        return generation != 0;
    }

    constexpr bool operator==(const Entity& other) const noexcept {
        return index == other.index && generation == other.generation;
    }

    constexpr bool operator!=(const Entity& other) const noexcept {
        return !(*this == other);
    }
};

} // namespace Engine
