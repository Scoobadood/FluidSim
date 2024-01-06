add_library(particles SHARED
        src/particle_system.cc particles/particle_system.h
        src/particle.cc particles/particle.h
        particles/ode_solver.h src/ode_solver.cc
        particles/force_handlers.h src/force_handlers.cc
        src/particle_factory.cc
)

target_include_directories(particles
        PRIVATE particles
        PUBLIC .
)

target_link_libraries(particles
        PUBLIC
        glm
        spdlog::spdlog
)
