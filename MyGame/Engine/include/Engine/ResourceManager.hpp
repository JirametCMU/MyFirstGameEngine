#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include "Engine/Log.hpp"

namespace Engine {

    /**
     * ResourceManager<T> — A generic, template-based asset cache for SFML resources.
     *
     * Manages the loading, caching, retrieval, and unloading of any SFML resource
     * type that supports `loadFromFile(const std::string&)` — such as sf::Texture,
     * sf::Font, and sf::SoundBuffer.
     *
     * How it works:
     *   - Resources are stored in a hash map keyed by a string ID (e.g., "player_texture").
     *   - When Load() is called, the manager first checks the cache. If the resource
     *     is already loaded, it returns the cached version (no disk I/O).
     *   - If the resource is not cached, it loads it from disk, stores it, and returns it.
     *   - If loading fails, a warning is logged to std::cerr and a fallback resource
     *     is returned instead of crashing (Req 1.3 — Resiliency).
     *   - Each resource is owned by a std::unique_ptr, so memory is automatically
     *     freed when the manager is destroyed or when Unload()/Clear() is called.
     *
     * Fallback behavior:
     *   - sf::Texture: Generates a 16×16 magenta placeholder texture.
     *   - Other types: Returns a default-constructed (empty) placeholder.
     *
     * Directory loading:
     *   - LoadDirectory() scans a folder for matching file extensions and loads all
     *     found assets automatically, keyed by filename (without extension).
     *   - The first resource successfully loaded becomes the "default" resource,
     *     retrievable via GetDefault().
     *
     * Usage example:
     *   auto& fonts = GetFonts();
     *   fonts.LoadDirectory("Assets/Fonts/", {".ttf", ".otf"});
     *   auto& defaultFont = fonts.GetDefault();
     *
     * Template requirement:
     *   T must have a method: bool loadFromFile(const std::string& filename)
     */
    template <typename T>
    class ResourceManager {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        // Prevent copying (resources are unique-owned)
        ResourceManager(const ResourceManager&) = delete;
        auto operator=(const ResourceManager&) -> ResourceManager& = delete;

        // Allow moving
        ResourceManager(ResourceManager&&) noexcept = default;
        auto operator=(ResourceManager&&) noexcept -> ResourceManager& = default;

        /**
         * Loads a resource from the given filepath and caches it under the given ID.
         * If a resource with the same ID is already loaded, returns the cached version
         * without reloading from disk.
         *
         * If loading fails, logs a warning to std::cerr and returns a valid fallback
         * resource instead of throwing. For sf::Texture, the fallback is a 16×16
         * magenta image. For other types, a default-constructed empty resource is used.
         *
         * The first successfully loaded resource automatically becomes the default.
         *
         * @param id       A unique string key to identify this resource (e.g., "player").
         * @param filepath The path to the file on disk (e.g., "Assets/Textures/player.png").
         * @return         A reference to the loaded (or fallback) resource.
         */
        auto Load(const std::string& id, const std::string& filepath, bool* outSuccess = nullptr) -> T&
        {
            // Return cached resource if already loaded
            if (auto it = m_Resources.find(id); it != m_Resources.end())
            {
                ENGINE_TRACE("[ResourceManager] Cache hit: \"{}\"", id);
                if (outSuccess) *outSuccess = true;
                return *(it->second);
            }

            // Load new resource from disk
            auto resource = std::make_unique<T>();
            if (!resource->loadFromFile(filepath))
            {
                ENGINE_WARN("[ResourceManager] Failed to load resource \"{}\" from: {} — using fallback placeholder.", id, filepath);
                if (outSuccess) *outSuccess = false;

                // Generate a type-specific fallback resource
                auto fallback = std::make_unique<T>();
                if constexpr (std::is_same_v<T, sf::Texture>)
                {
                    // Create a visible 16x16 magenta texture so missing assets are obvious
                    sf::Image image;
                    image.create(16, 16, sf::Color::Magenta);
                    fallback->loadFromImage(image);
                }
                // For other types (sf::Font, sf::SoundBuffer, etc.), the default-constructed
                // empty resource serves as a safe placeholder.

                auto& ref = *fallback;
                m_Resources.emplace(id, std::move(fallback));
                return ref;
            }

            ENGINE_INFO("[ResourceManager] Loaded: \"{}\" from {}", id, filepath);
            if (outSuccess) *outSuccess = true;

            // Track the first successfully loaded resource as the default
            if (m_DefaultId.empty())
            {
                m_DefaultId = id;
            }

            // Store and return a reference
            auto& ref = *resource;
            m_Resources.emplace(id, std::move(resource));
            return ref;
        }

        /**
         * Scans a directory for files matching the given extensions and loads each one.
         *
         * Each file is registered with its filename (without extension) as the ID.
         * Files are loaded in alphabetical order for deterministic behavior.
         * The first file successfully loaded becomes the default resource.
         *
         * @param directory  Path to the directory to scan (e.g., "Assets/Fonts/").
         * @param extensions List of allowed extensions including dot (e.g., {".ttf", ".otf"}).
         * @return           The number of resources successfully loaded.
         */
        auto LoadDirectory(const std::string& directory,
                           const std::vector<std::string>& extensions) -> std::size_t
        {
            namespace fs = std::filesystem;

            if (!fs::exists(directory) || !fs::is_directory(directory))
            {
                ENGINE_WARN("[ResourceManager] Directory not found: {}", directory);
                return 0;
            }

            // Collect matching file paths, then sort for deterministic load order
            std::vector<fs::path> matchingFiles;

            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (!entry.is_regular_file())
                {
                    continue;
                }

                auto ext = entry.path().extension().string();
                // Case-insensitive extension comparison
                std::transform(ext.begin(), ext.end(), ext.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                for (const auto& allowedExt : extensions)
                {
                    std::string lowerAllowed = allowedExt;
                    std::transform(lowerAllowed.begin(), lowerAllowed.end(), lowerAllowed.begin(),
                        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                    
                    if (ext == lowerAllowed)
                    {
                        matchingFiles.push_back(entry.path());
                        break;
                    }
                }
            }

            // Sort alphabetically by filename for deterministic load order
            std::sort(matchingFiles.begin(), matchingFiles.end(),
                [](const fs::path& a, const fs::path& b) {
                    return a.filename().string() < b.filename().string();
                });

            std::size_t loadedCount = 0;
            for (const auto& filePath : matchingFiles)
            {
                auto id = filePath.stem().string(); // filename without extension
                bool success = false;
                Load(id, filePath.string(), &success);
                if (success) {
                    ++loadedCount;
                }
            }

            ENGINE_INFO("[ResourceManager] LoadDirectory: loaded {} resource(s) from {}", loadedCount, directory);

            return loadedCount;
        }

        /**
         * Retrieves a previously loaded resource by its ID.
         * Throws std::runtime_error if the resource has not been loaded yet.
         *
         * @param id The string key used when the resource was loaded.
         * @return   A const reference to the resource.
         */
        [[nodiscard]] auto Get(const std::string& id) const -> const T&
        {
            auto it = m_Resources.find(id);
            if (it == m_Resources.end())
            {
                ENGINE_ERROR("[ResourceManager] Resource not found: \"{}\". Returning fallback.", id);
                return GetFallback();
            }
            return *(it->second);
        }

        /**
         * Retrieves a previously loaded resource by its ID (mutable version).
         * Throws std::runtime_error if the resource has not been loaded yet.
         *
         * @param id The string key used when the resource was loaded.
         * @return   A mutable reference to the resource.
         */
        auto Get(const std::string& id) -> T&
        {
            auto it = m_Resources.find(id);
            if (it == m_Resources.end())
            {
                ENGINE_ERROR("[ResourceManager] Resource not found: \"{}\". Returning fallback.", id);
                return const_cast<T&>(GetFallback());
            }
            return *(it->second);
        }

        /**
         * Returns the default resource — the first one that was successfully loaded.
         * Throws if no resources have been loaded yet.
         */
        [[nodiscard]] auto GetDefault() const -> const T&
        {
            return Get(m_DefaultId);
        }

        /** Mutable version of GetDefault(). */
        auto GetDefault() -> T&
        {
            return Get(m_DefaultId);
        }

        /**
         * Returns true if a default resource exists (at least one resource was loaded).
         */
        [[nodiscard]] auto HasDefault() const -> bool
        {
            return !m_DefaultId.empty() && IsLoaded(m_DefaultId);
        }

        /**
         * Checks whether a resource with the given ID is currently loaded in the cache.
         *
         * @param id The string key to check.
         * @return   True if the resource is loaded, false otherwise.
         */
        [[nodiscard]] auto IsLoaded(const std::string& id) const -> bool
        {
            return m_Resources.contains(id);
        }

        /**
         * Unloads and frees a single resource by its ID.
         * Does nothing if the resource is not found.
         *
         * @param id The string key of the resource to unload.
         */
        void Unload(const std::string& id)
        {
            if (auto it = m_Resources.find(id); it != m_Resources.end())
            {
                ENGINE_INFO("[ResourceManager] Unloaded: \"{}\"", id);
                m_Resources.erase(it);
            }
        }

        /**
         * Unloads and frees all cached resources.
         * Useful for level transitions or shutdown.
         */
        void Clear()
        {
            ENGINE_INFO("[ResourceManager] Clearing all resources ({} entries)", m_Resources.size());
            m_Resources.clear();
            m_DefaultId.clear();
        }

        /**
         * Returns the number of resources currently cached.
         */
        [[nodiscard]] auto Count() const -> std::size_t
        {
            return m_Resources.size();
        }

    private:
        const T& GetFallback() const {
            if (!m_Fallback) {
                m_Fallback = std::make_unique<T>();
                if constexpr (std::is_same_v<T, sf::Texture>) {
                    sf::Image image;
                    image.create(16, 16, sf::Color::Magenta);
                    m_Fallback->loadFromImage(image);
                }
            }
            return *m_Fallback;
        }

        /** Hash map storing all loaded resources, keyed by their string ID. */
        std::unordered_map<std::string, std::unique_ptr<T>> m_Resources;

        /** The ID of the first successfully loaded resource (used by GetDefault). */
        std::string m_DefaultId;

        /** Fallback resource used when Get() fails in Release mode. */
        mutable std::unique_ptr<T> m_Fallback;
    };

} // namespace Engine
