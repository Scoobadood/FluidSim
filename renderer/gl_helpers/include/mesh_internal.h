#ifndef UTAH_ICG_MESH_INTERNAL_H
#define UTAH_ICG_MESH_INTERNAL_H

#include "mesh.h"

#include <string>
#include <vector>
#include <fstream>

bool parse_face_elements(const std::string &face_elem,
                         int32_t *vertex_idx,
                         bool include_normal = false,
                         int32_t *normal_idx = nullptr,
                         bool include_tex_coord = false,
                         int32_t *tex_coord_idx = nullptr
);

bool parse_3f(const std::string &args, float &x, float &y, float &z);

bool parse_2f(const std::string &args, float &x, float &y);

bool parse_face(const std::string &args, int32_t *vertices,
                bool include_normals = false,
                int32_t *normals = nullptr,
                bool include_tex_coords = false,
                int32_t *tex_coords = nullptr);

bool parse_raw_data(std::ifstream &f,
                    std::vector<std::tuple<float, float, float>> &vertices,
                    std::vector<std::vector<std::tuple<int32_t, int32_t, int32_t>>> &faces,
                    bool include_normals = false,
                    std::vector<std::tuple<float, float, float>> *normals = nullptr,
                    bool include_tex_coords = false,
                    std::vector<std::tuple<float, float>> *tex_coords = nullptr
);

#endif //UTAH_ICG_MESH_INTERNAL_H
