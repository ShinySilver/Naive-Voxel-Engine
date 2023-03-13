//
// Created by silver on 18/01/23.
//

#ifndef IVY_POSITIONING_TCC
#define IVY_POSITIONING_TCC

#include "../world/chunk.h"

inline ChunkPos location_to_chunk_pos(const Location &l) {
    return glm::vec3(int(l.position.x / CHUNK_SIZE),
                     int(l.position.y / CHUNK_SIZE),
                     int(l.position.z / CHUNK_SIZE));
}

inline ChunkPos world_pos_to_chunk_pos(const WorldPos &v) {
    return glm::vec3(int(v.x / CHUNK_SIZE),
                     int(v.y / CHUNK_SIZE),
                     int(v.z / CHUNK_SIZE));
}

inline WorldPos chunk_pos_to_world_pos(const ChunkPos &v) {
    return glm::vec3(int(v.x * CHUNK_SIZE),
                     int(v.y * CHUNK_SIZE),
                     int(v.z * CHUNK_SIZE));
}

#endif //IVY_POSITIONING_TCC
