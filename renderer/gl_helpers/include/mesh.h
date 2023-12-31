#ifndef UTAH_ICG_MESH_H
#define UTAH_ICG_MESH_H

#include <string>

bool load_obj(const std::string &obj_file_name,
              uint32_t &vao,
              uint32_t &vbo,
              uint32_t &ebo,
              int32_t &num_elements,
              uint32_t pos_attr,
              bool include_normals = false,
              uint32_t norm_attr = 0,
              bool include_textures = false,
              uint32_t tx_attr = 0
);

#endif //UTAH_ICG_MESH_H
