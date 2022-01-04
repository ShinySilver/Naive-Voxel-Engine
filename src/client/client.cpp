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
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0);
        int render_distance = 32;
        bool debug_mode = false, wire_mode = false, is_fullscreen = false;

        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, 300.0f);
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
            _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, 300.0f);
            //_textRenderer->setRatio(width, height);
        }

        std::vector<Worker *> workers;
        std::vector<Entity *> loaded_entities;

        SafeQueue<Entity *> preloading_queue;
        SafeQueue<Entity *> loading_queue;
        SafeQueue<Entity *> unloading_queue;



        void main_worker_tick(){

            // if chunk pos the same, skip.
            // Add chunks to the loading queue
            // Add chunks to the unloading queue
            // sleep 50 ms - 20 TPS

            // MEH -> we have to get the position, but it would be easier to keep it in the main thread
            static bool first_tick = true;
            if(first_tick){
                std::cout << "Main worker thread added entities into the world!\n";
                first_tick = false;
                for (auto &entity : world::get_entities()) {
                    entity->preload();
                    loading_queue.enqueue(entity);
                }
            }else{
                std::cout << "Main worker thread had nothing to add to the world!\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }

        void worker_tick(){
            Entity *e = preloading_queue.dequeue();
            if(e){
                e->preload();
                loading_queue.enqueue(e);
            }
        }
    }

    void tick(GLFWwindow *window) {

        /**
         * Loading entities from the world into the vram
         */
        //std::cout << "Preloading entities...\n";
        //for (auto &entity : world::get_entities()) {
        //    entity->load();
        //}

        /**
         * Starting workers
         */
        std::cout << "Client starting its worker threads...." << std::endl;
        workers = std::vector<Worker *>();
        workers.emplace_back(new Worker("main_client_worker", main_worker_tick));
        for(int i = 0; i<2; ++i){
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
                while(!unloading_queue.empty()) {
                    tmp = unloading_queue.dequeue();
                    tmp->unload();
                    auto it = std::find(loaded_entities.begin(), loaded_entities.end(), tmp);
                    if (it != loaded_entities.end()) { loaded_entities.erase(it); }
                    delete tmp; // TODO: think about chunk serialization.
                }
                while(!loading_queue.empty()) {
                    tmp = loading_queue.dequeue();
                    tmp->load();
                    loaded_entities.emplace_back(tmp);
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
            for (auto const e : world::get_entities()) {
                e->lock();
                e->fastUpdate();
                e->draw(_matrix);
                e->unlock();
            }
            glfwSwapBuffers(window);
        }

        std::cout << "Unlocking preloading queue. Waiting for client workers to stop...\n";
        preloading_queue.unlock_all();
        for(int i = 0; i<3; ++i){
            workers[i]->stop();
            workers[i]->join();
            delete workers[i];
        }
    }
}
