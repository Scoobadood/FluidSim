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
                 bool with_textures = false
  );

  struct StorageNeeds {
    long vertex_storage_sz_bytes;
    long index_storage_sz_bytes;
    int32_t position_data_size;
    int32_t normal_data_size;
    int32_t texture_coord_size;
    int32_t bytes_per_vertex;
    int32_t num_elements;
  };

  void GenerateGeometry(const HeightField &hf, std::vector<float> &vertex_data,
                        std::vector<uint32_t> &index_data) const;

  StorageNeeds ComputeStorageNeeds(const HeightField &hf) const;

protected:
  static void AddSlab(float min_x, float max_x,
               float min_y, float max_y,
               float min_z, float max_z,
               float r, float g, float b,
               std::vector<float> &vertex_data,
               std::vector<uint32_t> &index_data) ;

private:
  float column_width_;
  float column_depth_;
  bool with_normals_;
  bool with_textures_;
};

#endif //FLUIDSIM_GEOMETRY_HELPER_H
