//
// Created by Dave Durbin on 2/1/2024.
//

#ifndef FLUIDSIM_GEOMETRY_HELPER_H
#define FLUIDSIM_GEOMETRY_HELPER_H

#include <vector>
#include "height_field_sim.h"


class GeometryHelper {
public:
  GeometryHelper(float column_width, //
                 float column_height, //
                 bool with_normals = true,//
                 bool with_colours = false,//
                 bool with_textures = false
  );

  struct MetaData {
    long vertex_storage_sz_bytes;
    long index_storage_sz_bytes;
    int32_t position_data_size;
    int32_t normal_data_size;
    int32_t colour_data_size;
    int32_t texture_coord_size;
    int32_t bytes_per_vertex;
    int32_t total_elements;
    int32_t water_elements;
  };

  void GenerateGeometry(const HeightField &hf, std::vector<float> &vertex_data,
                        std::vector<uint32_t> &index_data) const;

  MetaData ComputeStorageNeeds(const HeightField &hf) const;

protected:
  void AddXPlane(float x, float normal_x,
                 float min_y, float max_y,
                 float min_z, float max_z,
                 float r, float g, float b,
                 const std::string& texture,
                 std::vector<float> &vertex_data,
                 std::vector<uint32_t> &index_data) const;

  void AddYPlane(float y, float normal_y,
                 float min_x, float max_x,
                 float min_z, float max_z,
                 float r, float g, float b,
                 const std::string& texture,
                 std::vector<float> &vertex_data,
                 std::vector<uint32_t> &index_data) const;

  void AddZPlane(float z, float normal_z,
                 float min_x, float max_x,
                 float min_y, float max_y,
                 float r, float g, float b,
                 const std::string& texture,
                 std::vector<float> &vertex_data,
                 std::vector<uint32_t> &index_data) const;

  void AddBlock(float min_x, float max_x,
                float min_y, float max_y,
                float min_z, float max_z,
                float r, float g, float b,
                const std::string& texture,
                std::vector<float> &vertex_data,
                std::vector<uint32_t> &index_data) const;

private:
  float column_width_;
  float column_depth_;
  bool with_normals_;
  bool with_colours_;
  bool with_textures_;
};

#endif //FLUIDSIM_GEOMETRY_HELPER_H
