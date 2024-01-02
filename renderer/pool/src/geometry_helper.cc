//
// Created by Dave Durbin on 2/1/2024.
//
#include "geometry_helper.h"
#include <map>
#include <string>

const std::map<std::string, std::pair<std::pair<float, float>, std::pair<float, float>>> TEXTURES
        {
                {"concrete", std::make_pair(std::make_pair(0.f,0.5f),std::make_pair(.5f,1.f))},
                {"tiles", std::make_pair(std::make_pair(0.5f,0.5f),std::make_pair(1.f,1.f))},
                {"water", std::make_pair(std::make_pair(0.6f,0.6f),std::make_pair(.6f,.601))}
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
                               const std::string& texture,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;

  float ll=0,rr=1,tt=0,bb=1;
  if( with_textures_ && !texture.empty()) {
    auto tex_data = TEXTURES.at(texture);
    ll = tex_data.first.first;
    rr = tex_data.second.first;
    tt = tex_data.first.second;
    bb = tex_data.second.second;
  }

  vertex_data.insert(vertex_data.end(), {x, max_y, max_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, tt});
  vertex_data.insert(vertex_data.end(), {x, max_y, min_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, tt});
  vertex_data.insert(vertex_data.end(), {x, min_y, min_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, bb});
  vertex_data.insert(vertex_data.end(), {x, min_y, max_z, normal_x, 0, 0});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, bb});
  BumpIndexData(base_vertex_idx, index_data);
}

void GeometryHelper::AddYPlane(float y, float normal_y,
                               float min_x, float max_x,
                               float min_z, float max_z,
                               float r, float g, float b,
                               const std::string& texture,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  float ll=0,rr=1,tt=0,bb=1;
  if( with_textures_ && !texture.empty()) {
    auto tex_data = TEXTURES.at(texture);
    ll = tex_data.first.first;
    rr = tex_data.second.first;
    tt = tex_data.first.second;
    bb = tex_data.second.second;
  }
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;
  vertex_data.insert(vertex_data.end(), {min_x, y, max_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, tt});
  vertex_data.insert(vertex_data.end(), {max_x, y, max_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, tt});
  vertex_data.insert(vertex_data.end(), {max_x, y, min_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, bb});
  vertex_data.insert(vertex_data.end(), {min_x, y, min_z, 0.0f, normal_y, 0.0f});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, bb});
  BumpIndexData(base_vertex_idx, index_data);
}

void GeometryHelper::AddZPlane(float z, float normal_z,
                               float min_x, float max_x,
                               float min_y, float max_y,
                               float r, float g, float b,
                               const std::string& texture,
                               std::vector<float> &vertex_data,
                               std::vector<uint32_t> &index_data) const {
  float ll=0,rr=1,tt=0,bb=1;
  if( with_textures_ && !texture.empty()) {
    auto tex_data = TEXTURES.at(texture);
    ll = tex_data.first.first;
    rr = tex_data.second.first;
    tt = tex_data.first.second;
    bb = tex_data.second.second;
  }
  auto floats_per_vertex = 6 + (with_textures_ ? 2:0) + (with_colours_ ? 3:0);
  auto base_vertex_idx = vertex_data.size() / floats_per_vertex;
  vertex_data.insert(vertex_data.end(), {min_x, max_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, tt});
  vertex_data.insert(vertex_data.end(), {max_x, max_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, tt});
  vertex_data.insert(vertex_data.end(), {max_x, min_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {rr, bb});
  vertex_data.insert(vertex_data.end(), {min_x, min_y, z, 0.0f, 0.0f, normal_z});
  if (with_colours_) vertex_data.insert(vertex_data.end(), {r, g, b});
  if (with_textures_) vertex_data.insert(vertex_data.end(), {ll, bb});
  BumpIndexData(base_vertex_idx, index_data);

}

void GeometryHelper::AddBlock(float min_x, float max_x,
                              float min_y, float max_y,
                              float min_z, float max_z,
                              float r, float g, float b,
                              const std::string& texture,
                              std::vector<float> &vertex_data,
                              std::vector<uint32_t> &index_data) const {
  // Left face
  AddXPlane(min_x, -1, min_y, max_y, min_z, max_z, r, g, b, texture,vertex_data, index_data);
  // Front face
  AddZPlane(min_z, -1, min_x, max_x, min_y, max_y, r, g, b, texture,vertex_data, index_data);
  // Right face
  AddXPlane(max_x, 1, min_y, max_y, min_z, max_z, r, g, b, texture,vertex_data, index_data);
  // Back face
  AddZPlane(max_z, 1, min_x, max_x, min_y, max_y, r, g, b, texture,vertex_data, index_data);
  // Top face
  AddYPlane(max_y, 1, min_x, max_x, min_z, max_z, r, g, b, texture,vertex_data, index_data);
  // Bottom face
  AddYPlane(min_y, -1, min_x, max_x, min_z, max_z, r, g, b, texture,vertex_data, index_data);
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

      AddBlock(min_x, max_x, min_y, max_y, min_z, max_z, 0.0, 0.3, 0.8, "water", vertex_data, index_data);

      min_x += column_width_;
      height_idx++;
    }
    min_z += column_depth_;
  }

  // Generate the pool surrounds geometry
  // Left wall
  AddBlock(-(total_width / 2.0f) - column_width_, -(total_width / 2.0f),
           0, 2.0f,
           -(total_depth / 2.0f), (total_depth / 2.0f),
           6.0, 6.0, 6.0, "concrete", vertex_data, index_data
  );
  // Back wall
  AddBlock(-(total_width / 2.0f) - column_width_, (total_width / 2.0f) + column_width_,
           0, 2.0f,
           -(total_depth / 2.0f) - column_depth_, -(total_depth / 2.0f),
           6.0, 6.0, 6.0, "concrete", vertex_data, index_data
  );
  // Right wall
  AddBlock((total_width / 2.0f), (total_width / 2.0f) + column_width_,
           0, 2.0f,
           -(total_depth / 2.0f), (total_depth / 2.0f),
           6.0, 6.0, 6.0, "concrete", vertex_data, index_data
  );
  // Base slab
  AddBlock(-(total_width / 2.0f) - column_width_, (total_width / 2.0f) + column_width_,
           -0.2f, 0.0f,
           -(total_depth / 2.0f) - column_depth_, (total_depth / 2.0f),
           6.0, 6.0, 6.0, "tiles", vertex_data, index_data
  );
}

GeometryHelper::MetaData GeometryHelper::ComputeStorageNeeds(const HeightField &hf) const {
  const uint32_t VERTS_PER_CUBE = 6 /* faces */ * 4 /* vertices per face */;
  const uint32_t BYTES_PER_VERTEX = 3 /* x y z */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_NORMAL = 3 /* nx ny nz */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_COLOUR = 3 /* rgb */ * sizeof(float) /* bytes per float */;
  const uint32_t BYTES_PER_TEXTURE_COORD = 2 /* u v */ * sizeof(float) /* bytes per float */;
  const uint32_t INDICES_PER_CUBE = 2 /* triangles per face */ * 6 /* faces */ * 3 /*indices per triangle */;

  MetaData s{};
  s.position_data_size = BYTES_PER_VERTEX;
  s.normal_data_size = with_normals_ ? BYTES_PER_NORMAL : 0;
  s.texture_coord_size = with_textures_ ? BYTES_PER_TEXTURE_COORD : 0;
  s.colour_data_size = with_colours_ ? BYTES_PER_COLOUR: 0;
  s.bytes_per_vertex = s.position_data_size + s.normal_data_size + s.texture_coord_size + s.colour_data_size;

  s.vertex_storage_sz_bytes = VERTS_PER_CUBE * s.bytes_per_vertex * hf.DimZ() * hf.DimX();
  s.index_storage_sz_bytes = (long) (INDICES_PER_CUBE * sizeof(uint32_t) * hf.DimZ() * hf.DimX());
  s.total_elements = INDICES_PER_CUBE * hf.DimZ() * hf.DimX();

  // Add in pool walls.
  s.vertex_storage_sz_bytes += (4 * VERTS_PER_CUBE * s.bytes_per_vertex);
  s.index_storage_sz_bytes += (4 * INDICES_PER_CUBE * sizeof(uint32_t));
  s.water_elements = s.total_elements;
  s.total_elements += INDICES_PER_CUBE * 4;
  return s;
}
