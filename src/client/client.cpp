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
#include "../server/world.h"
#include "../common/utils/worker.h"
#include "../common/utils/safe_queue.h"
#include "client_networking.h"
#include "chunk_loading.h"
#include "utils/colors.h"
#include "utils/shader/text_renderer.h"

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

        std::vector<std::unique_ptr<Worker>> workers;
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
         * Initializing chunk loading around the camera
         */
        chunk_loading::init();

        /**
         * Starting workers
         */
        LOG_S(1) << "Client starting its worker threads....";
        workers.emplace_back(std::make_unique<Worker>("main_client_worker", chunk_loading::main_worker_tick));
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
         * Stats
         */
        int chunk_unloaded_this_tick, chunk_loaded_this_tick, chunk_loaded = 0;
        double avg_time_spent_loading_entities = 0, avg_time_spent_unloading_entities = 0,
                avg_time_spent_rendering_entities = 0, time_spent_rendering_entities_this_tick,
                avg_time_spent_ticking_entities = 0, time_spent_ticking_entities_this_tick,
                avg_time_spent_waiting_for_entity_sync = 0, time_spent_waiting_for_entity_sync_this_tick;
        double avg_time_spent_handling_inputs = 0, avg_time_spent_rendering_ui = 0, avg_time_spent_updating_camera = 0;
        double avg_frame_duration = 0, avg_time_spent_waiting_for_gpu = 0;
        clock_t t0, t1 = clock();

        /**
         * Render loop!
         */
        LOG_S(1) << "Client ticking!";
        while (!glfwWindowShouldClose(window)) {

            t1 = clock();

            /**
             * Adding to the scene entities waiting in the loading queue
             */
            Entity *tmp;
            t0 = clock();
            chunk_loaded_this_tick = 0;
            while (!chunk_loading::loading_queue.empty()) {
                chunk_loaded++;
                chunk_loaded_this_tick++;
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
            avg_time_spent_loading_entities = avg_time_spent_loading_entities * 0.99 + 0.01 * (clock() - t0);

            /**
             * Removing the entities waiting in the unloading queue
             */
            chunk_unloaded_this_tick = 0;
            t0 = clock();
            while (!chunk_loading::unloading_queue.empty()) {
                // Updating statistics
                chunk_loaded--;
                chunk_unloaded_this_tick++;

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
            avg_time_spent_unloading_entities = avg_time_spent_unloading_entities * 0.99 + 0.01 * (clock() - t0);

            /**
             * Handling camera and inputs
             */
            t0 = clock();
            camera::updateControlling(window);
            glfwPollEvents();
            avg_time_spent_handling_inputs = avg_time_spent_handling_inputs * 0.99 + 0.01 * (clock() - t0);

            t0 = clock();
            camera::updateView(window, _projectionMatrix, _viewMatrix);
            _matrix = _projectionMatrix * _viewMatrix;
            avg_time_spent_updating_camera = avg_time_spent_updating_camera * 0.99 + 0.01 * (clock() - t0);

            /**
             * Rendering entities
             */
            t0 = clock();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            avg_time_spent_waiting_for_gpu = avg_time_spent_waiting_for_gpu * 0.99 + 0.01 * (clock() - t0);

            time_spent_waiting_for_entity_sync_this_tick = 0;
            time_spent_ticking_entities_this_tick = 0;
            time_spent_rendering_entities_this_tick = 0;
            for (auto const entity: loaded_entities) {
                t0 = clock();
                entity->lock();
                time_spent_waiting_for_entity_sync_this_tick += clock() - t0;

                t0 = clock();
                entity->fastUpdate();
                time_spent_ticking_entities_this_tick += clock() - t0;

                t0 = clock();
                entity->draw(_matrix, glm::vec3(100.0, 100.0, 100.0), camera::get_location().position);
                time_spent_rendering_entities_this_tick += clock() - t0;

                entity->unlock();
            }
            avg_time_spent_waiting_for_entity_sync = avg_time_spent_waiting_for_entity_sync * 0.99
                                                     + 0.01 * time_spent_waiting_for_entity_sync_this_tick;
            avg_time_spent_ticking_entities = avg_time_spent_ticking_entities * 0.99
                                              + 0.01 * time_spent_ticking_entities_this_tick;
            avg_time_spent_rendering_entities = avg_time_spent_rendering_entities * 0.99
                                                + 0.01 * time_spent_rendering_entities_this_tick;

            /**
             * If enabled by pressing F3, we are showing some debug info
             * TODO: replace this interface with one made with a nice library, and unlock the mouse with left alt.
             */
            if (info_mode) {
                t0 = clock();
                debug_font.bind();
                debug_font.renderText("iVy dev build ", 0.0125, 0.98, 0.4, colors::PINK);
                debug_font.renderText(__DATE__ ", " __TIME__, 0.0125, 0.95, 0.4, colors::PINK);

                Location l = camera::get_location();
                std::stringstream ss;
                ss << "X=" << std::fixed << std::setprecision(3) << l.position.x;
                ss << " Y=" << std::fixed << std::setprecision(3) << l.position.y;
                ss << " Z=" << std::fixed << std::setprecision(3) << l.position.z;
                debug_font.renderText(ss.str(), 0.0125, 0.91, 0.4, colors::WHITE);

                ss = std::stringstream();
                ss << "RX=" << std::fixed << std::setprecision(3) << l.rotation.x;
                ss << " RY=" << std::fixed << std::setprecision(3) << l.rotation.y;
                ss << " RZ=" << std::fixed << std::setprecision(3) << l.rotation.z;
                debug_font.renderText(ss.str(), 0.0125, 0.88, 0.4, colors::WHITE);

                debug_font.renderText("chunks_loaded=" + std::to_string(chunk_loaded),
                                      0.0125, 0.84, 0.4, colors::WHITE);
                debug_font.renderText("chunks_loaded_this_tick=" + std::to_string(chunk_loaded_this_tick),
                                      0.0125, 0.81, 0.4, colors::WHITE);
                debug_font.renderText("chunks_unloaded_this_tick=" + std::to_string(chunk_unloaded_this_tick),
                                      0.0125, 0.78, 0.4, colors::WHITE);

                debug_font.renderText(
                        "avg_time_spent_loading_entities=" +
                        std::to_string(int(round(avg_time_spent_loading_entities / CLOCKS_PER_SEC * 1000000))) + " us",
                        0.0125, 0.74, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_time_spent_unloading_entities=" +
                        std::to_string(int(round(avg_time_spent_unloading_entities / CLOCKS_PER_SEC * 1000000))) +
                        " us",
                        0.0125, 0.71, 0.4, colors::WHITE);
                debug_font.renderText("avg_time_spent_waiting_for_entity_sync=" +
                                      std::to_string(int(round(
                                              avg_time_spent_waiting_for_entity_sync / CLOCKS_PER_SEC * 1000000))) +
                                      " us",
                                      0.0125, 0.68, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_time_spent_handling_inputs=" +
                        std::to_string(int(round(avg_time_spent_handling_inputs / CLOCKS_PER_SEC * 1000000))) + " us",
                        0.0125, 0.65, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_time_spent_updating_camera=" +
                        std::to_string(int(round(avg_time_spent_updating_camera / CLOCKS_PER_SEC * 1000000))) + " us",
                        0.0125, 0.62, 0.4, colors::WHITE);

                debug_font.renderText("avg_time_spent_rendering_ui=" +
                                      std::to_string(int(round(avg_time_spent_rendering_ui / CLOCKS_PER_SEC * 1000))) +
                                      " ms",
                                      0.0125, 0.58, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_time_spent_rendering_entities=" +
                        std::to_string(int(round(avg_time_spent_rendering_entities / CLOCKS_PER_SEC * 1000))) + " ms",
                        0.0125, 0.55, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_time_spent_ticking_entities=" +
                        std::to_string(int(round(avg_time_spent_ticking_entities / CLOCKS_PER_SEC * 1000))) + " ms",
                        0.0125, 0.52, 0.4, colors::WHITE);

                debug_font.renderText(
                        "avg_time_spent_waiting_for_gpu=" +
                        std::to_string(int(round(avg_time_spent_waiting_for_gpu / CLOCKS_PER_SEC * 1000))) + " ms",
                        0.0125, 0.49, 0.4, colors::WHITE);
                debug_font.renderText(
                        "avg_total_frame_duration=" +
                        std::to_string(int(round(avg_frame_duration / CLOCKS_PER_SEC * 1000))) + " ms / "
                        +std::to_string(int(round(1/(avg_frame_duration / CLOCKS_PER_SEC))))+" FPS",
                        0.0125, 0.46, 0.4, colors::PINK);

                debug_font.renderText(
                        "loading_queue_size=" +
                        std::to_string(chunk_loading::loading_queue.size()) + " entities",
                        0.0125, 0.42, 0.4, colors::WHITE);
                debug_font.renderText(
                        "unloading_queue_size=" +
                        std::to_string(chunk_loading::unloading_queue.size()) + " entities",
                        0.0125, 0.39, 0.4, colors::WHITE);

                debug_font.renderText(
                        "preloading_queue_size=" +
                        std::to_string(chunk_loading::preloading_queue.size()) + " cache entries",
                        0.0125, 0.35, 0.4, colors::WHITE);
                debug_font.renderText(
                        "cascading_loading_queue_size=" +
                        std::to_string(chunk_loading::cascading_loading_queue.size()) + " cache entries",
                        0.0125, 0.32, 0.4, colors::WHITE);
                debug_font.unbind();
                avg_time_spent_rendering_ui = avg_time_spent_rendering_ui * 0.99 + 0.01 * (clock() - t0);
            }

            /**
             * Swapping buffers!
             */
            glfwSwapBuffers(window);
            avg_frame_duration = avg_frame_duration * 0.99 + 0.01 * (clock() - t1);
        }

        /**
         * Closing all opened buffers and destroying context since all of GL ops are above
         */
        context::terminate();

        /**
         * Sync with client workers
         */
        LOG_S(1) << "Unlocking preloading queue. Waiting for client workers to stop...";
        for (auto& worker : workers) {
            worker->stop();
        }
        chunk_loading::preloading_queue.unlock_all();
        for (auto& worker : workers) {
            worker->join();
        }
    }
}

