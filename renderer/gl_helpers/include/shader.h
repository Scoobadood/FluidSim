#ifndef GLHELPERS_SHADER_H
#define GLHELPERS_SHADER_H

#include "gl_common.h"

#include <string>

class Shader {
public:
  // With geometry and source
  Shader(const char *vertex_shader_source[],
         const char *fragment_shader_source[],
         const char *geometry_shader_source[] = nullptr);

  // With geometry and source
  Shader(const char *vertex_shader_source,
         const char *fragment_shader_source,
         const char *geometry_shader_source = nullptr);

  ~Shader();

  static std::shared_ptr<Shader> from_files(const std::string &vertex_shader_file,
                                            const std::string &fragment_shader_file);

  static std::shared_ptr<Shader> from_files(const std::string &vertex_shader_file,
                                            const std::string &fragment_shader_file,
                                            const std::string &geometry_shader_file);


  // @return true if the shader is ready.
  inline bool is_good() const { return is_ready_; }

  // use/activate the shader
  void use() const;

  //
  uint32_t id() const { return id_; }

  // get_attribute_location
  uint32_t get_attribute_location(const std::string &attribute_name) const;

  // utility uniform functions
  void set_uniform(const std::string &name, int32_t value) const;

  void set_uniform(const std::string &name, float value) const;

  void set_uniform(const std::string &name, int32_t count, float * value) const;

  void set_uniform(const std::string &name, float f0, float f1, float f2) const;

  void set_uniform(const std::string &name, int32_t count, const int32_t *v4) const;

  void set_uniform(const std::string &name, bool v0[4]) const;

  void set_uniform(const std::string &name, uint32_t v0, uint32_t v1) const;

  void set_uniform(const std::string &name, const glm::vec2 &vec) const;

  void set_uniform(const std::string &name, const glm::vec3 &vec) const;

  void set_uniform(const std::string &name, const glm::vec4 &vec) const;

  void set_uniform(const std::string &name, const glm::mat4 &mat) const;

  void set_uniform(const std::string &name, int32_t count, const glm::vec3 *vecs) const;

  inline const std::string &get_error() const { return error_msgs_; }

  std::string info() const;

private:
  // The program ID
  uint32_t id_;

  // Flag indicating that the shader is ready to run
  bool is_ready_;

  // Log of any compile or link errors
  std::string error_msgs_;
};

#endif //GLHELPERS_SHADER_H
