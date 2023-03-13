//
// Created by silverly on 20/05/2021.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <loguru.hpp>

#include "context.h"

namespace context {
    namespace {
        int win_width, prev_win_width = WIN_WIDTH, win_height, prev_win_height = WIN_HEIGHT, win_x, win_y;
        GLFWwindow *window = nullptr;
    }

    GLFWwindow *init() {
        if (window != nullptr) {
            ABORT_S() << "Tried to init an already existing context!\n";
            return nullptr;
        }

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "iVy_", nullptr, nullptr);
        if (window == nullptr) {
            glfwTerminate();
            ABORT_S() << "Failed to create GLFW window\n";
            return nullptr;
        }

        glfwMakeContextCurrent(window);
#if WIN_STICKY != 0
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key being pressed below
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide the mouse and enable unlimited mouvement
#endif
        glfwPollEvents();
        glfwSetCursorPos(window, WIN_WIDTH / 2, WIN_HEIGHT / 2);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            ABORT_S() << "Failed to initialize GLAD\n";
            return nullptr;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        return window;
    }

    void setFullscreen(bool b) {
        if (b) {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwGetWindowSize(window, &prev_win_width, &prev_win_height);
            glfwGetWindowPos(window, &win_x, &win_y);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height,
                                 mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, 0, 0, prev_win_width,
                                 prev_win_height,
                                 WIN_DEFAULT_FRAMERATE);
            glfwSetWindowPos(window, win_x, win_y);

        }
    }

    int getWidth() {
        glfwGetWindowSize(window, &win_width, &win_height);
        return win_width;
    }

    int getHeight() {
        glfwGetWindowSize(window, &win_width, &win_height);
        return win_height;
    }

    void terminate() {
        // glfw: terminate, clearing all previously allocated GLFW resources.
        window = nullptr;
        glfwTerminate();
    }
}
