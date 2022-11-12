//
// Created by silverly on 20/05/2021.
//
#include "client.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <loguru.hpp>

#include "context.h"
#include "camera.h"
#include "../common/world/world.h"
#include "../common/world/grid.h"
#include "../common/utils/worker.h"
#include "../common/utils/safe_queue.h"
#include "client_networking.h"
#include "chunk_loading.h"

#include <sstream>

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0), z_far = 1e10f;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false;

        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, z_far);
        glm::mat4 _viewMatrix, _matrix;

        void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                switch (key) {
                    case GLFW_KEY_F1:
                        wire_mode = !wire_mode;
                        glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);
                        LOG_S(INFO) << (wire_mode ? "Enabling" : "Disabling") << " wire mode";
                        break;
                    case GLFW_KEY_F2:
                        debug_mode = !debug_mode;
                        if (debug_mode) {
                            LOG_S(INFO) << "Disabling OpenGL culling";
                            glDisable(GL_CULL_FACE);
                        } else {
                            LOG_S(INFO) << "Enabling OpenGL culling";
                            glEnable(GL_CULL_FACE);
                        }
                        break;
                    case GLFW_KEY_F11:
                        is_fullscreen = !is_fullscreen;
                        context::setFullscreen(is_fullscreen);
                        break;
                    case GLFW_KEY_ESCAPE:
                        LOG_S(INFO) << "User pressed Escape. Client will now exit";
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
    }

    void tick() {

        /**
         * Creating context
         */
        GLFWwindow *window = nullptr;
        if (!(window = context::init())) ABORT_S() << "Could not init context!";

        /**
         * Initializing chunk loading around the camera
         */
         chunk_loading::init();

        /**
         * Starting workers
         */
        LOG_S(1) << "Client starting its worker threads....";
        workers = std::vector<Worker *>();
        workers.emplace_back(new Worker("main_client_worker", chunk_loading::main_worker_tick));
        for (int i = 0; i < 2; ++i) {
            std::stringstream name;
            name << "client_worker_" << i + 1;
            workers.emplace_back(new Worker(name.str(), chunk_loading::worker_tick));
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
        LOG_S(1) << "Client ticking!";
        while (!glfwWindowShouldClose(window)) {

            /**
             * Adding/removing entities waiting in the queues to the scene
             */
            {
                Entity *tmp;
                while (!chunk_loading::unloading_queue.empty()) {
                    tmp = chunk_loading::unloading_queue.dequeue();
                    tmp->unload();
                    auto it = std::find(loaded_entities.begin(), loaded_entities.end(), tmp);
                    if (it != loaded_entities.end()) { loaded_entities.erase(it); }

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    DLOG_S(4) << "Unloading chunk at chunk pos "
                              << chunk_pos.x << ";"
                              << chunk_pos.y << ";"
                              << chunk_pos.z << " and pos "
                              << tmp->getLocation().position.x << ";"
                              << tmp->getLocation().position.y << ";"
                              << tmp->getLocation().position.z << "";

                    world::unload_cell(tmp);
                    tmp->~Entity();
                    free(tmp); // TODO: think about chunk serialization.
                }
                while (!chunk_loading::loading_queue.empty()) {
                    tmp = chunk_loading::loading_queue.dequeue();
                    tmp->load();
                    loaded_entities.emplace_back(tmp);

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    DLOG_S(4) << "Loading chunk at chunk pos "
                              << chunk_pos.x << ";"
                              << chunk_pos.y << ";"
                              << chunk_pos.z << " and pos "
                              << tmp->getLocation().position.x << ";"
                              << tmp->getLocation().position.y << ";"
                              << tmp->getLocation().position.z << "";
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
                //entity->draw(_matrix);
                //TODO temporary light pos
                entity->draw(_matrix, glm::vec3(100.0, 100.0, 100.0), camera::get_location().position);
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
        LOG_S(1) << "Unlocking preloading queue. Waiting for client workers to stop...";
        for (int i = 0; i < 3; ++i) {
            workers[i]->stop();
        }
        chunk_loading::preloading_queue.unlock_all();
        for (int i = 0; i < 3; ++i) {
            workers[i]->join();
            delete workers[i];
        }
    }
}

