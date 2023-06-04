//
// Created by silver on 17/05/23.
//

#ifndef IVY_VOXELS_H
#define IVY_VOXELS_H

#include <cstdint>

namespace Voxel {

/**
 * A voxel is simply a specific color and opacity that represents a cube in space
 */
typedef union {
	struct __attribute__((packed)) {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	uint32_t rgba;

} Voxel;

/**
 * Returns true if the voxel is at least partially transparent.
 */
inline bool is_transparent(const Voxel& voxel)
{
	return voxel.a != 255;
}

/**
 * Returns true if the voxel is at least partially transparent.
 */
inline bool is_visible(const Voxel& voxel)
{
	return voxel.a != 0;
}

/**
 * Equality operator for the voxel struct. Inlined for improved performance since we use it a lot
 * the mesher
 */
inline bool operator==(const Voxel& v1,  const Voxel& v2)
{
	return v1.rgba == v2.rgba;
}

/**
 * Convertion function to int. It is not possibl to use cpp operators since we work on a union.
 * Should have no impact on performance compared to operator int()
 */
inline int to_int(const Voxel& v)
{
	return static_cast<int>(v.rgba);
}

constexpr Voxel air(void)
{
	return Voxel{0, 0, 0, 0};
}

constexpr Voxel grass(void)
{
	return Voxel{26, 153, 26, 255};
}

constexpr Voxel dirt(void)
{
	return Voxel{153, 117, 82, 255};
}

constexpr Voxel stone(void)
{
	return Voxel{135, 140, 140, 255};
}


//#define VOXEL_STONE Voxel(0.53, 0.55, 0.55, 1.0)
//#define VOXEL_DIRT Voxel(0.6, 0.46, 0.32, 1.0)
//#define VOXEL_GRASS Voxel(0.1, 0.6, 0.1, 1.0)
//#define VOXEL_AIR Voxel(0.0, 0.0, 0.0, 0.0)

}

#endif //IVY_VOXELS_H
