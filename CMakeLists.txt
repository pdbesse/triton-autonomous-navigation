
# Minimum CMake version required to configure this project.
cmake_minimum_required(VERSION 3.20)

# Project metadata and implementation language.
project(TritonAutonomousNavigation
    VERSION 0.1.0
    LANGUAGES CXX
)

# Require standard C++17 without compiler-specific extensions.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Core simulation library
# ------------------------------------------------------------
#
# Compile vessel movement and navigation logic into one
# reusable static library.
#
# This allows the simulator and both test executables
# to share the same production implementation.
add_library(triton_core
    src/Vessel.cpp
    src/NavigationController.cpp
    src/Route.cpp
    src/Obstacle.cpp
)

# Make public header files available to triton_core and
# every target that links against it.
#
# PUBLIC propagates the include directory to consumers.
target_include_directories(triton_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ------------------------------------------------------------
# Main simulator executable
# ------------------------------------------------------------

# Compile the application's entry point.
add_executable(triton
    src/main.cpp
)

# Link the simulator to the shared core implementation.
# PRIVATE means this dependency is used by this target.
target_link_libraries(triton PRIVATE triton_core)

# ------------------------------------------------------------
# Automated testing
# ------------------------------------------------------------

# Enable CTest integration for the project.
enable_testing()

# Build the Vessel unit test executable.
add_executable(vessel_tests
    tests/VesselTests.cpp
)

target_link_libraries(vessel_tests PRIVATE triton_core)

# Register the executable as a CTest test.
add_test(
    NAME VesselTests
    COMMAND vessel_tests
)

# Build the NavigationController test executable.
add_executable(navigation_tests
    tests/NavigationTests.cpp
)

target_link_libraries(navigation_tests
    PRIVATE triton_core
)

# Register the navigation test executable with CTest.
add_test(
    NAME NavigationTests
    COMMAND navigation_tests
)

# Build and register automated route navigation tests.
add_executable(route_tests
    tests/RouteTests.cpp
)

target_link_libraries(route_tests
    PRIVATE triton_core
)

add_test(
    NAME RouteTests
    COMMAND route_tests
)

# Build the obstacle test executable.
add_executable(obstacle_tests
    tests/ObstacleTests.cpp
)

# Link the tests against the Triton core library.
target_link_libraries(obstacle_tests PRIVATE triton_core)

# Register the executable with CTest.
add_test(NAME ObstacleTests COMMAND obstacle_tests)



# ------------------------------------------------------------
# Compiler diagnostics
# ------------------------------------------------------------
#
# Enable useful warnings in production library code.
#
# MSVC uses /W4.
# Clang and GCC use -Wall, -Wextra, and -Wpedantic.
#
# Warnings help identify suspicious constructs before
# they become runtime bugs.
if(MSVC)
    target_compile_options(triton_core PRIVATE /W4)
else()
    target_compile_options(triton_core PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
