
#include "basic_mesher.h"
#include "../../../common/world/voxels.h"

#include <tuple>
#include <ranges>
#include <glm/glm/ext.hpp>
#include <loguru.hpp>

constexpr float offset = -CHUNK_SIZE * VOXEL_SIZE / 2.0f;
constexpr float delta = VOXEL_SIZE / 2.0f;

namespace BasicMesher
{
	Mesh* mesh(Chunk& chunk)
	{
		auto mesh = new Mesh;
		for (int x = 0; x < CHUNK_WIDTH; ++x) {
			for (int y = 0; y < CHUNK_WIDTH; ++y) {
				for (int z = 0; z < CHUNK_HEIGHT; ++z) {

					if (*chunk.get(x, y, z) == VOXEL_AIR) {
						continue;
					}

					if ((x == 0) || (*chunk.get(x - 1, y, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z + delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}

					if ((x == CHUNK_WIDTH - 1) || (*chunk.get(x + 1, y, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z + delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}

					if ((y == 0) || (*chunk.get(x, y - 1, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z + delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}

					if ((y == CHUNK_WIDTH - 1) || (*chunk.get(x, y + 1, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z + delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}

					if ((z == 0) || (*chunk.get(x, y, z - 1) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z - delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z - delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}

					if ((z == CHUNK_HEIGHT - 1) || (*chunk.get(x, y, z + 1) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x - delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y - delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x + delta, y + delta, z + delta));
						mesh->vertices.emplace_back(glm::vec3(x - delta, y + delta, z + delta));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(*chunk.get(x, y, z));
						}
					}
				}
			}
		}

//		for (const auto& voxel : chunk.data) {
//			if (voxel != VOXEL_AIR) {
//				mesh->colors.emplace_back(voxel);
//			}
//		}

		mesh->normals.reserve(mesh->vertices.size());
		for (int i = 0; i < mesh->vertices.size(); i += 3) {
			mesh->normals.emplace_back(
					glm::normalize(glm::cross(
						mesh->vertices[i + 2] - mesh->vertices[i + 0],
						mesh->vertices[i + 1] - mesh->vertices[i + 0]
					)));
			mesh->normals.emplace_back(
					glm::normalize(glm::cross(
						mesh->vertices[i + 2] - mesh->vertices[i + 0],
						mesh->vertices[i + 1] - mesh->vertices[i + 0]
					)));
			mesh->normals.emplace_back(
					glm::normalize(glm::cross(
						mesh->vertices[i + 2] - mesh->vertices[i + 0],
						mesh->vertices[i + 1] - mesh->vertices[i + 0]
					)));
		}

		return mesh;
	}
}

