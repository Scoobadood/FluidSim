# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3-stable
)
FetchContent_MakeAvailable(glfw)

add_executable(test_glfw
        src/main.cc include/main.h
        )

target_include_directories(test_glfw
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
)

target_link_libraries(test_glfw
        PRIVATE
        glfw
        Lesson4
        GLEW::GLEW
)
