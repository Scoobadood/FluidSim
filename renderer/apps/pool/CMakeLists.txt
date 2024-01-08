add_executable(pool
        src/main.cc
        src/height_field_sim.cc
        src/geometry_helper.cc include/geometry_helper.h
        ../particle_int/src/renderer.cc
)

target_include_directories(pool
        PRIVATE
        include
#        ${glfw3_SOURCE_DIR}/include
)

target_link_libraries(pool
        PRIVATE
        glfw
        GLHelpers
        common
        GLEW::GLEW
)
