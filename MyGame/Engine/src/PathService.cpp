#include "Engine/PathService.hpp"

namespace Engine {
    std::filesystem::path PathService::Resolve(std::string_view relativePath) {
        return GetExecutableDirectory() / std::filesystem::path(relativePath);
    }
}
