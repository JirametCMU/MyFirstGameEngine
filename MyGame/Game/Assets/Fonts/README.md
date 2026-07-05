# Game Fonts Directory

Place your `.ttf` or `.otf` font files in this folder.

They will be **automatically discovered and loaded** when the game starts.

## How it works

1. Drop any `.ttf` or `.otf` file into this folder
2. The game loads all fonts on startup via `ResourceManager::LoadDirectory()`
3. Each font is registered using its **filename** (without extension) as the ID
4. The **first font alphabetically** becomes the default used by the FPS counter and main menu

## Changing the font

Simply replace the font file here and restart the game. No code changes needed.

## Using a specific font by name

If you have multiple fonts (e.g., `Roboto.ttf` and `PressStart2P.ttf`), you can
reference them in code by their filename:

```cpp
auto& font = GetFonts().Get("Roboto");       // Gets Roboto.ttf
auto& font = GetFonts().Get("PressStart2P"); // Gets PressStart2P.ttf
auto& font = GetFonts().GetDefault();        // Gets whichever comes first alphabetically
```
