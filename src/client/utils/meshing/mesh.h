//
// Created by silver on 19/11/22.
//

#ifndef IVY_MESH_H
#define IVY_MESH_H

#include <vector>
#include "glm/glm/vec3.hpp"
#include "../../../common/world/chunk.h"

typedef struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<Color> colors;
    std::vector<glm::vec3> normals;
} Mesh;

inline bool is_transparent(Color &c) { return c.r == 0 && c.g == 0 && c.b == 0; }

#endif //IVY_MESH_H
