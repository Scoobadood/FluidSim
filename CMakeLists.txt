cmake_minimum_required(VERSION 3.5)

project(FluidSim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED
    COMPONENTS
    Widgets
    OpenGLWidgets)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please make sure Qt6 is installed and CMake can locate it.")
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        fluid_grid.cpp
        fluid_grid_widget.h
        fluid_grid_widget.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(FluidSim
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    fluid_grid.h
)

target_link_libraries(FluidSim PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets)

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
