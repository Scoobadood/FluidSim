//
// Created by Dave Durbin on 3/1/2024.
//

#include <stdexcept>
#include "spdlog/spdlog.h"
#include "mesh.h"
#include "gl_error.h"


Mesh::Mesh(int32_t position_attr_idx,
           int32_t normal_attr_idx,
           int32_t colour_attr_idx,
           int32_t texture_attr_idx) //
        : position_attr_idx_{position_attr_idx}//
        , normal_attr_idx_{normal_attr_idx}//
        , colour_attr_idx_{colour_attr_idx}//
        , texture_attr_idx_{texture_attr_idx}//
        , vao_{0}//
        , vbo_{0}//
        , ebo_{0}//
        , num_elements_{0}//
{
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glEnableVertexAttribArray(position_attr_idx_);

  auto position_data_sz = (GLsizei) (3 * sizeof(float)); // x,y,z
  auto normal_data_sz = (GLsizei) (HasNormals() ? (3 * sizeof(float)) : 0); // nX, nY, nZif present
  auto colour_data_sz = (GLsizei) ((HasColours()) ? (3 * sizeof(float)) : 0); // RGB if present
  auto texture_data_sz = (GLsizei) ((HasTextureCoords()) ? (2 * sizeof(float)) : 0); // UV if present
  GLsizei bytes_per_vertex = position_data_sz + normal_data_sz + colour_data_sz + texture_data_sz;

  long offset = 0;
  glVertexAttribPointer(position_attr_idx_, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *) offset);
  offset += position_data_sz;

  if (HasNormals()) {
    glEnableVertexAttribArray(normal_attr_idx_);
    glVertexAttribPointer(normal_attr_idx_, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *) offset);
    offset += normal_data_sz;
  }

  if (HasColours()) {
    glEnableVertexAttribArray(colour_attr_idx_);
    glVertexAttribPointer(colour_attr_idx_, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *) offset);
    offset += colour_data_sz;
  }

  if (HasTextureCoords()) {
    glEnableVertexAttribArray(texture_attr_idx_);
    glVertexAttribPointer(texture_attr_idx_, 2, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *) offset);
  }

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

  CHECK_GL_ERROR("Generate Mesh buffers")
  glBindVertexArray(0);
}

Mesh::~Mesh() {
  spdlog::warn("Goddam muthafucking mesh deallocated - you just lost all the things");
  glBindVertexArray(0);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteVertexArrays(1, &vao_);
}

void Mesh::Bind() const {
  glBindVertexArray(vao_);
  CHECK_GL_ERROR("Bind Vertex Array")

}

Mesh::BoundingBox ComputeBoundingBox(const std::vector<float> &vertex_data,
                                     bool has_normals,
                                     bool has_colours,
                                     bool has_texture_coords) {
  Mesh::BoundingBox b{{
                              std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max()},
                      {
                              std::numeric_limits<float>::lowest(),
                              std::numeric_limits<float>::lowest(),
                              std::numeric_limits<float>::lowest()
                      }
  };
  auto base_idx = 0;
  while (base_idx + 2 < vertex_data.size()) {
    float x = vertex_data[base_idx];
    float y = vertex_data[base_idx + 1];
    float z = vertex_data[base_idx + 2];
    if (x < b.min_vertex[0])b.min_vertex[0] = x;
    if (x > b.max_vertex[0])b.max_vertex[0] = x;
    if (y < b.min_vertex[1])b.min_vertex[1] = y;
    if (y > b.max_vertex[1])b.max_vertex[1] = y;
    if (z < b.min_vertex[2])b.min_vertex[0] = z;
    if (z > b.max_vertex[2])b.max_vertex[0] = z;
    base_idx += 3;
    if (has_normals)base_idx += 3;
    if (has_colours)base_idx += 3;
    if (has_texture_coords)base_idx += 2;
  }
  return b;
}

void Mesh::SetVertexData(const std::vector<float> &vertex_data)  {
  bounding_box_ = ComputeBoundingBox(vertex_data, HasNormals(), HasColours(), HasTextureCoords());
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertex_data.size() * sizeof(float)), vertex_data.data(), GL_DYNAMIC_DRAW);

  CHECK_GL_ERROR("Set vertex data")
  glBindVertexArray(0);
}

void Mesh::SetIndexData(const std::vector<uint32_t> &indices) {
  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (indices.size() * sizeof(uint32_t)), indices.data(),
               GL_DYNAMIC_DRAW);
  num_elements_ = (GLsizei) indices.size();
  CHECK_GL_ERROR("Set index data")
  glBindVertexArray(0);
}