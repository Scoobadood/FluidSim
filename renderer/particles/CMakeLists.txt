add_executable(party
        src/main.cc
        src/particle_system.cc  include/particle_system.h
        src/particle.cc  include/particle.h
        include/ode_solver.h  src/ode_solver.cc
        include/force_handlers.h  src/force_handlers.cc
        src/particle_factory.cc
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
