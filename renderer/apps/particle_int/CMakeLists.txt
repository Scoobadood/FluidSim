add_executable(particle_interact
        src/main.cc)

target_include_directories(particle_interact
        PRIVATE include
)

target_link_libraries(particle_interact
        PRIVATE
        particles
        glfw_utils
        common
)