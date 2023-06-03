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
#include "../server/worldgen/world.h"
#include "../common/utils/worker.h"
#include "../common/utils/safe_queue.h"
#include "workers/chunk_loading.h"
#include "utils/colors.h"
#include "utils/shader/text_renderer.h"
#include "../common/utils/stats.h"
#include "ui/debug_screen.h"
#include <sstream>

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0), z_far = 1e10f;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false, info_mode = false;

        TextRenderer *font;

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
                    case GLFW_KEY_F3:
                        info_mode = !info_mode;
                        if (info_mode) {
                            LOG_S(INFO) << "Enabling F3 screen";
                        } else {
                            LOG_S(INFO) << "Disabling F3 screen";
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
            font->setRatio(width, height);
        }

        std::vector<Worker *> workers;
        std::vector<Entity *> loaded_entities;
    }

    void tick() {
        LOG_S(INFO) << "Client starting!";

        /**
         * Creating context
         */
        GLFWwindow *window = nullptr;
        if (!(window = context::init())) ABORT_S() << "Could not init context!";

        /**
         * Some ugly hacks for the text renderer
         */
        TextRenderer debug_font = TextRenderer("resources/fonts/arial.ttf", 1,
                                               1, WIN_WIDTH, WIN_HEIGHT);
        font = &debug_font;

        /**
         * Initializing chunk loading
         */
        chunk_loading::init();

        /**
         * Starting workers
         */
        LOG_S(1) << "Client starting its worker threads....";
        workers = std::vector<Worker *>();
        workers.emplace_back(new Worker("main_client_worker", chunk_loading::main_worker_tick));
        for (int i = 0; i < CLIENT_SECONDARY_WORKER_THREAD_NUMBER; ++i) {
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
         * Clocks for stats
         */
        #if ALLOW_DEBUG_STATS
        clock_t t0, t1;
        #endif

        /**
         * Render loop!
         */
        LOG_S(1) << "Client ticking!";
        while (!glfwWindowShouldClose(window)) {

            #if ALLOW_DEBUG_STATS
            t1 = clock();
            #endif

            /**
             * Adding to the scene entities waiting in the loading queue
             */
            Entity *tmp;
            #if ALLOW_DEBUG_STATS
            t0 = clock();
            stats::chunk_loaded_this_tick = 0;
            #endif
            while (!chunk_loading::loading_queue.empty()) {
                #if ALLOW_DEBUG_STATS
                stats::chunk_loaded++;
                stats::chunk_loaded_this_tick++;
                #endif
                tmp = chunk_loading::loading_queue.dequeue();
                tmp->load();
                loaded_entities.emplace_back(tmp);

                glm::vec3 chunk_pos = location_to_chunk_pos(tmp->getLocation());
                DLOG_S(4) << "Loading chunk at chunk pos "
                          << chunk_pos.x << ";"
                          << chunk_pos.y << ";"
                          << chunk_pos.z << " and pos "
                          << tmp->getLocation().position.x << ";"
                          << tmp->getLocation().position.y << ";"
                          << tmp->getLocation().position.z << "";
            }
            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_loading_entities =
                    stats::avg_time_spent_loading_entities * 0.99 + 0.01 * (clock() - t0);
            #endif

            /**
             * Removing the entities waiting in the unloading queue
             */
            #if ALLOW_DEBUG_STATS
            stats::chunk_unloaded_this_tick = 0;
            t0 = clock();
            #endif
            while (!chunk_loading::unloading_queue.empty()) {
                // Updating statistics
                #if ALLOW_DEBUG_STATS
                stats::chunk_loaded--;
                stats::chunk_unloaded_this_tick++;
                #endif

                // Unloading chunks
                tmp = chunk_loading::unloading_queue.dequeue();
                tmp->unload();
                world::unload_cell(tmp); // Does nothing atm

                // Keeping the entity list up to date. We might want to use a linked hashmap later on.
                auto it = std::find(loaded_entities.begin(), loaded_entities.end(), tmp);
                if (it != loaded_entities.end()) { loaded_entities.erase(it); }

                // Some logging
                glm::vec3 chunk_pos = location_to_chunk_pos(tmp->getLocation());
                DLOG_S(4) << "Unloading chunk at chunk pos "
                          << chunk_pos.x << ";"
                          << chunk_pos.y << ";"
                          << chunk_pos.z << " and pos "
                          << tmp->getLocation().position.x << ";"
                          << tmp->getLocation().position.y << ";"
                          << tmp->getLocation().position.z << "";

                // Actually freeing the chunk
                delete (tmp); // TODO: think about chunk serialization.
            }

            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_unloading_entities =
                    stats::avg_time_spent_unloading_entities * 0.99 + 0.01 * (clock() - t0);
            #endif

            /**
             * Handling camera and inputs
             */
            #if ALLOW_DEBUG_STATS
            t0 = clock();
            #endif

            camera::updateControlling(window);
            glfwPollEvents();

            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_handling_inputs =
                    stats::avg_time_spent_handling_inputs * 0.99 + 0.01 * (clock() - t0);
            t0 = clock();
            #endif

            camera::updateView(window, _projectionMatrix, _viewMatrix);
            _matrix = _projectionMatrix * _viewMatrix;

            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_updating_camera =
                    stats::avg_time_spent_updating_camera * 0.99 + 0.01 * (clock() - t0);
            #endif

            /**
             * Rendering entities
             */
            #if ALLOW_DEBUG_STATS
            t0 = clock();
            #endif

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_waiting_for_gpu =
                    stats::avg_time_spent_waiting_for_gpu * 0.99 + 0.01 * (clock() - t0);
            stats::time_spent_waiting_for_entity_sync_this_tick = 0;
            stats::time_spent_ticking_entities_this_tick = 0;
            stats::time_spent_rendering_entities_this_tick = 0;
            #endif

            for (auto const entity: loaded_entities) {
                #if ALLOW_DEBUG_STATS
                t0 = clock();
                #endif

                entity->lock();

                #if ALLOW_DEBUG_STATS
                stats::time_spent_waiting_for_entity_sync_this_tick += clock() - t0;
                t0 = clock();
                #endif

                entity->fastUpdate();

                #if ALLOW_DEBUG_STATS
                stats::time_spent_ticking_entities_this_tick += clock() - t0;
                t0 = clock();
                #endif

                entity->draw(_matrix, glm::vec3(100.0, 100.0, 100.0), camera::get_location().position);

                #if ALLOW_DEBUG_STATS
                stats::time_spent_rendering_entities_this_tick += clock() - t0;
                #endif

                entity->unlock();
            }

            #if ALLOW_DEBUG_STATS
            stats::avg_time_spent_waiting_for_entity_sync = stats::avg_time_spent_waiting_for_entity_sync * 0.99 +
                                                            0.01 * stats::time_spent_waiting_for_entity_sync_this_tick;
            stats::avg_time_spent_ticking_entities = stats::avg_time_spent_ticking_entities * 0.99
                                                     + 0.01 * stats::time_spent_ticking_entities_this_tick;
            stats::avg_time_spent_rendering_entities = stats::avg_time_spent_rendering_entities * 0.99
                                                       + 0.01 * stats::time_spent_rendering_entities_this_tick;
            #endif
            /**
             * If enabled by pressing F3, we are showing some debug info
             */
            if (info_mode) {
                debug_screen::render(debug_font);
            }

            /**
             * Swapping buffers!
             */
            glfwSwapBuffers(window);
            #if ALLOW_DEBUG_STATS
            stats::avg_frame_duration = stats::avg_frame_duration * 0.99 + 0.01 * (clock() - t1);
            #endif
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

