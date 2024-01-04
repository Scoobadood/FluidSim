# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3-stable
)
FetchContent_MakeAvailable(glfw)


add_library(glfw_utils SHARED
        src/window.cc include/window.h
)

target_include_directories(glfw_utils
        PUBLIC
        include
)

target_link_libraries(glfw_utils
        PUBLIC
        glfw
        GLHelpers
)