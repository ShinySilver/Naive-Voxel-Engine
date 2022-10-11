#include "client/client.h"
#include "server/server.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <loguru.hpp>
#include <execinfo.h>
#include <csignal>

#include "client/context.h"
#include "client/client.h"
#include "server/server.h"
#include "client/utils/mesher/chunk_util.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv) {

	/**
	 * Setup logs
	 */
	loguru::init(argc, argv);
	// Put every log message in "latest_everything.log":
	//loguru::add_file("latest_everything.log", loguru::Truncate, loguru::Verbosity_MAX);
	// Only log INFO, WARNING, ERROR and FATAL to "readable.log":
	//loguru::add_file("readable.log", loguru::Append, loguru::Verbosity_INFO);
	// Only show most relevant things on stderr:
	loguru::g_stderr_verbosity = 1;	

    /**
     * Starting the server.
     */
    server::start();

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

