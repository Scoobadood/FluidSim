set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")


# Write targets to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#
# Deps
#

# Google test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main
)

# GLM
option(GLM_TEST_ENABLE "Disable" OFF)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE
)

# STB For Image libs
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(googletest spdlog glm stb)


## Supress Apple OpenGL messages
IF (APPLE)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    add_definitions(-D__gl_h_)
    add_definitions(-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)

ENDIF (APPLE)

add_subdirectory(libs)
add_subdirectory(apps)