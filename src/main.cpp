#include <iostream>

#ifndef __gl_h_
#include <glad/glad.h>
#endif
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
     * Creating context
     */
    GLFWwindow *window = nullptr;
    if(!(window=context::init()))
    {
        std::cout << "Could not init context!" << std::endl;
        return -1;
    }

    /**
     * Starting the server.
     */
    server::start();

    /**
     * Render loop. Blocking.
     */
    client::tick(window);

    /**
     * Stopping the server after the client closing
     */
    server::stop();
    server::join();

    /**
     * Closing all opened buffers and destroying context
     */
    context::terminate();
    return 0;
}