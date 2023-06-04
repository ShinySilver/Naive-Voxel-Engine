//
// Created by silverly on 20/05/2021.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <loguru.hpp>

#include "context.h"
#include "client.h"

namespace context {
    namespace {
        int win_width, prev_win_width = WIN_WIDTH, win_height, prev_win_height = WIN_HEIGHT, win_x, win_y;
        GLFWwindow *window = nullptr;

        void APIENTRY glDebugOutput(GLenum source,
                                    GLenum type,
                                    unsigned int id,
                                    GLenum severity,
                                    GLsizei length,
                                    const char *message,
                                    const void *userParam)
        {
            // ignore non-significant error/warning codes
            if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
            DLOG_S(WARNING) << "----------------------------";
            DLOG_S(WARNING) << "OpenGL debug message (" << id << "): " <<  message;

            switch (source)
            {
                case GL_DEBUG_SOURCE_API:             DLOG_S(WARNING) << "Source: API"; break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   DLOG_S(WARNING) << "Source: Window System"; break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER: DLOG_S(WARNING) << "Source: Shader Compiler"; break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:     DLOG_S(WARNING) << "Source: Third Party"; break;
                case GL_DEBUG_SOURCE_APPLICATION:     DLOG_S(WARNING) << "Source: Application"; break;
                case GL_DEBUG_SOURCE_OTHER:           DLOG_S(WARNING) << "Source: Other"; break;
            }

            switch (type)
            {
                case GL_DEBUG_TYPE_ERROR:               DLOG_S(WARNING) << "Type: Error"; break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: DLOG_S(WARNING) << "Type: Deprecated Behaviour"; break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  DLOG_S(WARNING) << "Type: Undefined Behaviour"; break;
                case GL_DEBUG_TYPE_PORTABILITY:         DLOG_S(WARNING) << "Type: Portability"; break;
                case GL_DEBUG_TYPE_PERFORMANCE:         DLOG_S(WARNING) << "Type: Performance"; break;
                case GL_DEBUG_TYPE_MARKER:              DLOG_S(WARNING) << "Type: Marker"; break;
                case GL_DEBUG_TYPE_PUSH_GROUP:          DLOG_S(WARNING) << "Type: Push Group"; break;
                case GL_DEBUG_TYPE_POP_GROUP:           DLOG_S(WARNING) << "Type: Pop Group"; break;
                case GL_DEBUG_TYPE_OTHER:               DLOG_S(WARNING) << "Type: Other"; break;
            }

            switch (severity)
            {
                case GL_DEBUG_SEVERITY_HIGH:         DLOG_S(WARNING) << "Severity: high"; break;
                case GL_DEBUG_SEVERITY_MEDIUM:       DLOG_S(WARNING) << "Severity: medium"; break;
                case GL_DEBUG_SEVERITY_LOW:          DLOG_S(WARNING) << "Severity: low"; break;
                case GL_DEBUG_SEVERITY_NOTIFICATION: DLOG_S(WARNING) << "Severity: notification"; break;
            }
        }
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
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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
#if WIN_VSYNC
        glfwSwapInterval(0);
#endif
        glfwPollEvents();
        glfwSetCursorPos(window, WIN_WIDTH / 2, WIN_HEIGHT / 2);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            ABORT_S() << "Failed to initialize GLAD\n";
            return nullptr;
        }

        int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT){
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }else{
            DLOG_S(WARNING) << "Could not create OpenGL debug context, we are running blind!\n";
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
