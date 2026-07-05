#ifdef _WIN32
#include "Engine/PathService.hpp"
#include <windows.h>
#include <vector>

namespace Engine {

    void PathService::Initialize(int /*argc*/, char* /*argv*/[]) {
        // Nothing needed on Windows
    }

    std::filesystem::path PathService::GetExecutableDirectory() {
        std::vector<wchar_t> buffer(MAX_PATH);
        DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        while (size == buffer.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer.resize(buffer.size() * 2);
            size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        }
        std::filesystem::path exePath(buffer.data());
        return exePath.parent_path();
    }
}
#endif // _WIN32
