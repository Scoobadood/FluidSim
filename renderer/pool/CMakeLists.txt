add_executable(test_glfw
        src/main.cc
        src/height_field_sim.cc include/scene_data.h
        src/geometry_helper.cc include/geometry_helper.h
        src/renderer.cc include/renderer.h
)

target_include_directories(test_glfw
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
)

target_link_libraries(test_glfw
        PRIVATE
        glfw
        GLHelpers
        common
        GLEW::GLEW
)
