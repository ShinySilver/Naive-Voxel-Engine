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
         * Initializing chunk loading around the camera
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
         * Stats
         */
        int chunk_unloaded_this_tick = 0, chunk_loaded_this_tick = 0, chunk_loaded = 0;

        /**
         * Render loop!
         */
        LOG_S(1) << "Client ticking!";
        while (!glfwWindowShouldClose(window)) {

            /**
             * Removing the entities waiting in the unloading queue
             */
            Entity *tmp;
            chunk_unloaded_this_tick = 0;
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
                delete(tmp); // TODO: think about chunk serialization.
            }

            /**
             * Adding to the scene entities waiting in the loading queue
             */
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

            /**
             * Handling camera and inputs
             */
            camera::updateControlling(window);
            camera::updateView(window, _projectionMatrix, _viewMatrix);
            _matrix = _projectionMatrix * _viewMatrix;
            glfwPollEvents();

            /**
             * Rendering entities
             */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (auto const entity: loaded_entities) {
                entity->lock();
                entity->fastUpdate();
                entity->draw(_matrix, glm::vec3(100.0, 100.0, 100.0), camera::get_location().position);
                entity->unlock();
            }

            /**
             * If enabled by pressing F3, we are showing some debug info
             * TODO: replace this interface with one made with a nice library, and unlock the mouse with left alt.
             */
            if(info_mode){
                debug_font.bind();
                debug_font.renderText("iVy dev build ", 0.0125, 0.98, 0.4, colors::WHITE);
                debug_font.renderText(__DATE__ ", " __TIME__, 0.0125, 0.95, 0.4, colors::WHITE);

                Location l = camera::get_location();
                std::stringstream ss;
                ss << "X=" << std::fixed << std::setprecision(3) << l.position.x;
                ss << " Y=" << std::fixed << std::setprecision(3) << l.position.y;
                ss << " Z=" << std::fixed << std::setprecision(3) << l.position.z;
                debug_font.renderText(ss.str(),0.0125, 0.91, 0.4, colors::WHITE);

                ss = std::stringstream();
                ss << "RX=" << std::fixed << std::setprecision(3) << l.rotation.x;
                ss << " RY=" << std::fixed << std::setprecision(3) << l.rotation.y;
                ss << " RZ=" << std::fixed << std::setprecision(3) << l.rotation.z;
                debug_font.renderText(ss.str(),0.0125, 0.88, 0.4, colors::WHITE);

                debug_font.renderText("chunks_loaded="+std::to_string(chunk_loaded),
                                      0.0125, 0.84, 0.4, colors::WHITE);
                debug_font.renderText("chunks_loaded_this_tick="+std::to_string(chunk_loaded_this_tick),
                                      0.0125, 0.81, 0.4, colors::WHITE);
                debug_font.renderText("chunks_unloaded_this_tick="+std::to_string(chunk_unloaded_this_tick),
                                      0.0125, 0.78, 0.4, colors::WHITE);
                debug_font.unbind();
            }

            /**
             * Swapping buffers!
             */
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

