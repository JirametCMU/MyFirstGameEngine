add_library(Engine STATIC)

# Define sources and headers
target_sources(Engine
    PRIVATE
        src/Engine.cpp
        src/InputManager.cpp
        src/FPSCounter.cpp
        src/SceneManager.cpp
        src/DisplaySyncController.cpp
        src/PathService.cpp
    PUBLIC
        include/Engine/Engine.hpp
        include/Engine/InputManager.hpp
        include/Engine/Math.hpp
        include/Engine/Transform2D.hpp
        include/Engine/ResourceManager.hpp
        include/Engine/ResourceTypes.hpp
        include/Engine/FPSCounter.hpp
        include/Engine/Scene.hpp
        include/Engine/SceneManager.hpp
        include/Engine/DisplaySyncController.hpp
        include/Engine/PathService.hpp
        include/Engine/Log.hpp
)

if(WIN32)
    target_sources(Engine PRIVATE src/Platform/PathService_Win32.cpp)
else()
    target_sources(Engine PRIVATE src/Platform/PathService_POSIX.cpp)
endif()

# Fetch spdlog
include(FetchContent)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

# Specify include directories
target_include_directories(Engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Common compiler warnings for high quality code
if(MSVC)
    target_compile_options(Engine PRIVATE /W4 /WX)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

# Set spdlog active level definition
target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN>
)
target_compile_definitions(Engine PUBLIC
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
)

# Link libraries
target_link_libraries(Engine
    PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        spdlog::spdlog
)

# Enable testing
enable_testing()
add_subdirectory(tests)

