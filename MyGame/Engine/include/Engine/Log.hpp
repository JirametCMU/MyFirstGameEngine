#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace Engine {
    class Log {
    public:
        static void Init() {
            spdlog::set_pattern("[%T] [%l] %v");
            spdlog::set_level(spdlog::level::trace);
        }
    };
}

// Wrapping macros so that we strip debug logs in Release mode based on SPDLOG_ACTIVE_LEVEL
#define ENGINE_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define ENGINE_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define ENGINE_INFO(...)  SPDLOG_INFO(__VA_ARGS__)
#define ENGINE_WARN(...)  SPDLOG_WARN(__VA_ARGS__)
#define ENGINE_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
