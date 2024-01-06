add_executable(party
        src/main.cc)

target_link_libraries(party
        PUBLIC
        particles
        glfw_utils
        common)