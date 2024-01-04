//
// Created by Dave Durbin on 3/1/2024.
//

#ifndef FLUIDSIM_MESH_H
#define FLUIDSIM_MESH_H

#include <vector>
#include "gl_common.h"

class Mesh {
public:
  explicit Mesh(int32_t position_attr_idx = 0,
                int32_t normal_attr_idx = -1,
                int32_t colour_attr_idx = -1,
                int32_t texture_attr_idx = -1);
  ~Mesh();

  struct BoundingBox{
    std::array<float,3> min_vertex;
    std::array<float,3> max_vertex;
  };

  void Bind() const;
  inline GLsizei NumElements() const {return num_elements_;}

  inline bool HasNormals() const { return normal_attr_idx_ >= 0; }

  inline bool HasColours() const { return colour_attr_idx_ >= 0; }

  inline bool HasTextureCoords() const { return texture_attr_idx_ >= 0; }

  void SetVertexData(const std::vector<float> &vertex_data) ;

  void SetIndexData(const std::vector<uint32_t> &indices) ;

  const BoundingBox& Bounds() const { return bounding_box_;};

private:
  int32_t position_attr_idx_;
  int32_t normal_attr_idx_;
  int32_t colour_attr_idx_;
  int32_t texture_attr_idx_;
  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLsizei num_elements_;
  BoundingBox bounding_box_;
};


#endif //FLUIDSIM_MESH_H
