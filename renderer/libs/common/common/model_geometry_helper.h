#ifndef FLUIDSIM_MODEL_GEOMETRY_HELPER_H
#define FLUIDSIM_MODEL_GEOMETRY_HELPER_H

#include <vector>
#include <string>

void load_model_from_file(const std::string& ply_file_name,
                     bool with_colours,
                     bool with_normals,
                     bool with_texture_coords,
                     std::vector<float>& vertex_data,
                     std::vector<uint32_t>& element_indices);

#endif //FLUIDSIM_MODEL_GEOMETRY_HELPER_H
