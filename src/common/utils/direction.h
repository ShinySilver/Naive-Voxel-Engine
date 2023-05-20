//
// Created by silver on 19/05/23.
//

#ifndef IVY_DIRECTION_H
#define IVY_DIRECTION_H

#include "glm/glm/vec3.hpp"

typedef glm::vec3 Direction;
namespace Directions {
    Direction All[]{Direction(0, 1, 0), Direction(0, -1, 0),
                    Direction(1, 0, 0), Direction(-1, 0, 0),
                    Direction(0, 0, 1), Direction(0, 0, -1)};
};


#endif //IVY_DIRECTION_H
