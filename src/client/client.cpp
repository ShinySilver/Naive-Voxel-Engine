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
#include "../common/world/world.h"
#include "../common/world/grid.h"
#include "../common/utils/worker.h"
#include "../common/utils/safe_queue.h"

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0), zfar=1e10f;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false;

        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, zfar);
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
            _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, zfar);
            //_textRenderer->setRatio(width, height);
        }

        std::vector<Worker *> workers;
        std::vector<Entity *> loaded_entities;

        SafeQueue<Entity *> preloading_queue;
        SafeQueue<Entity *> loading_queue;
        SafeQueue<Entity *> unloading_queue;
        Entity *chunk_map[VIEW_DISTANCE * 2+1][VIEW_DISTANCE * 2+1];

        // view distance 2, pos 2
        // (-3 -2 -1) 0 1 2 3 4 (5 6 7)
        // modulo 5 (2 3 4) 0 1 2 3 4 (0 1 2)

        void main_worker_tick() {
            glm::vec3 player_pos = grid::pos_to_chunk(camera::get_location());
            Entity *e;
            for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {

                    // Naive "Round" view distance imp. For fun only. We only load in a circle around the camera
                    if(std::pow(dx,2) + std::pow(dz,2) >= std::pow(VIEW_DISTANCE,2))
                        continue;

                    // We get the chunk in the cache corresponding to the given chunk pos
                    e = chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
                    [((INT_MAX/2+(int)player_pos.z + dz)) % (VIEW_DISTANCE * 2+1)];

                    // If there is no chunk in the cache at this place (first few runs only)
                    if (!e) {

                        // Asking the server for the missing chunk, then adding it to preload queue
                        Entity *new_chunk = world::get_cell(player_pos + glm::vec3(dx, 0, dz))[0];
                        chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
                        [((INT_MAX/2+(int)player_pos.z + dz)) % (VIEW_DISTANCE * 2+1)] = new_chunk;
                        preloading_queue.enqueue(new_chunk);

                        // FIXME: get to async gen
                        // TODO: add a state array
                    } else if (grid::pos_to_chunk(e->getLocation()) != player_pos + glm::vec3(dx, 0, dz)) {
                        // If the mismatched chunk is not loaded yet, ignore it. We can't unload a loading chunk
                        if (!e->is_loaded()) {
                            continue;
                        }
                        // TODO: do this check in the unloading queue?

                        // Else, put it in the unload queue
                        unloading_queue.enqueue(e);

                        // In any case, if there is a chunk mismatch, ask the server for the missing chunk
                        Entity *new_chunk = world::get_cell(player_pos + glm::vec3(dx, 0, dz))[0];
                        chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
                        [((INT_MAX/2+(int)player_pos.z + dz)) % (VIEW_DISTANCE * 2+1)] = new_chunk;
                        preloading_queue.enqueue(new_chunk);
                    }
                }
            }

            // if chunk pos the same, skip.
            // Add chunks to the loading queue
            // Add chunks to the unloading queue
            // sleep 50 ms - 20 TPS

            // MEH -> we have to get the position, but it would be easier to keep it in the main thread
            //static bool first_tick = true;
            //if (first_tick) {
            //    std::cout << "Main worker thread added entities into the world!\n";
            //    first_tick = false;
            //    for (auto &entity : world::get_entities()) {
            //        preloading_queue.enqueue(entity);
            //    }
            //} else {
            //    std::cout << "Main worker thread had nothing to add to the world!\n";
            //}
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        void worker_tick() {
            Entity *e = preloading_queue.dequeue();
            if (e) {
                glm::vec3 chunk_pos = grid::pos_to_chunk(e->getLocation());
                std::cout << "Preloading chunk at chunk pos "<<chunk_pos.x<<";"<<chunk_pos.y<<";"<<chunk_pos.z
                <<" and pos "<< e->getLocation().position.x << ";" << e->getLocation().position.y
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
                    std::cout << "Unloading chunk at chunk pos "<<chunk_pos.x<<";"<<chunk_pos.y<<";"<<chunk_pos.z
                              <<" and pos "<< tmp->getLocation().position.x << ";" << tmp->getLocation().position.y
                              << ";" << tmp->getLocation().position.z << "\n";

                    delete tmp; // TODO: think about chunk serialization.
                }
                while (!loading_queue.empty()) {
                    tmp = loading_queue.dequeue();
                    tmp->load();
                    loaded_entities.emplace_back(tmp);

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    std::cout << "Loading chunk at chunk pos "<<chunk_pos.x<<";"<<chunk_pos.y<<";"<<chunk_pos.z
                              <<" and pos "<< tmp->getLocation().position.x << ";" << tmp->getLocation().position.y
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
            for (auto const e : loaded_entities) {
                e->lock();
                e->fastUpdate();
                e->draw(_matrix);
                e->unlock();
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
