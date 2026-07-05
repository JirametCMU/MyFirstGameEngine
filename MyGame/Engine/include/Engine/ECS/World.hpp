#pragma once

#include "Engine/Entity.hpp"
#include <vector>
#include <memory>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace Engine {

/**
 * Base interface for component pools to allow type-erased destruction
 * of entity components without knowing the component type.
 */
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void EntityDestroyed(uint32_t entityIndex) = 0;
};

/**
 * A sparse set implementation mapping an entity index to component data.
 * Guarantees contiguous memory layout for fast iteration and cache coherence.
 */
template<typename T>
class ComponentPool : public IComponentPool {
public:
    std::vector<T> data;
    std::vector<Entity> entities;
    std::vector<uint32_t> entityToIndex;

    bool Has(uint32_t entityIndex) const {
        return entityIndex < entityToIndex.size() && entityToIndex[entityIndex] != static_cast<uint32_t>(-1);
    }

    T& Add(Entity e, T component) {
        if (e.index >= entityToIndex.size()) {
            entityToIndex.resize(e.index + 1, static_cast<uint32_t>(-1));
        }
        if (entityToIndex[e.index] != static_cast<uint32_t>(-1)) {
            data[entityToIndex[e.index]] = std::move(component);
            entities[entityToIndex[e.index]] = e;
            return data[entityToIndex[e.index]];
        }
        uint32_t newIndex = static_cast<uint32_t>(data.size());
        entityToIndex[e.index] = newIndex;
        data.push_back(std::move(component));
        entities.push_back(e);
        return data.back();
    }

    T* Get(uint32_t entityIndex) {
        if (entityIndex < entityToIndex.size() && entityToIndex[entityIndex] != static_cast<uint32_t>(-1)) {
            return &data[entityToIndex[entityIndex]];
        }
        return nullptr;
    }

    void Remove(uint32_t entityIndex) {
        if (entityIndex < entityToIndex.size() && entityToIndex[entityIndex] != static_cast<uint32_t>(-1)) {
            uint32_t denseIndex = entityToIndex[entityIndex];
            uint32_t lastDenseIndex = static_cast<uint32_t>(data.size() - 1);
            
            if (denseIndex != lastDenseIndex) {
                data[denseIndex] = std::move(data[lastDenseIndex]);
                entities[denseIndex] = entities[lastDenseIndex];
                entityToIndex[entities[denseIndex].index] = denseIndex;
            }
            
            data.pop_back();
            entities.pop_back();
            entityToIndex[entityIndex] = static_cast<uint32_t>(-1);
        }
    }

    void EntityDestroyed(uint32_t entityIndex) override {
        Remove(entityIndex);
    }
};

/**
 * World — The core of the Entity-Component-System registry.
 * 
 * Manages the lifecycle of entities, assigns unique generational handles,
 * and holds the component pools. Designed to avoid heap allocations
 * in the hot loop by utilizing sparse sets and contiguous arrays.
 */
class World {
public:
    World() = default;
    ~World() = default;

    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) noexcept = default;
    World& operator=(World&&) noexcept = default;

    /**
     * Creates a new entity. Reuses an old slot if available, bumping its generation.
     * @return A valid Entity handle.
     */
    Entity CreateEntity();

    /**
     * Marks an entity for deferred destruction at the end of the frame.
     * The entity remains fully valid until FlushDeferred() is called.
     */
    void DestroyEntity(Entity e);

    /**
     * Executes pending entity destructions. Invalidates handles and frees components.
     * Must be called at a safe synchronization point (e.g., end of frame).
     */
    void FlushDeferred();

    /**
     * Checks whether the given entity handle is currently alive in the world.
     */
    bool IsValid(Entity e) const;

    /**
     * Attaches a component to the entity. Replaces the existing component if present.
     */
    template<typename T>
    T& AddComponent(Entity e, T component) {
        if (!IsValid(e)) {
            return GetPool<T>().Add(e, std::move(component)); 
        }
        return GetPool<T>().Add(e, std::move(component));
    }

    /**
     * Retrieves a mutable pointer to the component if it exists. Returns nullptr otherwise.
     */
    template<typename T>
    T* GetComponent(Entity e) {
        if (!IsValid(e)) return nullptr;
        return GetPool<T>().Get(e.index);
    }

    /**
     * Checks if the entity has a specific component.
     */
    template<typename T>
    bool HasComponent(Entity e) const {
        if (!IsValid(e)) return false;
        auto* world = const_cast<World*>(this);
        return world->GetPool<T>().Has(e.index);
    }

    /**
     * Removes the component from the entity. Does nothing if the component does not exist.
     */
    template<typename T>
    void RemoveComponent(Entity e) {
        if (!IsValid(e)) return;
        GetPool<T>().Remove(e.index);
    }

    /**
     * Iterates over all valid entities that possess ALL of the specified component types.
     * The first template parameter T1 drives the iteration. For optimal performance, T1
     * should ideally be the rarest component in the set.
     * 
     * Callable can take (Entity, T1&, Ts&...) OR just (T1&, Ts&...).
     */
    template<typename T1, typename... Ts, typename Callable>
    void Each(Callable&& callable) {
        auto& pool1 = GetPool<T1>();
        std::tuple<ComponentPool<Ts>*...> pools = { &GetPool<Ts>()... };

        for (size_t i = 0; i < pool1.entities.size(); ++i) {
            Entity e = pool1.entities[i];
            if (!IsValid(e)) continue;

            bool has_all = (... && std::get<ComponentPool<Ts>*>(pools)->Has(e.index));
            if (has_all) {
                if constexpr (std::is_invocable_v<Callable, Entity, T1&, Ts&...>) {
                    callable(e, pool1.data[i], *std::get<ComponentPool<Ts>*>(pools)->Get(e.index)...);
                } else {
                    callable(pool1.data[i], *std::get<ComponentPool<Ts>*>(pools)->Get(e.index)...);
                }
            }
        }
    }

    template<typename... Ts, typename Callable>
    void View(Callable&& callable) {
        Each<Ts...>(std::forward<Callable>(callable));
    }

private:
    static uint32_t GetNextComponentTypeId();

    template<typename T>
    static uint32_t GetComponentTypeId() {
        static uint32_t id = GetNextComponentTypeId();
        return id;
    }

    template<typename T>
    ComponentPool<T>& GetPool() {
        uint32_t typeId = GetComponentTypeId<T>();
        if (typeId >= m_Pools.size()) {
            m_Pools.resize(typeId + 1);
        }
        if (!m_Pools[typeId]) {
            m_Pools[typeId] = std::make_unique<ComponentPool<T>>();
        }
        return static_cast<ComponentPool<T>&>(*m_Pools[typeId]);
    }

    std::vector<uint32_t> m_Generations;
    std::vector<uint32_t> m_FreeIndices;
    std::vector<Entity> m_DeferredDestructions;
    std::vector<std::unique_ptr<IComponentPool>> m_Pools;
};

} // namespace Engine
