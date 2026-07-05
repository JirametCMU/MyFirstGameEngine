#ifndef _WIN32
#include "Engine/PathService.hpp"
#include <filesystem>

namespace Engine {

    void PathService::Initialize(int /*argc*/, char* /*argv*/[]) {
        // POSIX specific initialization if needed
    }

    std::filesystem::path PathService::GetExecutableDirectory() {
        // Fallback for non-Windows platforms
        return std::filesystem::current_path();
    }
}
#endif // !_WIN32
