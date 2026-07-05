# My Cool C++ Game

This is a modern C++20 game engine and sandbox built using SFML. The project features a clean architecture, robust resource management, deterministic fixed-timestep simulation, data-driven configuration, and high-quality coding standards.

## Features

- **Fixed Timestep Game Loop:** Avoids the "spiral of death" with a capped maximum substeps and smooth render interpolation.
- **Robust Resource Management:** `ResourceManager` gracefully falls back to placeholders instead of crashing when assets fail to load.
- **Action-based Input System:** Gameplay logic is completely decoupled from raw keycodes via `GameAction` mappings.
- **Display Synchronization:** Built-in controller managing V-Sync and FPS limits appropriately.
- **Data-Driven Configuration:** Loads speeds, display settings, and input mapping dynamically from `Assets/Config/settings.json`.
- **Unit Tested & Production-Ready Logging:** Test coverage via Catch2 and robust, stripped-in-release logging using `spdlog`.

## Building the Project

### Prerequisites
- CMake 3.25+
- Visual Studio 2022 / MSVC (Windows)
- C++20 compatible compiler

### Build Steps

1. Configure the project:
   ```cmd
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
   ```

2. Build the project:
   ```cmd
   cmake --build build --config Debug
   ```
   *(Note: The `Assets/` directory is automatically copied to the output directory post-build).*

3. Run the tests:
   ```cmd
   ctest --test-dir build -C Debug
   ```

### Running the Game
The compiled executable is located at:
`build/bin/Debug/GameApp.exe`

If you open the `.sln` in Visual Studio, you can set `GameApp` as the startup project and run it directly with `F5`. The working directory is automatically configured to point to the correct output location.

## Controls

The default key bindings (configurable in `Assets/Config/settings.json`) are:

| Action | Key |
|---|---|
| **Move Up** | `W` |
| **Move Down** | `S` |
| **Move Left** | `A` |
| **Move Right** | `D` |
| **Jump** | `Space` |
| **Shoot** | `Left Ctrl` |
| **Confirm (Menu)** | `Enter` |
| **Pause** | `Escape` |
| **Toggle Debug Overlay** | `R` |
| **Toggle V-Sync** | `V` |

## Asset Layout

The game dynamically resolves its asset root relative to the executable path.

```text
Assets/
├── Config/
│   └── settings.json       # Generated on first run
└── Fonts/
    └── Roboto-Regular.ttf  # Shipped open-source font
```
