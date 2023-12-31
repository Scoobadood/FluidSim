#ifndef UTAH_ICG_GL_COMMON_H
#define UTAH_ICG_GL_COMMON_H

#ifdef __APPLE__

#include "OpenGL/gl3.h"

#else
#include "GL/glew.h"
#include "GL/glext.h"
#include "GL/gl.h"
#endif

#include "glm/glm.hpp"


#endif //UTAH_ICG_GL_COMMON_H
