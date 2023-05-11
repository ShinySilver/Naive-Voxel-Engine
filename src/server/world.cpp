//
// Created by silverly on 20/05/2021.
//

#include "generator.h"
#include "world.h"

#define TICK_RANGE 64
#define VIEW_RANGE 128

namespace world {
    namespace {
        static std::vector<Entity *> cells;
    }

    void init() {
        // Currently empty
    }

    void tick() {
        // Currently empty
    }

    std::vector<Entity *> &get_loaded_cells() {
        return cells;
    }

    void unload_cell(const Entity *entity) {
        // Currently doing nothing
    }

    /**
     * @deprecated
     */
    void load_cell(const glm::vec3 &cell_coordinate, Entity *entity) {
        //generator::generate(cell_coordinate, entity);
    }


    /**
     * @deprecated
     */
    void load_cell_with_callback(const glm::vec3 &cell_coordinate, Entity *entity, std::function<void(Entity *cell)> callback) {
        //generator::generate(cell_coordinate, entity);
        callback(entity);
    }

} //namespace world

