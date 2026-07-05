---
name: game-development
description: ECS-first C++20 game engine development — World registry, POD components, stateless systems, hybrid scene flow, performance rules, and agent behavior for MyGame.
---

# Game Development & Engine Engineering Skill

This guide defines how to build and modify **MyGame**: a **hybrid ECS engine** where simulation is data-oriented (entities, components, systems) and shell/flow remains OOP (Application, SceneManager, services).

**Authoritative spec:** [requirements.md §9 ECS Architecture Migration](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/requirements.md#9-ecs-architecture-migration)

When implementing gameplay features, **default to ECS**. Do not add scene-owned `sf::RectangleShape`, `sf::Text`, or per-object transform members for simulated objects.

---

## 1. Repeat Behavior of AI (System Constraints)

When working on this project, the AI must strictly adhere to the following behaviors:

* **ECS-first simulation**: New gameplay objects are entities with POD components and systems — not monolithic scene classes or `GameObject` hierarchies.
* **Hybrid shell**: Keep `Application`, `SceneManager`, `Scene`, and service subsystems (`InputManager`, `ResourceManager`) as OOP. Scenes orchestrate a `World`; they do not implement movement, rendering, or action logic inline.
* **Modern C++ Standard**: Use C++20 (concepts where helpful, `std::string_view`, structured bindings). Avoid legacy C-style idioms where safer C++ alternatives exist.
* **Preserve Documentation**: Retain existing code comments, docstrings, and license headers unless explicitly requested to modify them.
* **Cross-Platform Compatibility**: No direct platform API calls in common code. Platform code lives under `Engine/src/Platform/`.
* **Warning Hygiene**: Code must compile clean under `/W4 /WX` (MSVC) or `-Wall -Wextra -Werror` (GCC/Clang).
* **Clickable File Links**: Link files with absolute markdown URLs: `[filename](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/...)`.
* **Project-Scoped Documents**: Plans, specs, and reviews go inside the project folder only.
* **No third-party ECS libraries**: Build on in-house `Engine::World` (see requirements REQ-ECS-001–002). Do not add EnTT, flecs, etc., unless the user explicitly requests it.
* **Behavior parity**: ECS refactors must preserve fixed timestep, render interpolation (`Engine::Lerp`), input actions, and settings from JSON.

---

## 2. Architecture — Hybrid ECS

### 2.1 Layer model

| Layer | Pattern | Responsibility | Examples |
|-------|---------|----------------|----------|
| **Shell** | OOP | App lifecycle, window, loop | `Engine::Application`, `Game::MyGameApp` |
| **Flow** | OOP | Screen/state transitions | `Engine::Scene`, `SceneManager`, `MainMenuScene` |
| **Services** | OOP | Shared non-entity resources | `InputManager`, `FontManager`, `DisplaySyncController` |
| **Simulation** | **ECS** | Game objects and logic | `Engine::World`, components, systems |
| **Bridge** | Engine utility | SFML drawables outside POD storage | `DrawablePool` |

```text
Application
└── SceneManager
    └── GameplayScene (thin orchestrator)
        ├── Engine::World
        │   ├── Entities (handles)
        │   ├── Component pools (SoA)
        │   └── Registered systems
        └── SystemContext { input, fonts, sceneManager, designSize }
```

### 2.2 ECS definitions (mandatory)

| Concept | Rule | Example |
|---------|------|---------|
| **Entity** | Opaque handle with generation; not a class | `Engine::Entity { index, generation }` |
| **Component** | POD struct; **state only**; no virtuals, no logic | `Transform2D`, `MoveSpeed`, `PlayerTag` |
| **System** | Stateless; iterates matching components each frame | `MovementSystem`, `RenderSystem` |
| **World** | Owns entities, component storage, system order | `Engine::World` |
| **Tag** | Empty marker component for queries | `PlayerTag`, `EnemyTag` |

**Do not**

- Create `GameObject`, `Actor`, or base classes with virtual `Update()`/`Render()` for gameplay entities.
- Put input handling, physics, and drawing in the same scene method body.
- Store `sf::RectangleShape`, `sf::Text`, or other SFML drawables inside component structs.
- Use `std::map<Entity, T>` for component storage.

**Do**

- Spawn entities in scene `OnEnter`; destroy or reset world in `OnExit`.
- Pass dependencies via `SystemContext` each frame — no global singletons in systems.
- Keep `Engine::Transform2D` as the canonical spatial component; use `PreviousTransform2D` for fixed-step lerp.

---

## 3. Clean Code & Naming

* **Separation of concerns**: `.hpp` / `.cpp` split; header-only only for hot-path templates and tiny inlines (`Transform2D::ApplyTo`, component accessors).
* **Naming conventions**:

  | Item | Convention | Example |
  |------|------------|---------|
  | Engine types | `PascalCase` | `World`, `Entity`, `DrawablePool` |
  | Components | `PascalCase` noun | `MoveSpeed`, `RectangleShapeVisual` |
  | Tag components | `*Tag` suffix | `PlayerTag` |
  | Systems | `PascalCase` + `System` | `MovementSystem` |
  | Methods | `PascalCase` | `CreateEntity`, `OnFixedUpdate` |
  | Parameters / locals | `camelCase` | `fixedDeltaTime`, `alpha` |
  | Private members (classes) | `m_` prefix | `m_World`, `m_Systems` |
  | Component fields (POD) | `camelCase`, no `m_` | `unitsPerSecond`, `position` |

* **RAII**: `std::unique_ptr` for ownership; raw pointers only as non-owning observers within a frame.
* **Single responsibility**: One system = one concern. `MovementSystem` does not draw; `RenderSystem` does not read keyboard.

---

## 4. Component Design

### 4.1 Engine components (`Engine::`)

Reusable across games; live under [Engine/include/Engine/](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/Engine/include/Engine/).

| Component | Purpose |
|-----------|---------|
| `Transform2D` | Current position, rotation (degrees), scale |
| `PreviousTransform2D` | Snapshot before fixed step — for render lerp |
| `Velocity2D` | Optional linear velocity (if not derived from input each tick) |

Existing helpers stay: `ApplyTo(sf::Transformable&)`, `Engine::Lerp(a, b, t)`.

### 4.2 Game components (`Game::`)

Gameplay-specific; live under [Game/include/Game/ECS/](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/Game/include/Game/ECS/).

| Component | Purpose |
|-----------|---------|
| `PlayerTag` | Marks player entity |
| `MoveSpeed` | `float unitsPerSecond` from settings |
| `RectangleShapeVisual` | Size + color params (not the SFML shape) |
| `TextVisual` | String, char size, color (setup-time string only) |
| `PulseAnimation` | UI alpha pulse state |

### 4.3 Component rules

1. Trivially copyable where possible; target ≤ 64 bytes unless justified.
2. No methods beyond trivial defaults; no `virtual`, no inheritance trees.
3. No `std::function`, `shared_ptr`, or SFML drawables in hot components.
4. Strings in components are **spawn-time only** (e.g. menu title text), not mutated every frame.

---

## 5. Systems & Scheduling

### 5.1 Interface

Systems implement `Engine::ISystem`:

```cpp
struct ISystem {
    virtual void OnUpdate(World& world, const SystemContext& ctx, float deltaTime) {}
    virtual void OnFixedUpdate(World& world, const SystemContext& ctx, float fixedDeltaTime) {}
    virtual void OnRender(World& world, const SystemContext& ctx,
                          sf::RenderWindow& window, float alpha) {}
    virtual ~ISystem() = default;
};
```

Systems hold **no per-entity state**. All mutable state lives in components.

### 5.2 Execution order (fixed)

```text
Scene::OnUpdate(dt)
  └── World::Update(dt, ctx)
        1. InputSystem           — IsActionJustPressed (Jump, Shoot, Confirm)
        2. UIAnimationSystem     — pulse, non-physics motion

Scene::OnFixedUpdate(fixedDt)
  └── World::FixedUpdate(fixedDt, ctx)
        1. MovementSystem        — integrate position from held actions

Scene::OnRender(window, alpha)
  └── World::Render(window, alpha, ctx)
        1. RenderSystem          — Lerp(previous, current, alpha) + draw
```

Register order explicitly; document changes in tests.

### 5.3 SystemContext (dependency injection)

```cpp
struct SystemContext {
    const InputManager& input;
    FontManager& fonts;
    SceneManager* sceneManager{nullptr}; // Game-only scene transitions
    sf::Vector2f designSize{0.f, 0.f};
};
```

Engine systems must not include Game headers. Game systems may use `SceneManager` for menu → gameplay transitions.

---

## 6. World & Storage

### 6.1 Entity handles

Use index + generation so destroyed handles fail safely:

```cpp
struct Entity {
    uint32_t index{0};
    uint32_t generation{0};
    [[nodiscard]] bool IsValid() const noexcept { return generation != 0; }
};
```

On destroy: bump generation, free slot. Never reuse an index without bumping generation.

### 6.2 Structure-of-Arrays (SoA)

Each component type gets its own contiguous pool:

```cpp
template<typename T>
struct ComponentPool {
    std::vector<T> data;
    std::vector<Entity> entities; // parallel index
};
```

Iterate with index loops — not pointer chains:

```cpp
for (size_t i = 0; i < count; ++i) {
    auto& transform = transforms.data[i];
    auto& speed = speeds.data[i];
    // ...
}
```

### 6.3 Deferred destruction

Destroy entities at end of frame (or explicit `FlushDeferred()`) so systems never iterate stale slots mid-pass.

---

## 7. SFML Bridge — DrawablePool

SFML objects are **not** components. Bridge via a pool keyed by `Entity`:

```cpp
// Spawn (once, in scene OnEnter or factory)
auto e = world.CreateEntity();
world.AddComponent<Transform2D>(e, ...);
world.AddComponent<RectangleShapeVisual>(e, { size, size, sf::Color::Cyan });
drawablePool.CreateRectangle(e, size, size); // sets origin, fill

// Render (hot path)
drawablePool.WithRectangle(e, [&](sf::RectangleShape& shape) {
    renderTransform.ApplyTo(shape);
    window.draw(shape);
});
```

Create/destroy pool entries with entity lifetime. No `new`/`delete` per frame in render loop.

---

## 8. Scene Integration Pattern

Scenes are **thin orchestrators**. Reference implementation target:

```cpp
class GameplayScene : public Engine::Scene {
    Engine::World m_World;
    Engine::DrawablePool m_Drawables;
    std::vector<std::unique_ptr<Engine::ISystem>> m_Systems;

    void OnEnter() override {
        // Register systems in order
        // Spawn player entity with components
    }
    void OnFixedUpdate(float dt) override {
        m_World.FixedUpdate(dt, BuildContext());
    }
    void OnUpdate(float dt) override {
        m_World.Update(dt, BuildContext());
    }
    void OnRender(sf::RenderWindow& w, float alpha) override {
        m_World.Render(w, alpha, BuildContext());
    }
    void OnExit() override {
        m_World.Clear(); // or destroy all entities
    }
};
```

**Forbidden in scenes after ECS migration**

```cpp
// BAD — OOP gameplay object
sf::RectangleShape m_Player;
Engine::Transform2D m_CurrentTransform;
void OnFixedUpdate(float dt) {
    m_CurrentTransform.position += direction * speed * dt; // logic in scene
}
```

---

## 9. Fixed Timestep & Interpolation

Preserve Gaffer-on-Games model (already in engine loop):

| Phase | System | Action |
|-------|--------|--------|
| Fixed tick | `MovementSystem` | `previous = current`; update `Transform2D` |
| Render | `RenderSystem` | `render = Lerp(previous, current, alpha)`; `ApplyTo` drawable |

Movement uses `OnFixedUpdate`; one-shot input uses `OnUpdate`. Never skip `PreviousTransform2D` copy before integration.

---

## 10. Performance Rules

Hot paths: `World::Update`, `World::FixedUpdate`, `World::Render`, and everything they call.

| Rule | Detail |
|------|--------|
| **No allocations** | No `new`, `vector` growth, or string concat per entity per frame in steady state |
| **No RTTI** | No `dynamic_cast` / `typeid` in entity loops |
| **No per-entity virtual calls** | Virtual once per system per frame is OK |
| **Contiguous storage** | `std::vector` SoA; never `std::list` / `std::map` for components |
| **Reserve at spawn** | Pre-size pools in `OnEnter` when entity count is known |
| **Object pooling** | Bullets/particles: recycle entities, don't create/destroy every frame |
| **Profile Release** | `/O2` or `-O3`; use VS Profiler or Tracy before micro-optimizing |

### Anti-patterns (reject in review)

| Anti-pattern | Why |
|--------------|-----|
| Virtual `Component` base | Reintroduces OOP into data layer |
| God scene class | Unmaintainable; breaks cache locality |
| SFML drawable in component | Breaks POD layout; bloats SoA arrays |
| Global system state | Hidden coupling; untestable |
| `shared_ptr` in components | Atomic refcount in hot loop |

---

## 11. Engine vs Game Boundary

| Item | Layer | Location |
|------|-------|----------|
| `Entity`, `World`, `ISystem`, `DrawablePool`, `SystemContext` | Engine | `Engine/include/Engine/ECS/` |
| `Transform2D`, `PreviousTransform2D`, `Lerp`, `Math` | Engine | `Engine/include/Engine/` |
| `PlayerTag`, `MoveSpeed`, `MovementSystem`, `RenderSystem` | Game | `Game/include/Game/ECS/` |
| Settings, scene IDs, menu/gameplay scenes | Game | `Game/include/Game/` |

Game links Engine; Engine never includes Game.

---

## 12. Data-Driven Configuration

Gameplay numbers stay in JSON ([Assets/Config/settings.json](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/Game/Assets/Config/settings.json)):

- `playerSpeed`, `playerSize`, `fixedTimestepHz` → applied when **spawning** entities (component init), not hardcoded in systems.
- Input bindings → `InputManager` via settings; systems read `GameAction`, never raw keycodes.

---

## 13. Development Workflow

1. **Read spec**: Check [requirements.md §9](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/requirements.md#9-ecs-architecture-migration) for REQ-ECS-* acceptance criteria before coding.
2. **Implement in PR order**: World core → systems → scene migration → remove OOP leftovers (see §9.3.8 in requirements).
3. **Test**: Add Catch2 tests for registry, movement parity, stale handles. Run `ctest` Debug + Release.
4. **Profile first**: Never optimize speculatively; measure Release builds.
5. **CMake**: Register new headers/sources in `Engine/CMakeLists.txt` and `Game/CMakeLists.txt`.

---

## 14. Migration Checklist (when touching gameplay)

Before marking work complete, verify:

- [ ] Simulated objects are entities, not scene member drawables
- [ ] Logic lives in systems, not `GameplayScene` / `MainMenuScene` methods
- [ ] `Transform2D` + `PreviousTransform2D` drive render lerp
- [ ] SFML drawables live in `DrawablePool`, not components
- [ ] `SystemContext` passes input/fonts/scene manager — no globals
- [ ] No heap allocs in steady-state update/render loops
- [ ] Existing tests pass; new ECS tests added for changed behavior
- [ ] Movement/input/menu behavior matches pre-migration baseline

---

## 15. References

| Topic | Source |
|-------|--------|
| Full ECS requirements | [requirements.md §9](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/requirements.md#9-ecs-architecture-migration) |
| Transform component | [Transform2D.hpp](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/Engine/include/Engine/Transform2D.hpp) |
| Fixed timestep | [Fix Your Timestep — Gaffer on Games](https://gafferongames.com/post/fix_your_timestep/) |
| Data-oriented design | [Data-Oriented Design](https://www.dataorienteddesign.com/dodbook/) |
| Current baseline (pre-migration) | [GameplayScene.cpp](file:///C:/Works/LearningCode/Cpp/MyFirstGameEngine/MyGame/Game/src/GameplayScene.cpp) |
