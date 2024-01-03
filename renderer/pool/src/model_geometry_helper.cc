#include "model_geometry_helper.h"

#include "happly.h"

void load_model_from_file(const std::string& ply_file_name,
                          bool with_colours,
                          bool with_normals,
                          bool with_texture_coords,
                          std::vector<float>& vertex_data,
                          std::vector<uint32_t>& element_indices){
// Construct the data object by reading from file
  happly::PLYData plyIn(ply_file_name);

// Get mesh-style data from the object
  auto vPos = plyIn.getVertexPositions();
  auto fInd = plyIn.getFaceIndices<size_t>();

  // Get normals
  std::vector<float> nx, ny, nz;
  if( with_normals) {
    nx = plyIn.getElement("vertex").getProperty<float>("nx");
    ny = plyIn.getElement("vertex").getProperty<float>("ny");
    nz = plyIn.getElement("vertex").getProperty<float>("nz");
  }

  for( auto i=0; i<vPos.size(); i++ ) {
    vertex_data.push_back((float)vPos[i][0]);
    vertex_data.push_back((float)vPos[i][1]);
    vertex_data.push_back((float)vPos[i][2]);
    if( with_normals) {
      vertex_data.push_back(nx[i]);
      vertex_data.push_back( ny[i]);
      vertex_data.push_back( nz[i]);
    }
  }

  for( auto i=0; i<fInd.size(); i++ ) {
    element_indices.insert(element_indices.end(), fInd[i].begin(), fInd[i].end());
  }
}
