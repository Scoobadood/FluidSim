add_executable(particle_interact
        src/main.cc
        include/drag_drop_manager.h
        include/phys_model.h
        include/renderer.h src/renderer.cc
        include/frame_timer.h
        src/frame_timer.cc
        include/input_mgr.h
        src/input_mgr.cc)

target_include_directories(particle_interact
        PRIVATE include
)

target_link_libraries(particle_interact
        PRIVATE
        particles
        glfw_utils
        common
)