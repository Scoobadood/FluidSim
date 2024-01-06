#include "shader.h"
#include "gl_common.h"
#include "gl_enum_map.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog-inl.h>

GLuint make_shader(const GLchar *vertex_shader_source[],
                   const GLchar *geometry_shader_source[],
                   const GLchar *fragment_shader_source[],
                   std::string &error_msg);

uint32_t compile_shader(GLenum type, const GLchar *const *source, std::string &compile_errors);


// With geometry
Shader::Shader(const char *vertex_shader_source[],
               const char *fragment_shader_source[],
               const char *geometry_shader_source[]
) {
  is_ready_ = false;
  id_ = make_shader(vertex_shader_source, geometry_shader_source, fragment_shader_source, error_msgs_);
  if (!id_) {
    spdlog::error(error_msgs_);
    return;
  }
  is_ready_ = true;
}

Shader::Shader(const char *vertex_shader_source,
               const char *fragment_shader_source,
               const char *geometry_shader_source
) : Shader(vertex_shader_source ? (const char *[]) {vertex_shader_source} : nullptr,
           fragment_shader_source ? (const char *[]) {fragment_shader_source} : nullptr,
           geometry_shader_source ? (const char *[]) {geometry_shader_source} : nullptr) {
}


Shader::~Shader() {
  glDeleteShader(id_);
}

// use/activate the shader
void Shader::use() const {
  if (is_ready_) {
    glUseProgram(id_);
    return;
  }
  spdlog::error("Shader is not ready to run");
  throw std::runtime_error("Shader is not ready to run");
}

inline GLint get_uniform_loc_with_logging(const char *type, GLuint prog_id, const char *name) {
  auto loc = glGetUniformLocation(prog_id, name);
  if (loc == -1) spdlog::error("Couldn't find uniform {}:{}", name, type);
  return loc;
}

void Shader::set_uniform(const std::string &name, const glm::vec2 &vec) const {
  auto loc = get_uniform_loc_with_logging("vec2", id_, name.c_str());
  glUniform2fv(loc, 1, glm::value_ptr(vec));
}

void Shader::set_uniform(const std::string &name, const glm::vec3 &vec) const {
  auto loc = get_uniform_loc_with_logging("vec3", id_, name.c_str());
  glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void Shader::set_uniform(const std::string &name, const glm::vec4 &vec) const {
  auto loc = get_uniform_loc_with_logging("vec4", id_, name.c_str());
  glUniform4fv(loc, 1, glm::value_ptr(vec));
}

void Shader::set_uniform(const std::string &name, const glm::mat4 &mat) const {
  auto loc = get_uniform_loc_with_logging("matrix4fv", id_, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set_uniform(const std::string &name, float value) const {
  auto loc = get_uniform_loc_with_logging("1f", id_, name.c_str());
  glUniform1f(loc, (GLfloat) value);
}

void Shader::set_uniform(const std::string &name, int value) const {
  auto loc = get_uniform_loc_with_logging("1i", id_, name.c_str());
  glUniform1i(loc, value);
}

void Shader::set_uniform(const std::string &name, int32_t count, float *value) const {
  auto loc = get_uniform_loc_with_logging("1fv", id_, name.c_str());
  glUniform1fv(loc, count, value);
}


void Shader::set_uniform(const std::string &name, int32_t count, const GLint *v4) const {
  auto loc = get_uniform_loc_with_logging("4iv", id_, name.c_str());
  glUniform4iv(loc, count, v4);
}

void Shader::set_uniform(const std::string &name, bool v0[4]) const {
  auto loc = get_uniform_loc_with_logging("4b", id_, name.c_str());
  glUniform4iv(loc, 1, (const GLint *) v0);
}


void Shader::set_uniform(const std::string &name, GLuint v0, GLuint v1) const {
  auto loc = get_uniform_loc_with_logging("2ui", id_, name.c_str());
  glUniform2ui(loc, v0, v1);
}

void Shader::set_uniform(const std::string &name, float f0, float f1, float f2) const {
  auto loc = get_uniform_loc_with_logging("3f", id_, name.c_str());
  glUniform3f(loc, f0, f1, f2);
}

void Shader::set_uniform(const std::string &name, int32_t count, const glm::vec3 *vecs) const {
  auto loc = get_uniform_loc_with_logging("3fv", id_, name.c_str());
  glUniform3fv(loc, count, glm::value_ptr(vecs[0]));
}


/**
 * Compile vertex and fragment shaders and link.
 * Include geometry shader where it's supplied.
 *
 * @param vertex_shader_source
 * @param geometry_shader_source
 * @param fragment_shader_source
 * @return
 */
GLuint make_shader(const GLchar *vertex_shader_source[],
                   const GLchar *geometry_shader_source[],
                   const GLchar *fragment_shader_source[],
                   std::string &error_msg) {
  auto shader_program = glCreateProgram();
  if (!shader_program) {
    spdlog::error("Failed to create program [{}]", glGetError());
    return 0;
  }

  std::string compile_error_msg;
  auto vertex_shader = compile_shader(GL_VERTEX_SHADER,
                                      vertex_shader_source,
                                      compile_error_msg);
  if (!vertex_shader) {
    error_msg = fmt::format("Failed to compile vertex shader\n{}", compile_error_msg);
    glDeleteProgram(shader_program);
    return 0;
  }

  auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
                                        fragment_shader_source,
                                        compile_error_msg);
  if (!fragment_shader) {
    error_msg = fmt::format("Failed to compile fragment shader\n{}", compile_error_msg);
    glDeleteShader(vertex_shader);
    glDeleteProgram(shader_program);
    return 0;
  }

  if (geometry_shader_source) {
    auto geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_shader_source, compile_error_msg);
    if (!geometry_shader) {
      error_msg = fmt::format("Failed to compile geometry shader\n{}", compile_error_msg);
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      glDeleteProgram(shader_program);
      return 0;
    } else {
      glAttachShader(shader_program, geometry_shader);
      glDeleteShader(geometry_shader);
    }
  }

  glAttachShader(shader_program, vertex_shader);
  glDeleteShader(vertex_shader);

  glAttachShader(shader_program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(shader_program);
  int32_t success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

  if (success) {
    error_msg = "";
    return shader_program;
  }

  int32_t log_length;
  glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
  char info_log[log_length];
  glGetProgramInfoLog(shader_program, log_length, nullptr, info_log);
  error_msg = fmt::format("Failed to link shader program\n{}", std::string(info_log));
  glDeleteProgram(shader_program);
  return 0;
}

uint32_t compile_shader(GLenum type, const GLchar *const *source, std::string &compile_errors) {
  auto shader = glCreateShader(type);
  if (!shader) {
    auto msg = fmt::format("Couldn't create shader type {} [{}]", type, glGetError());
    spdlog::error(msg);
    return 0;
  }

  glShaderSource(shader, 1, source, nullptr);
  glCompileShader(shader);

  int32_t success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success) {
    compile_errors = "";
    return shader;
  }

  int32_t log_length;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
  char info_log[log_length];
  glGetShaderInfoLog(shader, log_length, nullptr, info_log);
  compile_errors = std::string(info_log);
  glDeleteShader(shader);
  return 0;
}

uint32_t Shader::get_attribute_location(const std::string &attribute_name) const {
  if (!id_) return -1;
  return glGetAttribLocation(id_, attribute_name.c_str());
}

std::string Shader::info() const {
  using namespace std;

  ostringstream os;
  char buff[1024];
  int32_t count;
  GLenum type;
  GLint size;
  GLsizei length;
  glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &count);
  os << "Attributes" << endl;
  for (auto i = 0; i < count; ++i) {
    glGetActiveAttrib(id_, i, 1024, &length, &size, &type, buff);
    os << "  " << setw(3) << i
       << " " << setw(3) << size
       << " " << setw(12) << name_for_enum(type)
       << " " << buff << endl;
  }
  os << endl << "Uniforms" << endl;
  glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &count);
  for (auto i = 0; i < count; ++i) {
    glGetActiveUniform(id_, i, 1024, &length, &size, &type, buff);
    os << "  " << setw(3) << i
       << " " << setw(3) << size
       << " " << setw(12) << name_for_enum(type)
       << " " << buff << endl;
  }

  return os.str();
}


char *load_file(const std::string &file_name) {
  using namespace std;

  ifstream f{file_name};
  if (!f) return nullptr;

  streamoff file_len;
  f.seekg(0, std::ios::end);
  file_len = f.tellg();
  f.seekg(0, std::ios::beg);

  auto buff = new char[file_len + 1];
  f.read(buff, file_len);
  f.close();
  buff[file_len] = 0;
  return buff;
}

std::shared_ptr<Shader> Shader::from_files(const std::string &vertex_shader_file,
                                           const std::string &fragment_shader_file) {
  return from_files(vertex_shader_file, fragment_shader_file, "");
}

std::shared_ptr<Shader> Shader::from_files(const std::string &vertex_shader_file,
                                           const std::string &fragment_shader_file,
                                           const std::string &geometry_shader_file) {
  auto vs_src = load_file(vertex_shader_file);
  if (!vs_src) {
    spdlog::error("Couldn't load vertex shader : {}", vertex_shader_file);
    return nullptr;
  }
  auto fs_src = load_file(fragment_shader_file);
  if (!fs_src) {
    spdlog::error("Couldn't load fragment shader : {}", fragment_shader_file);
    return nullptr;
  }
  char *gs_src = nullptr;
  if (!geometry_shader_file.empty()) {
    gs_src = load_file(geometry_shader_file);
    if (!gs_src) {
      spdlog::error("Couldn't load geometry shader : {}", geometry_shader_file);
      return nullptr;
    }
  }
  return std::make_shared<Shader>(vs_src, fs_src, gs_src);
}