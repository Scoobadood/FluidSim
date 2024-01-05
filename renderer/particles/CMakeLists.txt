add_executable(party
        src/main.cc
        src/particle_system.cc  include/particle_system.h
)

target_include_directories(party
        PRIVATE
        include
)

target_link_libraries(party
        PRIVATE
        GLHelpers
        common
)
