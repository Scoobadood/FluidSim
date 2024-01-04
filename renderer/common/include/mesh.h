//
// Created by Dave Durbin on 3/1/2024.
//

#ifndef FLUIDSIM_MESH_H
#define FLUIDSIM_MESH_H

#include <vector>
#include <array>
#include "gl_common.h"

class Mesh {
public:
  explicit Mesh(int32_t position_attr_idx = 0,
                int32_t normal_attr_idx = -1,
                int32_t colour_attr_idx = -1,
                int32_t texture_attr_idx = -1);

  ~Mesh();

  struct BoundingBox {
    std::array<float, 3> min_vertex{0,0,0};
    std::array<float, 3> max_vertex{0,0,0};
  };

  void Bind() const;

  inline GLsizei NumElements() const { return num_elements_; }

  inline bool HasNormals() const { return normal_attr_idx_ >= 0; }

  inline bool HasColours() const { return colour_attr_idx_ >= 0; }

  inline bool HasTextureCoords() const { return texture_attr_idx_ >= 0; }

  void SetVertexData(const std::vector<float> &vertex_data);

  void SetIndexData(const std::vector<uint32_t> &indices);

  const BoundingBox &Bounds() const { return bounding_box_; };

  const std::array<float, 3> &CentreOfMass() const { return centre_of_mass_; };

private:
  void UpdateDerivedProperties(const std::vector<float> &vertex_data);

  int32_t position_attr_idx_;
  int32_t normal_attr_idx_;
  int32_t colour_attr_idx_;
  int32_t texture_attr_idx_;
  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLsizei num_elements_;
  BoundingBox bounding_box_;
  std::array<float, 3> centre_of_mass_;
};


#endif //FLUIDSIM_MESH_H
