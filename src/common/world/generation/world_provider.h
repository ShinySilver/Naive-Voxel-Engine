//
// Created by silverly on 14/09/2021.
//

#ifndef IVY_WORLD_PROVIDER_H
#define IVY_WORLD_PROVIDER_H


class world_provider {
public:
    abstract biome[] &get_biomes_at_chunk(int chunk_x, int chunk_y, int chunk_z, int seed = 0) const = 0;
    chunk &gen_chunk(int chunk_x, int chunk_y, int chunk_z);
};


#endif //IVY_WORLD_PROVIDER_H
