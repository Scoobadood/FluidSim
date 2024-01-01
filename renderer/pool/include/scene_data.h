//
// Created by Dave Durbin on 1/1/2024.
//

#ifndef FLUIDSIM_SCENE_DATA_H
#define FLUIDSIM_SCENE_DATA_H

// Vertex size is 3 (position)
#include <cstdint>

/*
 * 0+-----------+4
 *  |\          :\
 *  | \         : \
 *  | 1+--------:--+5
 *  |  |        :  |
 * 3+..|........:7 |
 *   \ |           |
 *    \|           |
 *    2+-----------+6
 *
 */
const float CUBE_VERTEX_DATA[] = {
        -1, 1, 1,
        -1, 1, -1,
        -1, -1, -1,
        -1, -1, 1,
        1, 1, 1,
        1, 1, -1,
        1, -1, -1,
        1, -1, 1,
};
const uint32_t CUBE_INDICES[] = {
        0, 1, 2,
        0, 2, 3,
        1, 5, 6,
        1, 6, 2,
        5, 4, 7,
        5, 7, 6,
        4, 0, 3,
        4, 3, 7,
        0, 4, 5,
        0, 5, 1,
        2, 6, 7,
        2, 7, 3
};
#endif //FLUIDSIM_SCENE_DATA_H
