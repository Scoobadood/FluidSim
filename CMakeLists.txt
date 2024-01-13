cmake_minimum_required(VERSION 3.5)

project(FluidSim VERSION 0.1 LANGUAGES CXX C)


list(APPEND CMAKE_PREFIX_PATH "/Users/dave/Qt/6.6.0/macos")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Qt6
find_package(Qt6 REQUIRED
        COMPONENTS
        Widgets
        OpenGLWidgets
        3DCore
        3DExtras
        3DInput
)

if (NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please make sure Qt6 is installed and CMake can locate it.")
endif ()

# ------------------------------------------------------------------------------


include(FetchContent)

# ------------------------------------------------------------------------------
# A Fast C++ logging library.

# Spdlog
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)
include_directories(${spdlog_SOURCE_DIR}/include)
# ------------------------------------------------------------------------------

add_subdirectory(main)
add_subdirectory(renderer)