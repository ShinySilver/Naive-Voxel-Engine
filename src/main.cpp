#include "client/client.h"
#include "server/server.h"
#include <loguru.hpp>

int main(int argc, char** argv) {

	/**
	 * Setting up the logs
	 */
	loguru::init(argc, argv);
    loguru::g_stderr_verbosity = 3; // 6 for high verbosity, 2 for standard output
    //loguru::add_file("latest_everything.log", loguru::Truncate, loguru::Verbosity_MAX);
	//loguru::add_file("readable.log", loguru::Append, loguru::Verbosity_INFO);

    /**
     * Starting the server. Not blocking.
     */
    server::start();

	/**
	 * Start the client. Blocking, since the client need the main thread for OpenGL.
	 */
	client::tick();

	/**
	 * Stopping the server after the client closing
	 */
	server::stop();
	server::join();

    return 0;
}

