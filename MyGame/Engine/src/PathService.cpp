#include "Engine/PathService.hpp"

namespace Engine {
    // Resolves a relative path by appending it to the directory containing the executable.
    std::filesystem::path PathService::Resolve(std::string_view relativePath) {
        return GetExecutableDirectory() / std::filesystem::path(relativePath);
    }
}
