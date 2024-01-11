add_executable(jos_stam_3d
        src/main.cc
        src/grid_fluid_simulator_3d.cc include/grid_fluid_simulator_3d.h
        include/vectors.h
)

target_include_directories(jos_stam_3d
        PUBLIC include
)

target_link_libraries(jos_stam_3d
        PUBLIC
        particles
        glfw_utils
        common
)