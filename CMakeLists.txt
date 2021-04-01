cmake_minimum_required (VERSION 2.8)
project (physics-engine)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory (src)
