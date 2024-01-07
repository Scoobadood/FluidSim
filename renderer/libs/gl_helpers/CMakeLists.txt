find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(GLHelpers
        SHARED
        src/shader.cc GLHelpers/shader.h
        src/texture.cc GLHelpers/texture.h
        src/render_target.cc GLHelpers/render_target.h
        src/string_utils.cc GLHelpers/string_utils.h
        src/gl_enum_map.cc GLHelpers/gl_enum_map.h
        src/gl_error.cc GLHelpers/gl_error.h
        src/stbi_impl.cc
)

target_include_directories(GLHelpers
        PRIVATE
        GLHelpers
        PUBLIC
        ${stb_SOURCE_DIR}
        .
)

target_link_libraries(GLHelpers
        PUBLIC
        spdlog::spdlog
        ${OPENGL_LIBRARIES}
        glm
        )
