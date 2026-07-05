# Fetch Catch2 for unit testing
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Create the test executable
add_executable(EngineTests
    test_InputManager.cpp
    test_PathService.cpp
    test_DisplaySyncController.cpp
    test_Math.cpp
    test_Transform2D.cpp
)

# Link against the Engine and Catch2
target_link_libraries(EngineTests
    PRIVATE
        Engine
        Catch2::Catch2WithMain
)

# Register tests with CTest
add_test(NAME EngineTests COMMAND EngineTests)
