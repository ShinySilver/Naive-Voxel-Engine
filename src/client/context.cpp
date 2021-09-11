//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "context.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

namespace context {
    namespace {
        GLFWwindow *window = nullptr;

        void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
            /**
             * makes sure the viewport matches the new window dimensions; note that width and
             * height will be significantly larger than specified on retina displays.
             */
            glViewport(0, 0, width, height);
        }
    }

    GLFWwindow *init() {
        if (window != nullptr) {
            std::cout << "Tried to init an already existing context!" << std::endl;
            return nullptr;
        }

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }

        return window;
    }

    void terminate() {
        // glfw: terminate, clearing all previously allocated GLFW resources.
        window = nullptr;
        glfwTerminate();
    }
}