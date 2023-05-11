//
// Created by silver on 18/01/23.
//

#ifndef IVY_CHUNK_H
#define IVY_CHUNK_H

#define CHUNK_SIZE (32)
#define CHUNK_SIZE_CUBED (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

typedef glm::vec3 Color;
typedef Color Voxel;


struct Chunk {
public:
    inline Chunk(Voxel *data) : data{data} {}

    inline Voxel *get(int x, int y, int z) { return &data[x + CHUNK_SIZE * y + CHUNK_SIZE * CHUNK_SIZE * z]; }

    inline void set(int x, int y, int z, Voxel v) { data[x + CHUNK_SIZE * y + CHUNK_SIZE * CHUNK_SIZE * z] = v; }

    inline bool is_empty() { return data; }

    Voxel *data;
};

#endif //IVY_CHUNK_H
