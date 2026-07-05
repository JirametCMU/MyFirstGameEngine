#pragma once
#include <filesystem>
#include <string_view>

namespace Engine {
    /**
     * PathService — Cross-platform asset path resolution.
     * 
     * Ensures that asset loading paths are resolved correctly relative 
     * to the executable's directory, preventing issues when the game 
     * is run from a different working directory.
     */
    class PathService {
    public:
        /** Initializes the service with the program arguments (useful for POSIX). */
        static void Initialize(int argc, char* argv[]);

        /** Retrieves the absolute path to the directory containing the executable. */
        [[nodiscard]] static std::filesystem::path GetExecutableDirectory();

        /** Resolves a relative path against the executable's directory. */
        [[nodiscard]] static std::filesystem::path Resolve(std::string_view relativePath);
    };
}
