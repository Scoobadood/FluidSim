#include "gl_enum_map.h"

#include <spdlog/spdlog-inl.h>

std::string name_for_enum(GLenum type) {
  switch (type) {
    case GL_BYTE:
      return "GL_BYTE";
    case GL_UNSIGNED_BYTE:
      return "GL_UNSIGNED_BYTE";
    case GL_SHORT:
      return "GL_SHORT";
    case GL_UNSIGNED_SHORT:
      return "GL_UNSIGNED_SHORT";
    case GL_INT:
      return "GL_INT";
    case GL_UNSIGNED_INT:
      return "GL_UNSIGNED_INT";
    case GL_FLOAT:
      return "GL_FLOAT";
    case GL_DOUBLE:
      return "GL_DOUBLE";
    case GL_FLOAT_VEC2:
      return "GL_FLOAT_VEC2";
    case GL_FLOAT_VEC3:
      return "GL_FLOAT_VEC3";
    case GL_FLOAT_VEC4:
      return "GL_FLOAT_VEC4";
    case GL_INT_VEC2:
      return "GL_INT_VEC2";
    case GL_INT_VEC3:
      return "GL_INT_VEC3";
    case GL_INT_VEC4:
      return "GL_INT_VEC4";
    case GL_BOOL:
      return "GL_BOOL";
    case GL_BOOL_VEC2:
      return "GL_BOOL_VEC2";
    case GL_BOOL_VEC3:
      return "GL_BOOL_VEC3";
    case GL_BOOL_VEC4:
      return "GL_BOOL_VEC4";
    case GL_FLOAT_MAT2:
      return "GL_FLOAT_MAT2";
    case GL_FLOAT_MAT3:
      return "GL_FLOAT_MAT3";
    case GL_FLOAT_MAT4:
      return "GL_FLOAT_MAT4";
    case GL_SAMPLER_1D:
      return "GL_SAMPLER_1D";
    case GL_SAMPLER_2D:
      return "GL_SAMPLER_2D";
    case GL_SAMPLER_3D:
      return "GL_SAMPLER_3D";
    default:
      return fmt::format("UNKNOWN ({:04x})", type);
  }
}
