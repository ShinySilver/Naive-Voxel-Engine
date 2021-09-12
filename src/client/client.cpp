//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "client.h"
#include "../common/world.h"
#include "context.h"

namespace client {
    namespace {
        float win_fov = 45.0f, win_ratio = (float) (16.0 / 9.0);

        bool debug_mode = false, wire_mode = false, should_exit = false, is_fullscreen = false;
        glm::mat4 _viewMatrix;
        glm::mat4 _projectionMatrix = glm::perspective(win_fov, win_ratio, 0.1f, 300.0f);
        glm::mat4 _matrix;

        void key_callback(GLFWwindow *window, int key, int scancode, int action,
                          int mods) {
            if (action == GLFW_PRESS) {
                switch (key) {
                    case GLFW_KEY_F1:
                        wire_mode = !wire_mode;
                        break;
                    case GLFW_KEY_F2:
                        debug_mode = !debug_mode;
                        break;
                    case GLFW_KEY_F11:
                        debug_mode = !debug_mode;
                        context::setFullscreen(debug_mode);
                        break;
                    case GLFW_KEY_ESCAPE:
                        std::cout << "User pressed Escape. Client will now exit." << std::endl;
                        glfwSetWindowShouldClose(window, true);
                        break;
                    case GLFW_KEY_LEFT_ALT:
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    default:
                        break;
                }
            }else if(action == GLFW_RELEASE){
                switch (key) {
                    case GLFW_KEY_LEFT_ALT:
                        glfwSetCursorPos(window, context::getWidth() / 2, context::getHeight() / 2);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    }

    void tick(GLFWwindow *window) {
        std::cout << "Preloading entities...\n";
        for (auto &entity : world::get_entities()) {
            entity->preload();
        }

        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        std::cout << "Client ticking!" << std::endl;
        while (!glfwWindowShouldClose(window)) {
            /**
             * input
             */
            auto mat = glm::scale(glm::mat4(1), glm::vec3(0.1));

            /**
             * render
             */
            glClear(GL_COLOR_BUFFER_BIT);
            for (auto &entity : world::get_entities()) {
                entity->draw(mat);
            }

            /**
             * glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
             */
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
}
