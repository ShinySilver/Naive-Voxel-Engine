
#include "basic_mesher.h"
#include "../../../common/world/voxels.h"

#include <tuple>
#include <ranges>
#include <glm/glm/ext.hpp>

constexpr float offset = -CHUNK_SIZE * VOXEL_SIZE / 2.0f;
constexpr float delta = VOXEL_SIZE / 2.0f;
auto average_duration = std::chrono::duration<double> (0);
double count = 0;

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

					const auto x1 = x - delta;
					const auto x2 = x + delta;
					const auto y1 = y - delta;
					const auto y2 = y + delta;
					const auto z1 = z - delta;
					const auto z2 = z + delta;
					const auto voxel = *chunk.get(x, y, z);

					if ((x == 0) || (*chunk.get(x - 1, y, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z2));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}

					if ((x == CHUNK_WIDTH - 1) || (*chunk.get(x + 1, y, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z2));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}

					if ((y == 0) || (*chunk.get(x, y - 1, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z2));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}

					if ((y == CHUNK_WIDTH - 1) || (*chunk.get(x, y + 1, z) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z2));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}

					if ((z == 0) || (*chunk.get(x, y, z - 1) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z1));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z1));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z1));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}

					if ((z == CHUNK_HEIGHT - 1) || (*chunk.get(x, y, z + 1) == VOXEL_AIR))
					{
						mesh->vertices.emplace_back(glm::vec3(x1, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y1, z2));
						mesh->vertices.emplace_back(glm::vec3(x2, y2, z2));
						mesh->vertices.emplace_back(glm::vec3(x1, y2, z2));
						for (int i = 0; i < 6; ++i) {
							mesh->colors.emplace_back(voxel);
						}
					}
				}
			}
		}

		mesh->normals.reserve(mesh->vertices.size());
		for (int i = 0; i < mesh->vertices.size(); i += 3) {
			const auto normal = glm::normalize(glm::cross(
						mesh->vertices[i + 2] - mesh->vertices[i + 0],
						mesh->vertices[i + 1] - mesh->vertices[i + 0]
					));
			mesh->normals.emplace_back(normal);
			mesh->normals.emplace_back(normal);
			mesh->normals.emplace_back(normal);
		}

		return mesh;
	}
}

