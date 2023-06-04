//
// Created by silver on 18/01/23.
//

#ifndef IVY_CHUNK_H
#define IVY_CHUNK_H

#define CHUNK_SIZE (64)
#define CHUNK_DATA_SIZE (64+1)
#define CHUNK_DATA_SIZE_CUBED (CHUNK_DATA_SIZE*CHUNK_DATA_SIZE*CHUNK_DATA_SIZE)

typedef glm::vec3 Color;
typedef Color Voxel;


struct Chunk {
public:
    inline Chunk() : data{} {}
    inline ~Chunk(){}

    inline Voxel *get(int x, int y, int z) { return &data[x + CHUNK_DATA_SIZE * y + CHUNK_DATA_SIZE * CHUNK_DATA_SIZE * z];}

    inline void set(int x, int y, int z, Voxel v) { data[x + CHUNK_DATA_SIZE * y + CHUNK_DATA_SIZE * CHUNK_DATA_SIZE * z] = v;}

    Voxel data[CHUNK_DATA_SIZE_CUBED];

    bool has_full_face(glm::vec3 direction) {
        return false;
    }
};

#endif //IVY_CHUNK_H
