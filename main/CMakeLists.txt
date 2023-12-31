include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

qt_add_executable(FluidSim
        MANUAL_FINALIZATION

        src/main.cpp

        # Ui
        include/fluid_display_widget.h src/fluid_display_widget.cpp
        include/fluid_simulator_thread.h src/fluid_simulator_thread.cpp
        include/main_window.h src/main_window.cpp
        include/control_panel_widget.h src/control_panel_widget.cpp

        # Sims
        include/fluid_simulator.h
        include/fluid_simulator_2d.h src/fluid_simulator_2d.cpp
        include/grid_fluid_simulator.h src/grid_fluid_simulator.cpp
)

target_link_libraries(FluidSim PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        spdlog::spdlog)

set_target_properties(FluidSim PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FluidSim
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(FluidSim)
