# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3-stable
)
FetchContent_MakeAvailable(glfw)

add_executable(test_glfw
        src/main.cc
        src/height_field_sim.cc include/scene_data.h
        src/geometry_helper.cc include/geometry_helper.h
        src/renderer.cc include/renderer.h
        src/mesh.cpp include/mesh.h
        src/gl_error.cc  include/gl_error.h
        src/model_geometry_helper.cc
        include/happly.h
        include/model_geometry_helper.h
        include/window.h
        src/window.cc
src/arcball.cc
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
