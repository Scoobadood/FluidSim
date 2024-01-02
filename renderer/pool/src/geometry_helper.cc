//
// Created by Dave Durbin on 2/1/2024.
//
#include "geometry_helper.h"

const uint32_t CUBE_INDEX_OFFSETS[36] = {//
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
};


GeometryHelper::GeometryHelper(float column_width, //
                               float column_height, //
                               bool with_normals,//
                               bool with_colours,//
                               bool with_textures
) //
        : column_width_{column_width}//
        , column_depth_{column_height}//
        , with_normals_{with_normals}//
        , with_colours_{with_colours}//
        , with_textures_{with_textures}//
{}

void BumpIndexData(uint32_t first_vertex_number, std::vector<uint32_t> &index_data) {
  index_data.insert(index_data.end(), {first_vertex_number, first_vertex_number + 1, first_vertex_number + 2});
  index_data.insert(index_data.end(), {first_vertex_number, first_vertex_number + 2, first_vertex_number + 3});

}

void GeometryHelper::AddXPlane(float x, float normal_x,
                               float min_y, float max_y,
                               float min_z, float max_z,
                               float r, float g, float b,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;

  vertex_data.insert(vertex_data.end(), {x, max_y, max_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0, 0});
  vertex_data.insert(vertex_data.end(), {x, max_y, min_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1, 0});
  vertex_data.insert(vertex_data.end(), {x, min_y, min_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1, 1});
  vertex_data.insert(vertex_data.end(), {x, min_y, max_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0, 1});
  BumpIndexData(base_vertex_idx, index_data);
}

void GeometryHelper::AddYPlane(float y, float normal_y,
                               float min_x, float max_x,
                               float min_z, float max_z,
                               float r, float g, float b,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;
  vertex_data.insert(vertex_data.end(), {min_x, y, max_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0.0, 0.0});
  vertex_data.insert(vertex_data.end(), {max_x, y, max_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1.0, 0.0});
  vertex_data.insert(vertex_data.end(), {max_x, y, min_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1.0, 1.0});
  vertex_data.insert(vertex_data.end(), {min_x, y, min_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0.0, 1.0});
  BumpIndexData(base_vertex_idx, index_data);
}

void GeometryHelper::AddZPlane(float z, float normal_z,
                               float min_x, float max_x,
                               float min_y, float max_y,
                               float r, float g, float b,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;
  vertex_data.insert(vertex_data.end(), {min_x, max_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0.0, 0.0});
  vertex_data.insert(vertex_data.end(), {max_x, max_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1.0, 0.0});
  vertex_data.insert(vertex_data.end(), {max_x, min_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {1.0, 1.0});
  vertex_data.insert(vertex_data.end(), {min_x, min_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {0.0, 1.0});
  BumpIndexData(base_vertex_idx, index_data);

}

void GeometryHelper::AddSlab(float min_x, float max_x,
                             float min_y, float max_y,
                             float min_z, float max_z,
                             float r, float g, float b,
                             std::vector<float> &vertex_data,
                             std::vector<uint32_t> &index_data) const {
  // Left face
  AddXPlane(min_x, -1, min_y, max_y, min_z, max_z, r, g, b, vertex_data, index_data);
  // Front face
  AddZPlane(min_z, -1, min_x, max_x, min_y, max_y, r, g, b, vertex_data, index_data);
  // Right face
  AddXPlane(max_x, 1, min_y, max_y, min_z, max_z, r, g, b, vertex_data, index_data);
  // Back face
  AddZPlane(max_z, 1, min_x, max_x, min_y, max_y, r, g, b, vertex_data, index_data);
  // Top face
  AddYPlane(max_y, 1, min_x, max_x, min_z, max_z, r, g, b, vertex_data, index_data);
  // Bottom face
  AddYPlane(min_y, -1, min_x, max_x, min_z, max_z, r, g, b, vertex_data, index_data);
}

void GeometryHelper::GenerateGeometry(const HeightField &hf, std::vector<float> &vertex_data,
                                      std::vector<uint32_t> &index_data) const {

  auto total_width = (float) hf.DimX() * column_width_;
  auto total_depth = (float) hf.DimZ() * column_depth_;
  const auto min_y = 0.0f;

  vertex_data.clear();
  index_data.clear();

  auto height_idx = 0;
  auto heights = hf.Heights();
  auto min_z = -(total_depth / 2.0f);
  for (auto z = 0; z < hf.DimZ(); ++z) {
    auto min_x = -(total_width / 2.0f);
    auto max_z = min_z + column_depth_;
    for (auto x = 0; x < hf.DimX(); ++x) {
      auto max_x = min_x + column_width_;
      auto max_y = heights[height_idx];

      AddSlab(min_x, max_x, min_y, max_y, min_z, max_z, 0.0, 0.3, 0.8, vertex_data, index_data);

      min_x += column_width_;
      height_idx++;
    }
    min_z += column_depth_;
  }

  // Generate the pool surrounds geometry
  // Left wall
  AddSlab(-(total_width / 2.0f) - column_width_, -(total_width / 2.0f),
          0, 2.0f,
          -(total_depth / 2.0f), (total_depth / 2.0f),
          6.0, 6.0, 6.0, vertex_data, index_data
  );
  // Back wall
  AddSlab(-(total_width / 2.0f) - column_width_, (total_width / 2.0f) + column_width_,
          0, 2.0f,
          -(total_depth / 2.0f) - column_depth_, -(total_depth / 2.0f),
          6.0, 6.0, 6.0, vertex_data, index_data
  );
  // Right wall
  AddSlab((total_width / 2.0f), (total_width / 2.0f) + column_width_,
          0, 2.0f,
          -(total_depth / 2.0f), (total_depth / 2.0f),
          6.0, 6.0, 6.0, vertex_data, index_data
  );
  // Base slab
  AddSlab(-(total_width / 2.0f) - column_width_, (total_width / 2.0f) + column_width_,
          -0.2f, 0.0f,
          -(total_depth / 2.0f) - column_depth_, (total_depth / 2.0f),
          6.0, 6.0, 6.0, vertex_data, index_data
  );
}

GeometryHelper::StorageNeeds GeometryHelper::ComputeStorageNeeds(const HeightField &hf) const {
  const uint32_t VERTS_PER_CUBE = 6 /* faces */ * 4 /* vertices per face */;
  const uint32_t BYTES_PER_VERTEX = 3 /* floats */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_NORMAL = 3 /* floats */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_COLOUR = 3 /* rgb */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_TEXTURE_COORD = 2 /* floats */ * sizeof(float) /* bytes per float */;
  const uint32_t INDICES_PER_CUBE = 2 /* triangles per face */ * 6 /* faces */ * 3 /*indices per triangle */;

  StorageNeeds s{};
  s.position_data_size = BYTES_PER_VERTEX;
  s.normal_data_size = with_normals_ ? BYTES_PER_NORMAL : 0;
  s.texture_coord_size = with_textures_ ? BYTES_PER_TEXTURE_COORD : 0;
  s.colour_data_size = with_colours_ ? BYTES_PER_COLOUR: 0;
  s.bytes_per_vertex = s.position_data_size + s.normal_data_size + s.texture_coord_size + s.colour_data_size;

  s.vertex_storage_sz_bytes = VERTS_PER_CUBE * s.bytes_per_vertex * hf.DimZ() * hf.DimX();
  s.index_storage_sz_bytes = (long) (INDICES_PER_CUBE * sizeof(uint32_t) * hf.DimZ() * hf.DimX());
  s.num_elements = INDICES_PER_CUBE * hf.DimZ() * hf.DimX();

  // Add in pool walls.
  s.vertex_storage_sz_bytes += (4 * VERTS_PER_CUBE * s.bytes_per_vertex);
  s.index_storage_sz_bytes += (4 * INDICES_PER_CUBE * sizeof(uint32_t));
  s.num_elements += INDICES_PER_CUBE * 4;
  return s;
}
