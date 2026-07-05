#pragma once

#include <cstdint>

namespace Engine {

struct Entity {
    uint32_t index{0};
    uint32_t generation{0};

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
