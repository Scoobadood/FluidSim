add_executable(party
        src/main.cc
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
