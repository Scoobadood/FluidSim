//
// Created by Dave Durbin on 4/1/2024.
//

#include "mesh_helper.h"
#include <vector>

Mesh MeshHelper::Cuboid(float sz_x, float sz_y, float sz_z, float x, float y, float z, float r, float g, float b) {
  Mesh cube_mesh{0, 1, 2, -1};
  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  auto min_x = x - sz_x / 2.0f;
  auto max_x = x + sz_x / 2.0f;
  auto min_y = y - sz_y / 2.0f;
  auto max_y = y + sz_y / 2.0f;
  auto min_z = z - sz_z / 2.0f;
  auto max_z = z + sz_z / 2.0f;

  // L
  vertex_data.insert(vertex_data.end(), {
          min_x, max_y, max_z, -1, 0, 0, r, g, b,
          min_x, max_y, min_z, -1, 0, 0, r, g, b,
          min_x, min_y, min_z, -1, 0, 0, r, g, b,
          min_x, min_y, max_z, -1, 0, 0, r, g, b
  });
  // R
  vertex_data.insert(vertex_data.end(), {
          max_x, max_y, min_z, 1, 0, 0, r, g, b,
          max_x, max_y, max_z, 1, 0, 0, r, g, b,
          max_x, min_y, max_z, 1, 0, 0, r, g, b,
          max_x, min_y, min_z, 1, 0, 0, r, g, b
  });
  // F
  vertex_data.insert(vertex_data.end(), {
          min_x, max_y, min_z, 0, 0, -1, r, g, b,
          max_x, max_y, min_z, 0, 0, -1, r, g, b,
          max_x, min_y, min_z, 0, 0, -1, r, g, b,
          min_x, min_y, min_z, 0, 0, -1, r, g, b
  });
  // B
  vertex_data.insert(vertex_data.end(), {
          max_x, max_y, max_z, 0, 0, 1, r, g, b,
          min_x, max_y, max_z, 0, 0, 1, r, g, b,
          min_x, min_y, max_z, 0, 0, 1, r, g, b,
          max_x, min_y, max_z, 0, 0, 1, r, g, b
  });
  // Bt
  vertex_data.insert(vertex_data.end(), {
          min_x, min_y, min_z, 0, -1, 0, r, g, b,
          max_x, min_y, min_z, 0, -1, 0, r, g, b,
          max_x, min_y, max_z, 0, -1, 0, r, g, b,
          min_x, min_y, max_z, 0, -1, 0, r, g, b
  });
  // T
  vertex_data.insert(vertex_data.end(), {
          min_x, max_y, max_z, 0, 1, 0, r, g, b,
          max_x, max_y, max_z, 0, 1, 0, r, g, b,
          max_x, max_y, min_z, 0, 1, 0, r, g, b,
          min_x, max_y, min_z, 0, 1, 0, r, g, b
  });
  for (auto face_idx = 0; face_idx < 6; ++face_idx) {
    uint32_t base_idx = face_idx * 4;
    index_data.insert(index_data.end(), {base_idx, base_idx + 1, base_idx + 2,
                                         base_idx, base_idx + 2, base_idx + 3});
  }

  cube_mesh.SetIndexData(index_data);
  cube_mesh.SetVertexData(vertex_data);
  return cube_mesh;
}
