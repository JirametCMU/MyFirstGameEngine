# MyGame — Requirements, Guidelines & Quality Tracker

This document defines **current** requirements, implementation guidelines, and tracked quality findings for the MyGame engine and game layer.

**Legend:** ✅ Implemented · ⚠️ Partial · ❌ Not started

**Initial scan:** 2026-07-05 · **Last verified:** 2026-07-05 · Scope: full codebase (~44 source files — Engine, Game, CMake, tests)

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
| ECS component style | [game_development SKILL.md](.agents/skills/game_development/SKILL.md) |
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

---

### 8.6 Recommended fix priority

| Priority | IDs | Rationale |
|----------|-----|-----------|
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
