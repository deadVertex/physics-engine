cmake_minimum_required (VERSION 3.13)

project (physics-engine LANGUAGES CXX)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory (src)

enable_testing()

add_subdirectory(test)
