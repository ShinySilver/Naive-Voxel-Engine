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

#include <sstream>

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0), zfar=1e10f;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false;

        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, zfar);
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
                for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; dy++) {

					// Naive "Round" view distance imp. For fun only. We only load in a circle
					// around the camera
                    if(std::pow(dx,2) + std::pow(dy,2) >= std::pow(VIEW_DISTANCE,2))
                        continue;

                    // We get the chunk in the cache corresponding to the given chunk pos
                    e = chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
								 [((INT_MAX/2+(int)player_pos.y + dy)) % (VIEW_DISTANCE * 2+1)];

                    // If there is no chunk in the cache at this place (first few runs only)
                    if (!e) {

                        // Asking the server for the missing chunk, then adding it to preload queue
                        Entity *new_chunk = world::get_cell(player_pos + glm::vec3(dx, dy, 0))[0];
                        chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
								 [((INT_MAX/2+(int)player_pos.y + dy)) % (VIEW_DISTANCE * 2+1)] 
								 = new_chunk;
                        preloading_queue.enqueue(new_chunk);

                        // FIXME: get to async gen
                        // TODO: add a state array
                    } else if (grid::pos_to_chunk(e->getLocation()) 
							!= player_pos + glm::vec3(dx, dy, 0)) {
						// If the mismatched chunk is not loaded yet, ignore it. We can't unload a
						// loading chunk
                        if (!e->is_loaded()) {
                            continue;
                        }
                        // TODO: do this check in the unloading queue?

                        // Else, put it in the unload queue
                        unloading_queue.enqueue(e);

						// In any case, if there is a chunk mismatch, ask the server for the missing
						// chunk
                        Entity *new_chunk = world::get_cell(player_pos + glm::vec3(dx, dy, 0))[0];
                        chunk_map[((INT_MAX/2+(int)player_pos.x + dx)) % (VIEW_DISTANCE * 2+1)]
								 [((INT_MAX/2+(int)player_pos.y + dy)) % (VIEW_DISTANCE * 2+1)] 
								 = new_chunk;
                        preloading_queue.enqueue(new_chunk);
                    }
                }
            }

            // if chunk pos the same, skip.
            // Add chunks to the loading queue
            // Add chunks to the unloading queue
            // sleep 50 ms - 20 TPS

			// MEH -> we have to get the position, but it would be easier to keep it in the main
			// thread static bool first_tick = true;
            //if (first_tick) {
            //    std::cout << "Main worker thread added entities into the world!";
            //    first_tick = false;
            //    for (auto &entity : world::get_entities()) {
            //        preloading_queue.enqueue(entity);
            //    }
            //} else {
            //    std::cout << "Main worker thread had nothing to add to the world!";
            //}
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        void worker_tick() {
            Entity *e = preloading_queue.dequeue();
            if (e) {
                glm::vec3 chunk_pos = grid::pos_to_chunk(e->getLocation());
                DLOG_S(4) << "Preloading chunk at chunk pos "
						 << chunk_pos.x << ";" 
						 << chunk_pos.y << ";"
						 << chunk_pos.z << " and pos "
						 << e->getLocation().position.x << ";" 
						 << e->getLocation().position.y << ";" 
						 << e->getLocation().position.z << "";
                e->preload();
                loading_queue.enqueue(e);
            }
        }
    }

    void tick() {

        /**
         * Creating context
         */
        GLFWwindow *window = nullptr;
        if (!(window = context::init())) ABORT_S() << "Could not init context!";

        /**
         * Starting workers
         */
        LOG_S(1) << "Client starting its worker threads....";
        workers = std::vector<Worker *>();
        workers.emplace_back(new Worker("main_client_worker", main_worker_tick));
        for (int i = 0; i < 2; ++i) {
			std::stringstream name;
			name << "client_worker_" << i+1;
            workers.emplace_back(new Worker(name.str(), worker_tick));
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
                while (!unloading_queue.empty()) {
                    tmp = unloading_queue.dequeue();
                    tmp->unload();
                    auto it = std::find(loaded_entities.begin(), loaded_entities.end(), tmp);
                    if (it != loaded_entities.end()) { loaded_entities.erase(it); }

                    glm::vec3 chunk_pos = grid::pos_to_chunk(tmp->getLocation());
                    DLOG_S(4) << "Unloading chunk at chunk pos "
							    << chunk_pos.x << ";"
							    << chunk_pos.y <<";"
							    << chunk_pos.z <<" and pos "
							    << tmp->getLocation().position.x << ";" 
							    << tmp->getLocation().position.y << ";" 
							    << tmp->getLocation().position.z << "";

                    delete tmp; // TODO: think about chunk serialization.
                }
                while (!loading_queue.empty()) {
                    tmp = loading_queue.dequeue();
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
            for (auto const e : loaded_entities) {
                e->lock();
                e->fastUpdate();
				//TODO temporary light pos
                e->draw(_matrix, glm::vec3(100.0, 100.0, 100.0), camera::get_location().position);
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
        LOG_S(1) << "Unlocking preloading queue. Waiting for client workers to stop...";
        preloading_queue.unlock_all();
        for (int i = 0; i < 3; ++i) {
            workers[i]->stop();
            workers[i]->join();
            delete workers[i];
        }
    }
}

