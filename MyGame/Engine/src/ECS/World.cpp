#include "Engine/ECS/World.hpp"

namespace Engine {

// Generates a unique, statically incremented ID for each new component type at runtime.
uint32_t World::GetNextComponentTypeId() {
    static uint32_t nextId = 0;
    return nextId++;
}

// Creates a new entity. Pulls from the free list if available to keep arrays dense, 
// otherwise expands the generation vector.
Entity World::CreateEntity() {
    uint32_t index;
    if (!m_FreeIndices.empty()) {
        index = m_FreeIndices.back();
        m_FreeIndices.pop_back();
    } else {
        index = static_cast<uint32_t>(m_Generations.size());
        m_Generations.push_back(0);
    }
    
    m_Generations[index]++; 
    if (m_Generations[index] == 0) {
        m_Generations[index]++;
    }
    
    return Entity{index, m_Generations[index]};
}

// Pushes an entity index to a deferred destruction queue. 
// Actual deletion occurs during FlushDeferred to prevent mutating arrays while iterating them.
void World::DestroyEntity(Entity e) {
    if (!IsValid(e)) return;
    m_DeferredDestructions.push_back(e);
}

// Resolves all deferred destructions. Reclaims entity handles by advancing their 
// generation and wiping their associated components out of all contiguous pools.
void World::FlushDeferred() {
    for (auto e : m_DeferredDestructions) {
        if (IsValid(e)) {
            uint32_t index = e.index;
            m_Generations[index]++; // Invalidate handle
            if (m_Generations[index] == 0) {
                m_Generations[index]++;
            }
            m_FreeIndices.push_back(index);
            for (auto& pool : m_Pools) {
                if (pool) {
                    pool->EntityDestroyed(index);
                }
            }
        }
    }
    m_DeferredDestructions.clear();
}

// Validates a handle by comparing its generation tag with the registry's current version.
bool World::IsValid(Entity e) const {
    if (e.index >= m_Generations.size()) return false;
    return m_Generations[e.index] == e.generation;
}

} // namespace Engine
