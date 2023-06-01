#ifndef BASIC_MESHER_H_
#define BASIC_MESHER_H_

#include "mesh.h"


#define VOXEL_SIZE (1.0f)
#define CHUNK_WIDTH  CHUNK_SIZE
#define CHUNK_HEIGHT CHUNK_SIZE


namespace BasicMesher
{
    Mesh* mesh(Chunk& chunk);
}

#endif // BASIC_MESHER_H_
