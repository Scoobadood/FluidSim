//
// Created by Dave Durbin on 3/1/2024.
//

#ifndef FLUIDSIM_GL_ERROR_H
#define FLUIDSIM_GL_ERROR_H

#include <stdexcept>
#include "gl_common.h"
#include "spdlog/spdlog.h"


extern GLenum g_glerr;
#define CHECK_GL_ERROR(txt)   \
if ((g_glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, g_glerr); \
throw std::runtime_error("GLR"); \
} \


#endif //FLUIDSIM_GL_ERROR_H
