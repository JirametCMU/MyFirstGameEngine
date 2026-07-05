#pragma once

#include "Engine/ResourceManager.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Engine {

    /**
     * Convenient type aliases for the most common SFML resource types.
     *
     * Instead of writing ResourceManager<sf::Texture> everywhere, the Game
     * can simply use Engine::TextureManager, Engine::FontManager, etc.
     *
     * Usage:
     *   auto& textures = GetTextures();    // returns TextureManager&
     *   auto& fonts    = GetFonts();       // returns FontManager&
     *   auto& sounds   = GetSounds();      // returns SoundBufferManager&
     */

    /** Manages sf::Texture resources (images for sprites, tilesets, etc.) */
    using TextureManager = ResourceManager<sf::Texture>;

    /** Manages sf::Font resources (TrueType/OpenType fonts for text rendering) */
    using FontManager = ResourceManager<sf::Font>;

    /** Manages sf::SoundBuffer resources (short audio clips for sound effects) */
    using SoundBufferManager = ResourceManager<sf::SoundBuffer>;

} // namespace Engine
