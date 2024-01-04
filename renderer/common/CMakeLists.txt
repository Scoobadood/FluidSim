add_library(common SHARED
        src/arcball.cc include/arcball.h
        src/mesh.cpp include/mesh.h
        src/model_geometry_helper.cc include/model_geometry_helper.h
        )

target_include_directories(common
        PUBLIC
        include
        )

target_link_libraries(common
        PUBLIC
        ${OpenGL_LIBRARY}
        GLHelpers
        glm
        glfw_utils
        )