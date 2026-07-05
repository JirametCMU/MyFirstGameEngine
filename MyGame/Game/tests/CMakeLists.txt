add_executable(GameTests
    test_SettingsManager.cpp
    ../src/SettingsManager.cpp
)

target_include_directories(GameTests PRIVATE ../include)

target_link_libraries(GameTests
    PRIVATE
        Engine
        nlohmann_json::nlohmann_json
        Catch2::Catch2WithMain
)

add_test(NAME GameTests COMMAND GameTests)
