//
// Created by silver on 19/11/22.
//

#ifndef IVY_CHUNK_CACHE_H
#define IVY_CHUNK_CACHE_H

#include <bits/stdc++.h>
#include "glm/glm/vec3.hpp"
#include "../common/utils/positioning.h"
#include "../common/entities/entity_chunk.h"
#include "utils/meshing/mesh.h"
#include "client.h"

#define CACHE_WIDTH (VIEW_DISTANCE * 2 + 1)

namespace chunk_cache {
    typedef struct ChunkCacheEntry {
        // Chunk identifier
        ChunkPos position;

        // Content of this cache cell
        EntityChunk *entity = 0;

        // Plain chunk data
        Chunk *chunk_data = 0;

        // Flags
        bool is_air = false;
        bool is_awaiting_voxels = false;
        bool is_awaiting_mesh = false;
    } ChunkCacheEntry;

    namespace {
        ChunkCacheEntry cache[CACHE_WIDTH * CACHE_WIDTH * CACHE_WIDTH] = {};
    }

    inline EntityChunk *get_chunk(const ChunkPos &pos) {
        return cache[(INT_MAX + (unsigned int)pos.x) % CACHE_WIDTH +
                     CACHE_WIDTH * ((INT_MAX + (unsigned int) pos.y) % CACHE_WIDTH +
                                    CACHE_WIDTH * ((INT_MAX + (unsigned int) pos.z) % CACHE_WIDTH))].entity;
    }

    inline ChunkCacheEntry *get_cache_entry(const ChunkPos &pos) {
        return &cache[(INT_MAX + (unsigned int) pos.x) % CACHE_WIDTH +
                      CACHE_WIDTH * ((INT_MAX + (unsigned int) pos.y) % CACHE_WIDTH +
                                     CACHE_WIDTH * ((INT_MAX + (unsigned int) pos.z) % CACHE_WIDTH))];
    }
}

#endif //IVY_CHUNK_CACHE_H
