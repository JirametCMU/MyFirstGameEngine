#ifndef _WIN32
#include "Engine/PathService.hpp"
#include <filesystem>

namespace Engine {

    // Placeholder initialization for POSIX-compliant systems.
    void PathService::Initialize(int /*argc*/, char* /*argv*/[]) {
        // POSIX specific initialization if needed
    }

    // Fallback implementation for non-Windows platforms. 
    // Uses the current working directory, which requires the binary to be executed from the correct path.
    std::filesystem::path PathService::GetExecutableDirectory() {
        // Fallback for non-Windows platforms
        return std::filesystem::current_path();
    }
}
#endif // !_WIN32
