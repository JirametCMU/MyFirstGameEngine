#pragma once
#include <filesystem>
#include <string_view>

namespace Engine {
    class PathService {
    public:
        static void Initialize(int argc, char* argv[]);
        [[nodiscard]] static std::filesystem::path GetExecutableDirectory();
        [[nodiscard]] static std::filesystem::path Resolve(std::string_view relativePath);
    };
}
