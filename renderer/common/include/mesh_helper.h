//
// Created by Dave Durbin on 4/1/2024.
//

#ifndef FLUIDSIM_MESH_HELPER_H
#define FLUIDSIM_MESH_HELPER_H

#include "mesh.h"

class MeshHelper {
public:
  static Mesh Cuboid(float sz_x=1.0f, float sz_y=1.0f, float sz_z=1.0f, //
                     float x=0, float y=0, float z=0,//
                     float r=0.7f, float g=0.7f, float b=0.7f);
};


#endif //FLUIDSIM_MESH_HELPER_H
