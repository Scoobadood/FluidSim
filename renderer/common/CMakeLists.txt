add_library(common SHARED
        object.cc include/object.h
        )

target_include_directories(common
        PUBLIC
        include
        )

target_link_libraries(common
        PUBLIC
        ${OpenGL_LIBRARY}
        GLHelpers
        )