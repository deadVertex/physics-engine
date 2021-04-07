add_executable(main main.cpp particle_physics_2d.cpp drawing.cpp
                    collision_detection.cpp)

target_link_libraries(main ${SDL2_LIBRARIES})
