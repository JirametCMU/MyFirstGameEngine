#include "Engine/ECS/World.hpp"

namespace Engine {

uint32_t World::GetNextComponentTypeId() {
    static uint32_t nextId = 0;
    return nextId++;
}

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

void World::DestroyEntity(Entity e) {
    if (!IsValid(e)) return;
    m_DeferredDestructions.push_back(e);
}

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

bool World::IsValid(Entity e) const {
    if (e.index >= m_Generations.size()) return false;
    return m_Generations[e.index] == e.generation;
}

} // namespace Engine
