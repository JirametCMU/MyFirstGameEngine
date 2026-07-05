# MyGame — Requirements, Guidelines & Quality Tracker

This document defines **current** requirements, implementation guidelines, and tracked quality findings for the MyGame engine and game layer.

**Legend:** ✅ Implemented · ⚠️ Partial · ❌ Not started

**Initial scan:** 2026-07-05 · **Last verified:** 2026-07-05 · Scope: full codebase (~44 source files — Engine, Game, CMake, tests)

**Active initiative:** ECS architecture migration (§9) — refactor gameplay simulation from scene-owned OOP objects to a data-oriented Entity-Component-System while preserving the existing scene flow, fixed timestep, and SFML rendering pipeline.

---

## 1. Background & Problem

### Current state

✅ **Done.** [GameplayScene.cpp](Game/src/GameplayScene.cpp) uses `Engine::Transform2D` for player state with fixed-timestep interpolation via `Engine::Lerp`. Scale is clamped to a minimum of `1e-6f` in `ApplyTo` (REQ-TRANSFORM-002).

### Goal

Maintain a small, **header-first** `Engine::Transform2D` POD struct with position, rotation, and scale. Game code owns transform state; rendering applies it to `sf::Transformable` in one call.

---

## 2. Requirements

### REQ-TRANSFORM-001 — `Engine::Transform2D` module (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine layer only (Game consumes via `#include "Engine/Transform2D.hpp"`) |
| **Status** | ✅ Implemented |

**Acceptance criteria**

1. New public header: [Engine/include/Engine/Transform2D.hpp](Engine/include/Engine/Transform2D.hpp).
2. Type lives in namespace `Engine` as a **POD struct** (state only — no gameplay logic).
3. Default construction: position `{0, 0}`, rotation `0`, scale `{1, 1}`.
4. Registered in [Engine/CMakeLists.txt](Engine/CMakeLists.txt) as a `PUBLIC` header.

---

### REQ-TRANSFORM-002 — Position, rotation, scale (Critical)

| Field | Value |
|-------|-------|
| **Status** | ✅ Implemented — scale clamped in `ApplyTo`; optional rotation setter not added |

| Field | Type | Unit / convention |
|-------|------|-------------------|
| **Position** | `sf::Vector2f` | World/logical space (same as current gameplay coordinates) |
| **Rotation** | `float` | **Degrees**, matching SFML `setRotation` |
| **Scale** | `sf::Vector2f` | Per-axis scale; `{1, 1}` = no scaling |

**Acceptance criteria**

1. Public members (or trivial accessors): `position`, `rotation`, `scale`.
2. `rotation` is normalized to `[0, 360)` when written via a setter or `SetRotation` helper (optional convenience — document behavior).
3. `scale` components must not be zero; clamp or reject with a documented minimum (e.g. `1e-6f`) to avoid broken SFML transforms.

---

### REQ-TRANSFORM-003 — Apply to SFML drawables (High)

| Field | Value |
|-------|-------|
| **Status** | ✅ Implemented |

**Function:** `ApplyTo(sf::Transformable& drawable) const`

**Behavior**

1. Calls `drawable.setPosition(position)`, `setRotation(rotation)`, `setScale(scale)` in that order.
2. Does not modify origin — callers set origin on the shape/sprite once at setup.

**Acceptance criteria**

1. After `ApplyTo`, the drawable matches the transform state.
2. `noexcept`, inline in header, no heap allocation.

---

### REQ-TRANSFORM-004 — Interpolation for fixed timestep (High)

| Field | Value |
|-------|-------|
| **Status** | ✅ Implemented |

**Function:** `Lerp(const Transform2D& a, const Transform2D& b, float t) -> Transform2D`

**Behavior**

1. **Position:** component-wise `Engine::Math::Lerp`.
2. **Rotation:** shortest-path angle lerp in degrees (handle wrap across 360°).
3. **Scale:** component-wise `Engine::Math::Lerp`.
4. Does **not** clamp `t` (same contract as `Engine::Math::Lerp`).

**Acceptance criteria**

1. `Lerp(t=0)` equals `a`; `Lerp(t=1)` equals `b` within float tolerance.
2. Rotating from 350° to 10° lerps through ~0°, not through 180°.

---

### REQ-TRANSFORM-005 — Adopt in gameplay (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **File** | [GameplayScene.cpp](Game/src/GameplayScene.cpp) |
| **Status** | ✅ Implemented |

**Acceptance criteria**

1. Replace `m_PreviousPos` / `m_CurrentPos` with `m_PreviousTransform` / `m_CurrentTransform`.
2. Movement updates `m_CurrentTransform.position` only (rotation/scale unchanged unless gameplay sets them).
3. `OnFixedUpdate` applies `m_CurrentTransform.ApplyTo(m_Player)`.
4. `OnRender` uses `Transform2D::Lerp(m_PreviousTransform, m_CurrentTransform, alpha)` then `ApplyTo`.
5. Existing movement speed and diagonal normalization behavior unchanged.

---

### REQ-TRANSFORM-006 — Unit tests (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **Location** | [Engine/tests/test_Transform2D.cpp](Engine/tests/test_Transform2D.cpp) |
| **Status** | ✅ Implemented |

**Minimum test cases**

| Test | Expected |
|------|----------|
| Default construct | position `{0,0}`, rotation `0`, scale `{1,1}` |
| `ApplyTo` on `sf::RectangleShape` | position/rotation/scale match transform |
| `Lerp` position | midpoint at `t=0.5` |
| `Lerp` rotation 350° → 10° at `t=0.5` | ≈ 0° (shortest path) |
| `Lerp` scale | component-wise midpoint |

Tests must not require SFML window or GPU.

---

## 3. Senior Implementation Guidelines

### 3.1 Design — POD struct, not God-class

Use a **plain struct** for transform state. Keep math helpers as free functions in the same header or reuse `Engine::Math`:

```cpp
namespace Engine {

struct Transform2D {
    sf::Vector2f position{0.0f, 0.0f};
    float rotation{0.0f};           // degrees
    sf::Vector2f scale{1.0f, 1.0f};

    void ApplyTo(sf::Transformable& drawable) const noexcept;
};

[[nodiscard]] Transform2D Lerp(
    const Transform2D& a, const Transform2D& b, float t) noexcept;

} // namespace Engine
```

**Why**

- Matches ECS-style components (`PositionComponent`-like state without a monolithic `GameObject`).
- One place to apply position, rotation, and scale to any `sf::Transformable`.
- Easy to test and to extend later (parent/child hierarchy is out of scope for now).

**Do not**

- Put input, speed, or collision logic inside `Transform2D`.
- Create `TransformManager` or attach transforms to `Application`.
- Add 3D matrices, quaternions, or parent-child hierarchies in this phase.

---

### 3.2 Header-only + hot-path rules

| Rule | Detail |
|------|--------|
| **Inline in header** | `ApplyTo`, `Lerp` — called from `OnFixedUpdate` / `OnRender` |
| **Reuse `Engine::Math`** | Position/scale lerp and angle helpers — do not duplicate |
| **No allocations** | No `std::vector`, strings, or heap in transform helpers |
| **`noexcept`** | Mark functions that never throw |
| **Degrees for rotation** | Stay consistent with SFML; document in the header |

---

### 3.3 Rotation lerp (shortest path)

Use a delta in `[-180, 180]` before interpolating:

```cpp
float delta = b.rotation - a.rotation;
while (delta > 180.0f)  delta -= 360.0f;
while (delta < -180.0f) delta += 360.0f;
return a.rotation + delta * t;
```

Normalize the result to `[0, 360)` if you expose rotation through a setter.

---

### 3.4 Gameplay integration pattern

**Before (position only, scattered SFML calls)**

```cpp
sf::Vector2f m_PreviousPos;
sf::Vector2f m_CurrentPos;
m_CurrentPos += direction * speed * dt;
m_Player.setPosition(m_CurrentPos);
const sf::Vector2f renderPos = Engine::Math::Lerp(m_PreviousPos, m_CurrentPos, alpha);
m_Player.setPosition(renderPos);
```

**After (transform owns spatial state)**

```cpp
Engine::Transform2D m_PreviousTransform;
Engine::Transform2D m_CurrentTransform;

m_PreviousTransform = m_CurrentTransform;
m_CurrentTransform.position += direction * speed * dt;
m_CurrentTransform.ApplyTo(m_Player);

const Engine::Transform2D renderTransform =
    Engine::Lerp(m_PreviousTransform, m_CurrentTransform, alpha);
renderTransform.ApplyTo(m_Player);
```

---

### 3.5 Engine vs Game boundary

| Layer | Responsibility |
|-------|----------------|
| **Engine::Transform2D** | Spatial state + apply/lerp helpers — no input, no gameplay constants |
| **GameplayScene** | Update `position` from input; set `rotation`/`scale` when gameplay needs them |
| **SFML drawable** | Origin and visual setup at init; transform applied each frame |

---

### 3.6 Naming conventions (project alignment)

| Item | Convention |
|------|------------|
| Type | `Transform2D` |
| Namespace | `Engine` |
| Members | `position`, `rotation`, `scale` (or `m_` prefix only inside classes, not POD fields) |
| Functions | `PascalCase` — `ApplyTo`, `Lerp` |
| Parameters | `camelCase` — `fixedDeltaTime`, `alpha` |

---

## 4. Transform2D — Files (complete)

| Action | File | Status |
|--------|------|--------|
| Created | [Engine/include/Engine/Transform2D.hpp](Engine/include/Engine/Transform2D.hpp) | ✅ |
| Created | [Engine/tests/test_Transform2D.cpp](Engine/tests/test_Transform2D.cpp) | ✅ |
| Modified | [Engine/CMakeLists.txt](Engine/CMakeLists.txt) — `Transform2D.hpp` in `PUBLIC` headers | ✅ |
| Modified | [Engine/tests/CMakeLists.txt](Engine/tests/CMakeLists.txt) — `test_Transform2D.cpp` registered | ✅ |
| Modified | [Game/src/GameplayScene.cpp](Game/src/GameplayScene.cpp) — uses `Transform2D` for player state | ✅ |
| Modified | [Game/include/Game/GameplayScene.hpp](Game/include/Game/GameplayScene.hpp) — transform members | ✅ |

---

## 5. Verification checklist

1. **Position:** Player moves at `playerSpeed` — cardinal and diagonal unchanged from current behavior.
2. **Rotation:** Setting `rotation = 45` visibly rotates the player square around its origin.
3. **Scale:** Setting `scale = {2, 1}` doubles width only.
4. **Render lerp:** Smooth motion between fixed steps; no jitter when only position changes.
5. **Rotation lerp:** No 180° flip when lerping across 0°/360° boundary.
6. **Tests:** All `test_Transform2D.cpp` cases pass in Debug and Release.
7. **Warnings:** Builds clean under `/W4 /WX`.

---

## 6. Completed — Engine::Math & Transform2D (reference only)

The following are **done**; do not re-scope this document around them:

| ID | Summary | Status |
|----|---------|--------|
| REQ-MATH-001 | `Engine::Math` header-only module | ✅ |
| REQ-MATH-002 | Phase 1 functions (`Lerp`, `NormalizeOrZero`, etc.) | ✅ |
| REQ-MATH-003 | `NormalizeDigitalDirection` | ✅ |
| REQ-MATH-005 | Gameplay movement + render lerp via Math | ✅ |
| REQ-MATH-006 | `test_Math.cpp` | ✅ |
| REQ-TRANSFORM-001 | `Engine::Transform2D` module | ✅ |
| REQ-TRANSFORM-002 | Position, rotation, scale (scale clamp in `ApplyTo`) | ✅ |
| REQ-TRANSFORM-003 | `ApplyTo` for SFML drawables | ✅ |
| REQ-TRANSFORM-004 | `Lerp` with shortest-path rotation | ✅ |
| REQ-TRANSFORM-005 | Gameplay adoption in `GameplayScene` | ✅ |
| REQ-TRANSFORM-006 | `test_Transform2D.cpp` | ✅ |
| REQ-WEAK-001 | `fixedTimestepHz` wired via `SetFixedTimestepHz()` | ✅ |
| REQ-WEAK-002 | Platform-guarded `PathService` (Win32 + POSIX) | ✅ |
| REQ-WEAK-003 | Runtime VSync toggle persisted on shutdown | ✅ |
| REQ-WEAK-004 | Settings load uses temp copy — parse failure safe | ✅ |
| REQ-WEAK-011 | `Transform2D` scale-zero protection | ✅ |
| REQ-SEC-002 | Gameplay settings validation (`playerSpeed`, `playerSize`, `fixedTimestepHz`, `fpsCap`) | ✅ |

Implementation: [Engine/include/Engine/Math.hpp](Engine/include/Engine/Math.hpp), [Engine/include/Engine/Transform2D.hpp](Engine/include/Engine/Transform2D.hpp).

---

## 7. References

| Topic | Source |
|-------|--------|
| Fixed timestep render lerp | [Fix Your Timestep — Gaffer on Games](https://gafferongames.com/post/fix_your_timestep/) |
| ECS component style | [game_development SKILL.md](.agents/skills/game_development/SKILL.md) — ECS-first architecture |
| **ECS migration (full spec)** | [§9 ECS Architecture Migration](requirements.md#9-ecs-architecture-migration) |
| SFML transform order | position → rotation → scale on `sf::Transformable` |

---

## 8. Code Quality Scan — Vulnerabilities, Weaknesses & Technical Debt

**Scan summary:** This is a **local, offline SFML game** with no networking, no shell execution, and no unsafe C APIs. **No critical CVE-style vulnerabilities were found.** P0/P1 config and platform issues from the initial scan are **resolved**. Remaining gaps are **maintainability, polish, and future-scope hardening**.

### 8.1 Scan verdict

| Category | Verdict |
|----------|---------|
| **Critical vulnerabilities** | None found |
| **Weaknesses** | Some remain — silent resource fallbacks, logging inconsistency, stub gameplay actions |
| **Technical debt** | Yes — no CI, Game-layer tests, incomplete POSIX exe-dir path, stub features |

### 8.2 What is in good shape

- Modern C++20 with `unique_ptr`; no raw `new`/`delete` in game code
- Fixed timestep with accumulator and substep cap (spiral-of-death protection); **`fixedTimestepHz` from settings applied via `SetFixedTimestepHz()`**
- **Settings validation** for `fpsCap`, `playerSpeed`, `playerSize`, `fixedTimestepHz`; parse failures no longer partially corrupt loaded settings
- **Cross-platform PathService** — CMake selects `PathService_Win32.cpp` or `PathService_POSIX.cpp`
- **Runtime VSync toggle persisted** — `MyGameApp::OnShutdown()` saves `syncMode` to `settings.json`
- **`Transform2D` scale clamping** in `ApplyTo` prevents zero-scale SFML breakage
- Input focus handling when the window loses focus
- Pinned dependency versions (SFML 2.6.1, spdlog 1.13.0, nlohmann/json 3.11.3, Catch2 3.4.0)
- Engine unit tests for Math, Transform2D, InputManager, PathService, DisplaySync
- Strict warnings on Engine (`/W4 /WX` on MSVC, `-Wall -Wextra -Werror` on GCC/Clang)

---

### 8.3 Vulnerabilities (Security)

| ID | Severity | Finding | Location | Status |
|----|----------|---------|----------|--------|
| REQ-SEC-001 | Low | Settings JSON parsed without size/depth limits — corrupted or malicious local `settings.json` could cause high memory/CPU use during parse (JSON bomb pattern). Low risk: file is local, not downloaded. | `Game/src/SettingsManager.cpp` | ❌ Not started |
| REQ-SEC-003 | Low | Asset paths not sandboxed — `ResourceManager::Load()` loads any path passed in. Safe today (paths are hardcoded); risky if config/mod paths are added without normalization. | `Engine/include/Engine/ResourceManager.hpp` | ❌ Not started |
| REQ-SEC-004 | Low | Font/texture parsing trusts file content — malformed assets rely on SFML/STB parsers. Low risk for dev-only assets. | `ResourceManager.hpp` | ❌ Not started |
| REQ-SEC-005 | Info | Supply chain via FetchContent — deps are pinned but no commit-hash verification or vendoring option. | `MyGame/CMakeLists.txt`, `Engine/CMakeLists.txt`, `Game/CMakeLists.txt` | ❌ Not started |

**Resolved:** REQ-SEC-002 (gameplay settings validation) — see §8.7.

**Not found:** buffer overflows, command injection, auth/crypto issues, network exposure, secrets in repo, or deprecated unsafe C string functions.

---

### 8.4 Weaknesses (Design / Reliability) — open items

Resolved high-impact items (REQ-WEAK-001–004, REQ-WEAK-011) are listed in §8.7.

| ID | Finding | Location | Status |
|----|---------|----------|--------|
| REQ-WEAK-005 | **`ResourceManager::Get()` silently returns fallback** — both Debug and Release log error and return placeholder (no throw). `const_cast` on mutable `Get()` remains; missing assets still easy to miss visually. | `Engine/include/Engine/ResourceManager.hpp` | ⚠️ Partial |
| REQ-WEAK-006 | **`SceneManager` uses raw pointer** (`m_ActiveScene`) to objects owned by `unique_ptr`. Fragile if scene unloading is added while active. | `Engine/include/Engine/SceneManager.hpp`, `SceneManager.cpp` | ❌ Not started |
| REQ-WEAK-007 | **`InputManager` polls every key and mouse button every frame** — O(n) per frame; acceptable now, wasteful at scale. | `Engine/src/InputManager.cpp` | ❌ Not started |
| REQ-WEAK-008 | **Inconsistent logging** — mix of `std::cout`/`std::cerr` and `ENGINE_*` spdlog macros across Engine and Game. | Multiple Engine/Game sources | ❌ Not started |
| REQ-WEAK-009 | **`LoadDirectory()` extension check is case-sensitive on allowed list** — file extension is lowercased, but caller extensions like `".TTF"` would not match. | `ResourceManager.hpp` | ❌ Not started |
| REQ-WEAK-010 | **`LoadDirectory()` counts failed loads** — `loadedCount` increments even when `Load()` falls back to a placeholder. | `ResourceManager.hpp` | ❌ Not started |
| REQ-WEAK-012 | **Bound actions with no effect** — `Pause`, and partially `Jump`/`Shoot`, are wired in settings but only log to console in gameplay. | `GameplayScene.cpp`, `InputManager.hpp` | ❌ Not started |

---

### 8.5 Technical Debt

| ID | Area | Debt | Status |
|----|------|------|--------|
| REQ-DEBT-001 | CI/CD | No GitHub Actions or automated build/test pipeline. | ❌ Not started |
| REQ-DEBT-002 | Test coverage | Engine has Catch2 tests; Game layer (`SettingsManager`, scenes) has none. | ❌ Not started |
| REQ-DEBT-003 | Build system | FetchContent pulls deps on every configure — slow; no offline/vendor option documented. | ❌ Not started |
| REQ-DEBT-004 | Cross-platform | POSIX `PathService` added but uses `current_path()` fallback — not true executable-directory resolution. | ⚠️ Partial |
| REQ-DEBT-005 | Feature stubs | GSync mode logs warning and falls back; pause menu not implemented; gameplay is a moving-square prototype. | ❌ Not started |
| REQ-DEBT-006 | Key rebinding | `StringToKey`/`KeyToString` support only ~11 keys — not extensible for full keyboard rebinding. | ❌ Not started |
| REQ-DEBT-007 | SFML version | Pinned to SFML 2.6.1; SFML 3 migration is future work. | ❌ Not started |
| REQ-DEBT-008 | Hardening | No sanitizers (ASan/UBSan), static analysis (clang-tidy), or hardening flags in CMake. | ❌ Not started |
| REQ-DEBT-009 | Assets pipeline | Only Fonts README in repo; no default `settings.json` checked in (created at runtime). | ❌ Not started |
| REQ-DEBT-010 | Documentation | Engine.hpp comments still describe hardcoded R/V shortcut keys; keys are now configurable via settings. | ❌ Not started |
| REQ-DEBT-011 | Architecture | Gameplay uses scene-owned OOP objects; ECS migration planned in §9. | ❌ Not started |

---

### 8.6 Recommended fix priority

| Priority | IDs | Rationale |
|----------|-----|-----------|
| **P0** | REQ-ECS-001 – REQ-ECS-012 | ECS migration — foundation, gameplay/menu refactor, performance constraints (§9) |
| **P1** | REQ-DEBT-001, REQ-DEBT-002, REQ-WEAK-008 | CI, Game-layer tests, unified logging |
| **P2** | REQ-WEAK-005, REQ-WEAK-006, REQ-WEAK-009, REQ-WEAK-010, REQ-DEBT-004 | Resource manager polish, scene safety, POSIX exe-dir |
| **P3** | REQ-WEAK-007, REQ-WEAK-012, REQ-DEBT-005, REQ-DEBT-006, REQ-DEBT-010 | Performance/input stubs, feature completion, doc drift |
| **P4** | REQ-SEC-001, REQ-SEC-003, REQ-SEC-004, REQ-SEC-005, REQ-DEBT-003, REQ-DEBT-007–009 | Lower risk or future-scope hardening |

### 8.7 Resolved since initial scan (2026-07-05)

| ID | Resolution |
|----|------------|
| REQ-WEAK-001 | `SetFixedTimestepHz()` + `m_TargetFixedTimestep` in engine loop |
| REQ-WEAK-002 | Platform-guarded CMake + `PathService_POSIX.cpp` |
| REQ-WEAK-003 | `MyGameApp::OnShutdown()` saves display sync mode |
| REQ-WEAK-004 | `loadedSettings` temp copy in `SettingsManager::Load()` |
| REQ-WEAK-011 | Scale clamp in `Transform2D::ApplyTo` |
| REQ-SEC-002 | Gameplay + display settings clamped on load (`fpsCap`, `playerSpeed`, `playerSize`, `fixedTimestepHz`) |
| REQ-TRANSFORM-002 | Scale minimum enforced (optional rotation setter deferred) |

---

## 9. ECS Architecture Migration

This section defines requirements and senior guidelines for refactoring **gameplay simulation** from scene-owned OOP objects to a **data-oriented ECS**. Scene flow (`Scene`, `SceneManager`, `Application`) remains OOP — scenes become thin orchestrators that own an `Engine::World`, register systems, and spawn entities.

**Legend (this section):** ✅ Implemented · ⚠️ Partial · ❌ Not started

---

### 9.1 Background & Problem

#### Current state

⚠️ **Partial ECS readiness.** The engine already uses ECS-friendly POD data (`Engine::Transform2D`), fixed-timestep interpolation, and separated engine subsystems. Game objects are still **monolithic scene members**:

| Location | OOP pattern today | ECS gap |
|----------|-------------------|---------|
| [GameplayScene.hpp](Game/include/Game/GameplayScene.hpp) | `sf::RectangleShape m_Player` + transform members + movement logic in scene methods | No entity handle, no component storage, no systems |
| [MainMenuScene.hpp](Game/include/Game/MainMenuScene.hpp) | `sf::Text` members + pulse animation in scene | UI not entity-driven |
| [SceneManager.hpp](Engine/include/Engine/SceneManager.hpp) | Scene lifecycle only | No simulation world |

Logic, state, and rendering for the player live in one class. Adding enemies, bullets, or particles would duplicate patterns and hurt cache locality.

#### Goal

Introduce a **custom, header-friendly ECS core** in the Engine layer and migrate all **simulated game objects** (player, future enemies/projectiles, menu UI entities) into entities with POD components and stateless systems. Preserve existing behavior: movement speed, diagonal normalization, fixed-timestep physics, render interpolation, input actions, settings from JSON, and scene transitions.

#### Non-goals (this migration)

- Replacing `Scene` / `SceneManager` with ECS (scenes stay as state machines).
- Replacing engine services (`InputManager`, `ResourceManager`, `DisplaySyncController`) with ECS.
- Adding a third-party ECS library (EnTT, flecs, etc.) — build a minimal in-house registry tailored to this project.
- 3D transforms, physics engine integration, or multithreaded system scheduling.
- Networking or serialization of worlds.

#### Target architecture

```text
Application (OOP shell — unchanged)
└── SceneManager (OOP — unchanged)
    └── GameplayScene / MainMenuScene (thin orchestrators)
        └── Engine::World
            ├── Entities (handles)
            ├── Component storage (SoA, POD)
            └── Systems (Movement, Render, Input, UIAnimation, …)
                └── consume InputManager, FontManager, RenderWindow via context
```

---

### 9.2 ECS Requirements

Requirements are ordered by dependency. Complete each phase before starting the next unless noted.

---

#### REQ-ECS-001 — `Engine::Entity` handle (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine layer |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Public type `Engine::Entity` — opaque handle (recommended: `{ uint32_t index; uint32_t generation; }`).
2. Default-constructed handle is **invalid**; `IsValid()` or equivalent returns `false`.
3. Handles remain valid after other entities are destroyed; stale handles fail safely (no use-after-free).
4. `operator==` / `operator!=` for handle comparison.
5. Header: [Engine/include/Engine/Entity.hpp](Engine/include/Engine/Entity.hpp); registered in [Engine/CMakeLists.txt](Engine/CMakeLists.txt).

---

#### REQ-ECS-002 — `Engine::World` registry core (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine layer |
| **Depends on** | REQ-ECS-001 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. `Engine::World` supports:
   - `CreateEntity() -> Entity`
   - `DestroyEntity(Entity)` — deferred destruction until end of frame or explicit `FlushDeferred()` (document chosen policy).
2. Component API (templates or type-erased with code-gen — prefer **templates** for zero overhead):
   - `AddComponent<T>(Entity, T) -> T&`
   - `GetComponent<T>(Entity) -> T*` (nullptr if missing)
   - `HasComponent<T>(Entity) -> bool`
   - `RemoveComponent<T>(Entity)`
3. Iteration API for systems:
   - `Each<Ts...>(callable)` or `View<Ts...>()` returning contiguous ranges over matching entities.
4. **Structure-of-Arrays (SoA):** each component type stored in its own `std::vector`, indexed by entity slot — not `std::map<Entity, T>`.
5. No heap allocation in `Each`/`View` iteration hot path after world is populated.
6. Headers under [Engine/include/Engine/ECS/](Engine/include/Engine/ECS/); implementation in [Engine/src/ECS/](Engine/src/ECS/) or header-only if small.
7. Unit tests: create/destroy, add/get/remove, stale handle rejection, iteration count matches expected entities.

---

#### REQ-ECS-003 — Component design rules (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine + Game |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Components are **POD structs** (trivially copyable where possible): **state only, no virtual methods, no gameplay logic**.
2. Document and enforce maximum component size guideline (e.g. ≤ 64 bytes unless justified).
3. **Engine components** (reusable across games):

   | Component | Fields | Notes |
   |-----------|--------|-------|
   | `Transform2D` | existing `position`, `rotation`, `scale` | Becomes a world component; keep [Transform2D.hpp](Engine/include/Engine/Transform2D.hpp) |
   | `PreviousTransform2D` | same layout as `Transform2D` | Snapshot for fixed-timestep render lerp |
   | `Velocity2D` | `sf::Vector2f linear` | Optional; may derive from input each frame instead |

4. **Game components** (gameplay-specific, namespace `Game::`):

   | Component | Fields | Notes |
   |-----------|--------|-------|
   | `PlayerTag` | empty or `uint8_t pad` | Marker for player entity |
   | `MoveSpeed` | `float unitsPerSecond` | From `GameplaySettings::playerSpeed` |
   | `RectangleShapeVisual` | `float width`, `float height`, `sf::Color fill` | Visual params — not the SFML object |
   | `TextVisual` | `std::string text`, `unsigned charSize`, `sf::Color fill` | For menu UI entities |
   | `PulseAnimation` | `float timer`, `float speed`, `float minAlpha`, `float maxAlpha` | Menu prompt pulsing |

5. Tag components (`PlayerTag`) have no behavior — systems query them for filtering only.

---

#### REQ-ECS-004 — System interface and scheduling (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine layer |
| **Depends on** | REQ-ECS-002 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Abstract interface `Engine::ISystem` with:
   - `OnFixedUpdate(World&, float fixedDeltaTime)` — deterministic simulation
   - `OnUpdate(World&, float deltaTime)` — real-time (input edge detection, animations)
   - `OnRender(World&, sf::RenderWindow&, float alpha)` — drawing only
2. `Engine::SystemRunner` (or methods on `World`) registers systems in **explicit order**; order is documented and tested.
3. Recommended default order:

   | Phase | Systems | Timestep |
   |-------|---------|----------|
   | Real-time | `InputSystem`, `UIAnimationSystem` | `OnUpdate` |
   | Fixed | `MovementSystem`, `ActionSystem` | `OnFixedUpdate` |
   | Render | `RenderSystem` | `OnRender` |

4. Systems are **stateless** — no entity-specific member variables; all state lives in components.
5. External dependencies (`InputManager`, `FontManager`, `TextureManager`) passed via a **`SystemContext`** struct per frame, not stored globally.

---

#### REQ-ECS-005 — `MovementSystem` (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **Scope** | Game layer |
| **Depends on** | REQ-ECS-003, REQ-ECS-004 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Queries entities with `Transform2D` + `PreviousTransform2D` + `MoveSpeed` + input-driven velocity (via `Velocity2D` or inline direction from context).
2. On each fixed step:
   - Copy `Transform2D` → `PreviousTransform2D`.
   - Read `GameAction` held states from `SystemContext::input`.
   - Normalize digital direction via `Engine::Math::NormalizeDigitalDirection`.
   - Update `Transform2D.position` by `direction * MoveSpeed * fixedDeltaTime`.
3. Behavior matches current [GameplayScene.cpp](Game/src/GameplayScene.cpp) movement exactly (cardinal + diagonal speed).
4. Does not allocate per entity per frame.

---

#### REQ-ECS-006 — `RenderSystem` with drawable pool (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **Scope** | Engine + Game |
| **Depends on** | REQ-ECS-003, REQ-ECS-004 |
| **Status** | ❌ Not started |

**Problem:** SFML drawables (`sf::RectangleShape`, `sf::Text`) are not POD and hold GPU-related state. They must **not** live inside hot-path component arrays.

**Acceptance criteria**

1. `Engine::DrawablePool` (or scene-local pool owned by `World`) maps `Entity` → concrete SFML drawable, created when visual components are added, destroyed with entity.
2. `RenderSystem`:
   - Iterates entities with `Transform2D` + a visual component (`RectangleShapeVisual` or `TextVisual`).
   - Lerps `PreviousTransform2D` and `Transform2D` via `Engine::Lerp` using `alpha`.
   - Applies transform to pooled drawable; calls `window.draw()`.
3. Text entities resolve fonts through `SystemContext::fonts` (same as current menu scene).
4. Origin setup (centered player square) happens once at entity spawn, not every frame.
5. No `new`/`delete` per frame in render path.

---

#### REQ-ECS-007 — `InputSystem` / `ActionSystem` (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **Scope** | Game layer |
| **Depends on** | REQ-ECS-004 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. `InputSystem::OnUpdate` handles **one-shot** actions (`Jump`, `Shoot`, `Confirm`, scene transitions) via `IsActionJustPressed`.
2. `ActionSystem` or inline handlers log or dispatch events — preserve current Jump/Shoot console log behavior until real gameplay exists.
3. Menu **Confirm** transitions scene via callback or `SystemContext::sceneManager` — not hard-coded in Engine.
4. Movement held-actions remain in `MovementSystem` (fixed step) or `InputSystem` writes `Velocity2D` (document single owner to avoid duplication).

---

#### REQ-ECS-008 — Migrate `GameplayScene` to ECS (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **File** | [GameplayScene.cpp](Game/src/GameplayScene.cpp), [GameplayScene.hpp](Game/include/Game/GameplayScene.hpp) |
| **Depends on** | REQ-ECS-005, REQ-ECS-006, REQ-ECS-007 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Remove `m_Player`, `m_PreviousTransform`, `m_CurrentTransform` from `GameplayScene`.
2. Scene owns `Engine::World m_World` and registers game systems in `OnEnter`.
3. `OnEnter` spawns player entity with: `Transform2D`, `PreviousTransform2D`, `MoveSpeed`, `RectangleShapeVisual`, `PlayerTag`.
4. `OnFixedUpdate` → `m_World.FixedUpdate(dt, context)`; `OnUpdate` → `m_World.Update(...)`; `OnRender` → `m_World.Render(...)`.
5. `OnExit` destroys all entities or resets world.
6. **Parity:** player size, color, start position, speed, interpolation smoothness unchanged vs pre-migration.

---

#### REQ-ECS-009 — Migrate `MainMenuScene` to ECS (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **File** | [MainMenuScene.cpp](Game/src/MainMenuScene.cpp) |
| **Depends on** | REQ-ECS-006, REQ-ECS-007 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. Title and prompt are separate entities with `TextVisual` + `Transform2D` (position-only; scale/rotation default).
2. Prompt entity has `PulseAnimation`; `UIAnimationSystem` updates alpha in `OnUpdate`.
3. Remove direct `sf::Text` members from scene class.
4. Visual parity with current menu (fonts, colors, pulsing prompt, Enter to play).

---

#### REQ-ECS-010 — ECS unit tests (High)

| Field | Value |
|-------|-------|
| **Priority** | High |
| **Location** | [Engine/tests/test_World.cpp](Engine/tests/test_World.cpp), [Game/tests/test_MovementSystem.cpp](Game/tests/test_MovementSystem.cpp) (or Engine if systems live there) |
| **Status** | ❌ Not started |

**Minimum test cases**

| Test | Expected |
|------|----------|
| Create / destroy entity | Handle invalid after destroy; generation prevents reuse bugs |
| Add / get / remove component | Pointer stability within same frame; missing component returns nullptr |
| `Each` iteration | Visits only entities with all required components |
| MovementSystem fixed step | Position delta matches `speed * dt` for unit direction |
| Render lerp | Interpolated transform at `alpha=0.5` matches `Engine::Lerp` |
| Stale handle | Operations on destroyed entity handle fail safely (no crash) |

Tests must not require SFML window or GPU except where headless SFML drawables are already used (`test_Transform2D` pattern).

---

#### REQ-ECS-011 — Performance constraints (Critical)

| Field | Value |
|-------|-------|
| **Priority** | Critical |
| **Scope** | Engine + Game |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. **No allocations** in `World::FixedUpdate`, `World::Update`, `World::Render` system loops for steady-state gameplay (after scene `OnEnter` setup).
2. **No RTTI** (`dynamic_cast`, `typeid`) in system hot paths.
3. **No virtual dispatch per entity** inside iteration — virtual calls allowed once per system per frame, not per entity.
4. Component arrays remain **contiguous** (`std::vector`); avoid node-based containers (`std::list`, `std::map`) for component storage.
5. Document cache-friendly component layout: hot fields together, avoid padding bloat; prefer `float` over `double` for 2D game.
6. Profile baseline (Debug + Release): record frame time before/after migration with 1 player entity; document in PR — ECS must not regress Release performance.

---

#### REQ-ECS-012 — Remove dead OOP gameplay code (Medium)

| Field | Value |
|-------|-------|
| **Priority** | Medium |
| **Depends on** | REQ-ECS-008, REQ-ECS-009 |
| **Status** | ❌ Not started |

**Acceptance criteria**

1. No scene class contains `sf::RectangleShape`, `sf::Text`, or per-object transform members for simulated objects.
2. No gameplay logic (movement, pulsing, action handling) remains in scene methods except world/system delegation.
3. Update [README.md](README.md) architecture section to describe ECS + Scene hybrid.

---

### 9.3 Senior Implementation Guidelines (ECS)

#### 9.3.1 Hybrid model — what stays OOP vs what becomes ECS

| Layer | Pattern | Examples |
|-------|---------|----------|
| **Shell & flow** | OOP | `Application`, `SceneManager`, `Scene` lifecycle, `SettingsManager` |
| **Services** | OOP singletons on Application | `InputManager`, `FontManager`, `DisplaySyncController` |
| **Simulation** | ECS | Player, enemies, bullets, menu text entities |
| **Systems** | Stateless functors / small classes | `MovementSystem`, `RenderSystem` |

Scenes answer: *which world exists, which systems run, which entities spawn*. They do **not** answer: *how does this entity move*.

---

#### 9.3.2 Entity handle with generation (recommended)

```cpp
namespace Engine {

struct Entity {
    uint32_t index{0};
    uint32_t generation{0};

    [[nodiscard]] bool IsValid() const noexcept {
        return generation != 0;
    }
};

} // namespace Engine
```

On destroy: increment slot generation, mark slot free. Reuse index only after generation bump so old handles invalidate.

---

#### 9.3.3 Component storage — SoA registry sketch

```cpp
// Per component type T — one pool
template<typename T>
struct ComponentPool {
    std::vector<T> data;
    std::vector<Entity> entities; // parallel, same order as data
};

// World maps type -> pool; Each<View...> merges by entity index
```

**Do**

- Iterate with index-based loops: `for (size_t i = 0; i < count; ++i)`.
- Reserve capacity in `OnEnter` when entity count is known.
- Keep `Transform2D` and `PreviousTransform2D` in separate pools but aligned by entity index.

**Do not**

- Store `std::function` or `std::string` in hot components updated every frame ( `TextVisual::text` is setup-time only).
- Store raw `Entity` pointers across frames without generation check.
- Put SFML drawables inside component structs.

---

#### 9.3.4 System context — inject dependencies

```cpp
struct SystemContext {
    const InputManager& input;
    FontManager& fonts;
    SceneManager* sceneManager{nullptr}; // Game-only transitions
    sf::Vector2f designSize{0.f, 0.f};
};
```

Pass `SystemContext` by const reference into `World::FixedUpdate/Update/Render`. Systems never reach for global singletons.

---

#### 9.3.5 Fixed timestep + interpolation (preserve existing model)

| Step | Owner | Action |
|------|-------|--------|
| Fixed tick | `MovementSystem` | `previous = current;` update `Transform2D` |
| Render frame | `RenderSystem` | `render = Lerp(previous, current, alpha);` apply to drawable |

Same contract as [REQ-TRANSFORM-004](requirements.md) and Gaffer on Games fixed timestep — ECS changes **where** state lives, not **how** time is stepped.

---

#### 9.3.6 Drawable pool pattern (SFML bridge)

```cpp
// Spawn (OnEnter) — once per entity
auto entity = world.CreateEntity();
world.AddComponent<RectangleShapeVisual>(entity, { size, size, sf::Color::Cyan });
pool.CreateRectangle(entity, size, size); // sets origin, fill from component

// Render (every frame) — hot path
pool.WithRectangle(entity, [&](sf::RectangleShape& shape) {
    renderTransform.ApplyTo(shape);
    window.draw(shape);
});
```

Engine owns pool mechanics; Game owns visual component definitions.

---

#### 9.3.7 System ordering and single responsibility

```text
OnUpdate (real-time, once per frame)
  1. InputSystem        — edge-triggered actions, scene switch requests
  2. UIAnimationSystem  — pulse alpha, non-physics motion

OnFixedUpdate (60 Hz or settings-driven)
  1. MovementSystem     — position integration from held actions

OnRender (every frame)
  1. RenderSystem       — lerp + draw all visual entities
```

One system owns one concern. Do not read keyboard state inside `RenderSystem`.

---

#### 9.3.8 Migration sequence (recommended PR order)

| PR | Scope | Outcome |
|----|-------|---------|
| **PR-1** | REQ-ECS-001, REQ-ECS-002, REQ-ECS-010 (core tests) | `World` usable in isolation |
| **PR-2** | REQ-ECS-003, REQ-ECS-004, Engine components | System runner + context |
| **PR-3** | REQ-ECS-005, REQ-ECS-006, REQ-ECS-007 | Game systems + drawable pool |
| **PR-4** | REQ-ECS-008 | GameplayScene migrated; OOP player removed |
| **PR-5** | REQ-ECS-009, REQ-ECS-012 | Menu migrated; docs updated |

Each PR must pass all existing tests plus new ECS tests; no behavioral regression in Release build.

---

#### 9.3.9 Engine vs Game boundary (ECS)

| Item | Layer | Namespace |
|------|-------|-----------|
| `Entity`, `World`, `ISystem`, `SystemRunner`, `DrawablePool` | Engine | `Engine::` |
| `Transform2D`, `PreviousTransform2D`, `Lerp`, `ApplyTo` | Engine | `Engine::` |
| `PlayerTag`, `MoveSpeed`, `MovementSystem`, `ActionSystem` | Game | `Game::` |
| `TextVisual`, `PulseAnimation`, `UIAnimationSystem` | Game | `Game::` |

Game links against Engine; Engine never includes Game headers.

---

#### 9.3.10 Naming conventions (ECS)

| Item | Convention | Example |
|------|------------|---------|
| Component structs | `PascalCase` noun | `MoveSpeed`, `PlayerTag` |
| Tag components | `*Tag` suffix | `PlayerTag` |
| Systems | `PascalCase` + `System` | `MovementSystem` |
| World methods | `PascalCase` | `CreateEntity`, `AddComponent` |
| Component members | `camelCase` | `unitsPerSecond` |
| Query / view types | `PascalCase` | `MovementView` |

---

#### 9.3.11 Performance anti-patterns to reject

| Anti-pattern | Why |
|--------------|-----|
| `std::map<Entity, Component>` | Cache misses, heap nodes |
| Virtual `Component` base class | Forces OOP back into data layer |
| `dynamic_cast` per entity | RTTI cost in hot loop |
| Spawning/destroying entities every frame | Allocation churn; use pools for bullets |
| Giant "System" that does input + physics + render | Unmaintainable; breaks ordering |
| Storing `shared_ptr` in components | Atomic ref-count in hot path |

**Profile before micro-optimizing.** ECS enables performance; it does not guarantee it. Use Visual Studio Profiler or Tracy on **Release** builds after PR-4.

---

#### 9.3.12 Relationship to existing `Transform2D` work

`Engine::Transform2D` remains the canonical spatial component (REQ-TRANSFORM-001–006 stay valid). Migration **moves** transform state from scene members into `World` component storage — do not rewrite lerp/apply math unless a bug is found.

---

### 9.4 ECS — Planned files

| Action | File | Status |
|--------|------|--------|
| Create | [Engine/include/Engine/Entity.hpp](Engine/include/Engine/Entity.hpp) | ❌ |
| Create | [Engine/include/Engine/ECS/World.hpp](Engine/include/Engine/ECS/World.hpp) | ❌ |
| Create | [Engine/include/Engine/ECS/ISystem.hpp](Engine/include/Engine/ECS/ISystem.hpp) | ❌ |
| Create | [Engine/include/Engine/ECS/SystemContext.hpp](Engine/include/Engine/ECS/SystemContext.hpp) | ❌ |
| Create | [Engine/include/Engine/ECS/DrawablePool.hpp](Engine/include/Engine/ECS/DrawablePool.hpp) | ❌ |
| Create | [Engine/src/ECS/World.cpp](Engine/src/ECS/World.cpp) | ❌ |
| Create | [Engine/tests/test_World.cpp](Engine/tests/test_World.cpp) | ❌ |
| Create | [Game/include/Game/ECS/Components.hpp](Game/include/Game/ECS/Components.hpp) | ❌ |
| Create | [Game/include/Game/ECS/MovementSystem.hpp](Game/include/Game/ECS/MovementSystem.hpp) | ❌ |
| Create | [Game/include/Game/ECS/RenderSystem.hpp](Game/include/Game/ECS/RenderSystem.hpp) | ❌ |
| Create | [Game/include/Game/ECS/InputSystem.hpp](Game/include/Game/ECS/InputSystem.hpp) | ❌ |
| Create | [Game/include/Game/ECS/UIAnimationSystem.hpp](Game/include/Game/ECS/UIAnimationSystem.hpp) | ❌ |
| Modify | [GameplayScene.cpp](Game/src/GameplayScene.cpp) — delegate to `World` | ❌ |
| Modify | [MainMenuScene.cpp](Game/src/MainMenuScene.cpp) — delegate to `World` | ❌ |
| Modify | [Engine/CMakeLists.txt](Engine/CMakeLists.txt), [Game/CMakeLists.txt](Game/CMakeLists.txt) | ❌ |

---

### 9.5 ECS verification checklist

1. **Registry:** Create 100 entities with `Transform2D`; destroy 50; recreate 50 — no crashes, stale handles rejected.
2. **Movement parity:** Player moves at `playerSpeed` from settings; diagonal speed matches pre-ECS behavior.
3. **Interpolation:** No jitter at 60 Hz fixed step with V-Sync off and FPS cap 120.
4. **Input:** Jump/Shoot log once per press; Confirm switches menu → gameplay.
5. **Menu:** Title and prompt visible; prompt pulses; fonts load from `Assets/Fonts/`.
6. **Allocations:** Debug heap profiling shows no per-frame allocations during 60 s gameplay loop (steady state).
7. **Tests:** All Engine + Game tests pass; new ECS tests pass in Debug and Release.
8. **Warnings:** Builds clean under `/W4 /WX` and `-Wall -Wextra -Werror`.

---

### 9.6 ECS references

| Topic | Source |
|-------|--------|
| ECS principles & agent rules | [game_development SKILL.md](.agents/skills/game_development/SKILL.md) |
| Data-oriented design | [Data-Oriented Design — Noel Berry](https://www.dataorienteddesign.com/dodbook/) |
| Fixed timestep + render alpha | [Fix Your Timestep — Gaffer on Games](https://gafferongames.com/post/fix_your_timestep/) |
| Existing transform component | [Engine/include/Engine/Transform2D.hpp](Engine/include/Engine/Transform2D.hpp) |
| Current gameplay baseline | [Game/src/GameplayScene.cpp](Game/src/GameplayScene.cpp) |
