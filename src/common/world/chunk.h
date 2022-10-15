//
// Created by silverly on 31/10/2021.
//

#ifndef IVY_CHUNK_H
#define IVY_CHUNK_H

#define DEFAULT_CHUNK_SIDE 16

#include <complex>
#include <glm/glm/vec3.hpp>
#include <vector>

typedef glm::vec3 Color;

typedef struct VoxelFace {
    bool transparent; // TODO: replace voxelface with vec4 colors
    int side;
    Color color;

    VoxelFace(bool transparent, int side, Color color) : transparent(transparent),
                                                         side(side), color(color) {
    }

    inline bool equals(VoxelFace face) {
        return face.transparent == this->transparent
               && face.color == this->color;
    }

    inline bool equals(VoxelFace *face) {
        return face->transparent == this->transparent
               && face->color == this->color;
    }
} VoxelFace;

typedef struct Voxel {
    Color color;
    VoxelFace uniformFace;

    Voxel(Color color) :
            color(color), uniformFace{false, 0, color} {
    }

    Voxel() :
            color{0, 128, 0}, uniformFace{true, 0, {0, 128, 0}} {
    }

    inline bool isEmpty() {
        return color.r == 0 && color.g == 0 && color.b == 0;
    }
} Voxel;

class Chunk {
public:
    inline Chunk(int size = DEFAULT_CHUNK_SIDE, bool fill = false) {
        if (fill) {
            data = std::vector<Voxel>();
            data.reserve(std::pow(size, 3));
        } else {
            data = std::vector<Voxel>(std::pow(size, 3));
        }
        this->size = size;
    }

    inline Chunk(std::vector<Voxel> data, int size) {
        this->data = data;
        this->size = size;
    }

    inline Voxel &get(int x, int y, int z) {
        return data[x + size * y + size * size * z];
    }

private:
    std::vector<Voxel> data;
    short size;                //side size (the chunk is cubic)
};


#endif //IVY_CHUNK_H
