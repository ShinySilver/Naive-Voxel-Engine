//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "client.h"
#include "context.h"
#include "camera.h"
#include "../server/world.h"
#include "../common/world/grid.h"
#include "../common/utils/worker.h"
#include "../common/utils/safe_queue.h"
#include "../common/world/entities/entity_chunk.h"
#include "client_networking.h"

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0), z_far = 1e10f;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false, round_view_distance = false;

        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, z_far);
        glm::mat4 _viewMatrix, _matrix;

        void key_callback(GLFWwindow *window, int key, int scancode, int action,
                          int mods) {
            if (action == GLFW_PRESS) {
                switch (key) {
                    case GLFW_KEY_F1:
                        wire_mode = !wire_mode;
                        glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);
                        std::cout << (wire_mode ? "Enabling" : "Disabling") << " wire mode." << std::endl;
                        break;
                    case GLFW_KEY_F2:
                        debug_mode = !debug_mode;
                        if (debug_mode) {
                            std::cout << "Disabling OpenGL culling." << std::endl;
                            glDisable(GL_CULL_FACE);
                        } else {
                            std::cout << "Enabling OpenGL culling." << std::endl;
                            glEnable(GL_CULL_FACE);
                        }
                        break;
                    case GLFW_KEY_F11:
                        is_fullscreen = !is_fullscreen;
                        context::setFullscreen(is_fullscreen);
                        break;
                    case GLFW_KEY_ESCAPE:
                        std::cout << "User pressed Escape. Client will now exit." << std::endl;
                        glfwSetWindowShouldClose(window, true);
                        break;
                    case GLFW_KEY_LEFT_ALT:
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        break;
                    default:
                        break;
                }
            } else if (action == GLFW_RELEASE) {
                switch (key) {
                    case GLFW_KEY_LEFT_ALT:
                        glfwSetCursorPos(window, context::getWidth() / 2, context::getHeight() / 2);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        break;
                    default:
                        break;
                }

            }
        }

        void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
            win_ratio = float(width) / float(height);
            _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, z_far);
            //_textRenderer->setRatio(width, height);
        }

        std::vector<Worker *> workers;
        std::vector<Entity *> loaded_entities;

        SafeQueue<Entity *> preloading_queue;
        SafeQueue<Entity *> loading_queue;
        SafeQueue<Entity *> unloading_queue;
        Entity *chunk_map[VIEW_DISTANCE * 2 + 1][VIEW_DISTANCE * 2 + 1];

        void main_worker_tick() {
            glm::vec3 player_pos = grid::pos_to_chunk(camera::get_location());
            Entity *e;

            for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {

                    // Naive "Round" view distance imp. For fun only. We only *load* in a circle around the camera
                    if (round_view_distance && std::pow(dx, 2) + std::pow(dz, 2) >= std::pow(VIEW_DISTANCE, 2))
                        continue;

                    // We get the chunk in the cache corresponding to the given chunk pos
                    e = chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) % (VIEW_DISTANCE * 2 + 1)]
                    [((INT_MAX / 2 + (int) player_pos.z + dz)) % (VIEW_DISTANCE * 2 + 1)];

                    // Skipping loading chunks - we are waiting for them to load before unloading 'em
                    if (e == 0 || !e->is_loaded())
                        continue;

                    // If the chunk is out of view distance, mark it for unload & replace it with a new one
                    if (grid::pos_to_chunk(e->getLocation()) != player_pos + glm::vec3(dx, 0, dz)) {
                        unloading_queue.enqueue(e);
                        chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) % (VIEW_DISTANCE * 2 + 1)]
                        [((INT_MAX / 2 + (int) player_pos.z + dz)) % (VIEW_DISTANCE * 2 + 1)] = 0;
                        Entity *new_chunk = (Entity *) malloc(sizeof(EntityChunk));
                        client_networking::load_cell_async(player_pos + glm::vec3(dx, 0, dz), new_chunk,
                                                    [player_pos, dx, dz](Entity *new_chunk) {
                                                        chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) %
                                                                  (VIEW_DISTANCE * 2 + 1)]
                                                        [((INT_MAX / 2 + (int) player_pos.z + dz)) %
                                                         (VIEW_DISTANCE * 2 + 1)] = new_chunk;
                                                        preloading_queue.enqueue(new_chunk);
                                                    });
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        void worker_tick() {
            Entity *e = preloading_queue.dequeue();
            if (e) {
                glm::vec3 chunk_pos = grid::pos_to_chunk(e->getLocation());
                std::cout << "Preloading chunk at chunk pos " << chunk_pos.x << ";" << chunk_pos.y << ";" << chunk_pos.z
                          << " and pos " << e->getLocation().position.x << ";" << e->getLocation().position.y
                          << ";" << e->getLocation().position.z << "\n";
                e->preload();
                loading_queue.enqueue(e);
            }
        }
    }

    int tick() {

        /**
         * Creating context
         */
        GLFWwindow *window = nullptr;
        if (!(window = context::init())) {
            std::cout << "Could not init context!" << std::endl;
            return -1;
        }

        /**
         * Initializing chunk map
         */
        {
            glm::vec3 player_pos = grid::pos_to_chunk(camera::get_location());
            Entity *e;
            for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
                    e = (Entity *) malloc(sizeof(EntityChunk));
                    world::load_cell(player_pos + glm::vec3(dx, 0, dz), e);
                    chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) % (VIEW_DISTANCE * 2 + 1)]
                    [((INT_MAX / 2 + (int) player_pos.z + dz)) % (VIEW_DISTANCE * 2 + 1)] = e;
                    preloading_queue.enqueue(e);
                }
            }
        }

        /**
         * Starting workers
         */
        std::cout << "Client starting its worker threads...." << std::endl;
        workers = std::vector<Worker *>();
        workers.emplace_back(new Worker("main_client_worker", main_worker_tick));
        for (int i = 0; i < 2; ++i) {
            workers.emplace_back(new Worker("secondary_client_worker", worker_tick));
        }

        /**
         * Registering our callbacks into the window
         */
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        /**
         * Render loop!
         */
        std::cout << "Client ticking!" << std::endl;
        while (!glfwWindowShouldClose(window)) {

            /**
             * Adding/removing entities waiting in the queues to the scene
             */
            {
                Entity *tmp;
                while (!unloading_queue.empty()) {
                    tmp = unloading_queue.dequeue();
                    tmp->unload();
                    auto it = std::find(loaded_entities.begin(), loaded_entities.end(), tmp);
                    if (it != loaded_entities.end()) { loaded_entities.erase(it); }

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    std::cout << "Unloading chunk at chunk pos " << chunk_pos.x << ";" << chunk_pos.y << ";"
                              << chunk_pos.z
                              << " and pos " << tmp->getLocation().position.x << ";" << tmp->getLocation().position.y
                              << ";" << tmp->getLocation().position.z << "\n";

                    world::unload_cell(tmp);
                    free(tmp); // TODO: think about chunk serialization.
                }
                while (!loading_queue.empty()) {
                    tmp = loading_queue.dequeue();
                    tmp->load();
                    loaded_entities.emplace_back(tmp);

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    std::cout << "Loading chunk at chunk pos " << chunk_pos.x << ";" << chunk_pos.y << ";"
                              << chunk_pos.z
                              << " and pos " << tmp->getLocation().position.x << ";" << tmp->getLocation().position.y
                              << ";" << tmp->getLocation().position.z << "\n";
                }
            }

            /**
             * Handling camera and inputs
             */
            camera::updateControlling(window);
            camera::updateView(window, _projectionMatrix, _viewMatrix);
            _matrix = _projectionMatrix * _viewMatrix;
            glfwPollEvents();

            /**
             * Rendering
             */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (auto const entity: loaded_entities) {
                entity->lock();
                entity->fastUpdate();
                entity->draw(_matrix);
                entity->unlock();
            }
            glfwSwapBuffers(window);
        }

        /**
         * Closing all opened buffers and destroying context since all of GL ops are above
         */
        context::terminate();

        /**
         * Sync with client workers
         */
        std::cout << "Unlocking preloading queue. Waiting for client workers to stop...\n";
        preloading_queue.unlock_all();
        for (int i = 0; i < 3; ++i) {
            workers[i]->stop();
            workers[i]->join();
            delete workers[i];
        }

        return 0;
    }
}
