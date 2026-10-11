
cmake_minimum_required(VERSION 3.20)

project(TritonAutonomousNavigation
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared vessel simulation library
add_library(triton_core
    src/Vessel.cpp
    src/NavigationController.cpp
)

target_include_directories(triton_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Main simulator
add_executable(triton
    src/main.cpp
)

target_link_libraries(triton PRIVATE triton_core)

# Unit tests
enable_testing()

add_executable(vessel_tests
    tests/VesselTests.cpp
)

target_link_libraries(vessel_tests PRIVATE triton_core)

add_test(
    NAME VesselTests
    COMMAND vessel_tests
)

# Compiler warnings
if(MSVC)
    target_compile_options(triton_core PRIVATE /W4)
else()
    target_compile_options(triton_core PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
