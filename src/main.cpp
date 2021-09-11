#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "client/context.h"
#include "client/client.h"
#include "server/server.h"
#include "client/utils/mesher/chunk_util.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    /**
     * context
     */
    GLFWwindow *window = nullptr;
    if(!(window=context::init()))
    {
        std::cout << "Could not init context!" << std::endl;
        return -1;
    }

    server::start();

    /**
     * render loop
     */
    client::tick(window);

    server::stop();
    server::join();

    /**
     * close all opened buffers
     */
    context::terminate();
    return 0;
}