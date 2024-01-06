# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3-stable
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)


add_library(glfw_utils SHARED
        src/window.cc glfw_utils/window.h
)

target_include_directories(glfw_utils
        PRIVATE glfw_utils
        PUBLIC .
)

target_link_libraries(glfw_utils
        PUBLIC
        glfw
        GLHelpers
)