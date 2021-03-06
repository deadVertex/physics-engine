# How is this the recommended way to install googletest?
include(FetchContent)
FetchContent_Declare(
  gtest
  QUIET
  URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz)
# configure build of googletest
set(GTEST_FORCE_SHARED_CRT
    ON
    CACHE BOOL "" FORCE)
set(BUILD_GMOCK
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

include_directories(../src)

add_executable(
  unit_tests
  test_vec2.cpp
  ../src/particle_physics_2d.cpp
  test_particle_physics_2d.cpp
  ../src/drawing.cpp
  test_drawing.cpp
  test_mat2.cpp
  ../src/collision_detection.cpp
  test_collision_detection.cpp)

target_link_libraries(unit_tests PRIVATE gtest_main)

# automatic discovery of unit tests
include(GoogleTest)
gtest_discover_tests(unit_tests)
