---
name: game-development
description: Best practices and instructions for high-performance C++ game engine development, clean code patterns, scaling, and agent behavior guidelines.
---

# Game Development & Engine Engineering Skill

This guide outlines core principles, coding rules, and system architectures for developing high-performance, scalable game engines and games in C++.

---

## 1. Repeat Behavior of AI (System Constraints)

When working on this project, the AI must strictly adhere to the following behaviors:
* **Modern C++ Standard**: Always use C++20 features (e.g., standard library concepts, modules if supported, `std::string_view`, structured bindings) and avoid legacy C-style idioms where safer C++ alternatives exist.
* **Preserve Documentation**: Retain all existing code comments, docstrings, and license headers unless explicitly requested to modify them.
* **Cross-Platform Compatibility**: Avoid platform-specific functions (e.g., direct Windows API calls) in common code. Abstract platform dependencies inside the `Platform` layer or `Engine` subsystem.
* **Warning Hygiene**: Write clean, compliant code that compiles without warnings under `/W4` (MSVC) or `-Wall -Wextra` (Clang/GCC).
* **Clickable File Links**: Always link to files and directories using absolute markdown URLs: `[filename](file:///C:/path/to/file)`.
* **Project-Scoped Documents**: When writing text, plans, expansions, or reviews in a file (e.g., `.md`), save it inside the project folder. Do not create such files outside the project root.

---

## 2. Clean Code Writing

C++ codebases can degrade quickly. Maintain clarity with these rules:
* **Separation of Concerns**: Keep declaration (`.hpp`) separated from definition (`.cpp`) files. Inline code should be reserved only for small, high-frequency accessors or template functions.
* **Naming Conventions**:
  * Classes/Structs: `PascalCase` (e.g., `Application`, `RenderPipeline`).
  * Functions/Methods: `PascalCase` (e.g., `OnUpdate`, `Initialize`).
  * Variables/Parameters: `camelCase` (e.g., `deltaTime`, `transformComponent`).
  * Private Member Variables: Prefix with `m_` (e.g., `m_Running`, `m_FrameCount`).
* **Explicit Resource Management**: Prefer RAII (Resource Acquisition Is Initialization). Use smart pointers (`std::unique_ptr` for exclusive ownership, `std::shared_ptr` for shared ownership) to avoid memory leaks. Avoid raw pointers (`*`) unless used as non-owning, optional observers.
* **Single Responsibility Principle (SRP)**: Do not create "God Classes" (like a giant `GameObject` that handles drawing, physics, sound, and networking). Split behavior into discrete components or services.

---

## 3. Performance & Code Optimization

Game loops run dozens or hundreds of times per second. Every millisecond counts:
* **Minimize Loop Allocations**: Avoid calling `new`, `delete`, `malloc`, `free`, or creating dynamic containers (like `std::vector` resizing or `std::string` concatenation) inside hot paths (`OnUpdate`, `OnRender`).
* **Object Pooling**: Pre-allocate memory buffers or objects during initialization (e.g., particle systems, bullets, enemy entities) and recycle them rather than creating them on the fly.
* **Cache Locality (Data-Oriented Design)**: 
  * Layout data contiguously in memory. Prefer `std::vector` over linked lists (`std::list`) or pointer-heavy maps.
  * Arrange struct components so that members commonly accessed together are placed next to each other in memory, maximizing CPU cache line usage (L1/L2).
* **Pass by Reference**: Pass large objects by const reference (`const T&`) to avoid duplicate copying, and use move semantics (`std::move`) for transfer of ownership.
* **Compiler Optimization**: Ensure performance profiling is done *only* on **Release builds** with optimizations enabled (`/O2` on MSVC, `-O3` on GCC/Clang). Enable Link-Time Optimization (LTO) in CMake.

---

## 4. Best Architecture for Scaling

To support a growing feature set, structure the engine using these patterns:
* **Engine vs. Game Boundary**: The game should never call low-level renderer APIs directly. The Engine provides abstract interfaces, and the Game implements gameplay components that consume those interfaces.
* **Entity-Component-System (ECS)**:
  * **Entities**: Just unique integers/handles that identify an object.
  * **Components**: Plain-Old-Data (POD) structures containing only state (no gameplay logic, e.g., `PositionComponent`, `VelocityComponent`).
  * **Systems**: Controllers containing logic that iterate through arrays of entities with matching components (e.g., `PhysicsSystem`, `RenderSystem`).
* **Subsystem Modularity**: Define distinct subsystems (e.g., `AudioDevice`, `Renderer`, `InputManager`) with abstract base interfaces to allow hot-swapping or testing with mock implementations.
* **Data-Driven Workflows**: Move gameplay parameters (such as health, speeds, spawning, assets paths) into external configuration files (JSON or YAML) to allow balance tuning without code recompilation.

---

## 5. Development Workflow & Profiling

* **Profile First**: Never perform speculative optimization. Always use a profiler (e.g., Tracy, Optick, or Visual Studio Profiler) to measure where CPU/GPU bottleneck cycles actually happen.
* **Build Infrastructure**: Maintain a clean, cross-platform build system using CMake. Treat external dependencies cleanly using CMake FetchContent or package managers (`vcpkg`).
