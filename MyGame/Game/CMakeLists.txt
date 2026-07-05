add_executable(GameApp)

# Define game sources
target_sources(GameApp
    PRIVATE
        src/main.cpp
        src/MyGameApp.cpp
        src/MainMenuScene.cpp
        src/GameplayScene.cpp
        src/SettingsManager.cpp
    PUBLIC
        include/Game/MyGameApp.hpp
        include/Game/MainMenuScene.hpp
        include/Game/GameplayScene.hpp
        include/Game/GameSettings.hpp
        include/Game/SettingsManager.hpp
        include/Game/SceneId.hpp
)

# Specify include directories for Game headers
target_include_directories(GameApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Fetch nlohmann_json
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Link to the Engine library and json
target_link_libraries(GameApp
    PRIVATE
        Engine
        nlohmann_json::nlohmann_json
)

# Setup runtime working directory so the game can find its Assets folder
# (Set to the output dir so Assets can be resolved correctly)
set_target_properties(GameApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:GameApp>"
)

# Copy openal32.dll to the output directory after building (required by sfml-audio)
if(WIN32)
    add_custom_command(TARGET GameApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll"
            "$<TARGET_FILE_DIR:GameApp>"
        COMMENT "Copying openal32.dll to output directory"
    )
endif()

# Copy Assets directory to the output directory so the executable can find fonts
add_custom_command(TARGET GameApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:GameApp>/Assets"
    COMMENT "Copying Assets directory to output directory"
)

# Enable testing for Game layer
add_subdirectory(tests)

