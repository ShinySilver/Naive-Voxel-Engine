#include <iostream>

#ifndef __gl_h_

#include <glad/glad.h>

#endif

#include <GLFW/glfw3.h>
#include <execinfo.h>
#include <csignal>

#include "client/context.h"
#include "client/client.h"
#include "server/server.h"
#include "client/utils/mesher/chunk_util.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {

    /**
     * Starting the server.
     */
    server::start();

    /**
     * Start the client. Blocking.
     */
    client::tick();

    /**
     * Stopping the server after the client closing
     */
    server::stop();
    server::join();

    return 0;
}