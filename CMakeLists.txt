
cmake_minimum_required(VERSION 3.20)

project(TritonAutonomousNavigation
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(triton
    src/main.cpp
    src/Vessel.cpp
)

target_include_directories(triton PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(triton PRIVATE /W4)
else()
    target_compile_options(triton PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
