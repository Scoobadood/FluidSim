find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(GLHelpers
        SHARED
        src/shader.cc include/shader.h
        src/texture.cc include/texture.h
        src/render_target.cc include/render_target.h
        src/mesh.cc include/mesh.h
        src/string_utils.cc include/string_utils.h
        src/gl_enum_map.cc include/gl_enum_map.h
        )

target_include_directories(GLHelpers
        PRIVATE
        ${spdlog_SOURCE_DIR}/include
        PUBLIC
        include
        ${stb_SOURCE_DIR}
)

target_link_libraries(GLHelpers
        PUBLIC
        ${OPENGL_LIBRARIES}
        glm
        )

add_executable(test_obj_loader
        tests/test_obj_parser.cc
        )


target_link_libraries(test_obj_loader
        PRIVATE
        GLHelpers
        gtest
        GLEW::GLEW
        )
