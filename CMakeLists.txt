cmake_minimum_required(VERSION 3.13)

project(physics-engine LANGUAGES CXX)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Enable all compiler warnings for g++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter")

add_subdirectory(src)

# we cannot analyse results without gcov
find_program(GCOV_PATH gcov)
if(NOT GCOV_PATH)
  message(FATAL_ERROR "Code coverage analysis requires gcov!")
endif()

enable_testing()

# TODO: Use --coverage flag for clang support
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
    -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} \
    -fprofile-arcs -ftest-coverage")

add_subdirectory(test)
