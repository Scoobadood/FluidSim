add_executable(physx
        src/main.cc
)

target_include_directories(physx
        PRIVATE
        include
)

target_link_libraries(physx
        PRIVATE
        GLHelpers
        common
)
