add_library(common SHARED
        src/arcball.cc common/arcball.h
        src/mesh.cpp common/mesh.h
        src/model_geometry_helper.cc common/model_geometry_helper.h
        src/mesh_helper.cc
        common/common.h
)

target_include_directories(common
        PRIVATE common
        PUBLIC  .
        )

target_link_libraries(common
        PUBLIC
        ${OpenGL_LIBRARY}
        GLHelpers
        glm
        glfw_utils
        )