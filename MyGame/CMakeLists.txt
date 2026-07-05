cmake_minimum_required(VERSION 3.20)
project(MyGame VERSION 1.0.0 LANGUAGES CXX)

# Require C++20 for modern features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable link-time optimization (LTO) in Release mode for maximum performance
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    message(STATUS "IPO/LTO is supported and enabled for Release configuration.")
else()
    message(STATUS "IPO/LTO is not supported: ${ipo_output}")
endif()

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Fetch SFML 2.6.1
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# Disable SFML network module and build static libraries to keep it simple
set(SFML_BUILD_NETWORK OFF CACHE INTERNAL "")
set(SFML_BUILD_AUDIO ON CACHE INTERNAL "")
set(SFML_BUILD_GRAPHICS ON CACHE INTERNAL "")
set(SFML_BUILD_WINDOW ON CACHE INTERNAL "")
set(SFML_BUILD_SYSTEM ON CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)

# Add Engine and Game subdirectories
add_subdirectory(Engine)
add_subdirectory(Game)

