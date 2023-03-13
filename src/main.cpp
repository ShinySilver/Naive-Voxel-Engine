#include "client/client.h"
#include "server/server.h"
#include <loguru.hpp>

int main(int argc, char** argv) {

	/**
	 * Setup logs
	 */
	loguru::init(argc, argv);
    loguru::g_stderr_verbosity = 1;
    //loguru::add_file("latest_everything.log", loguru::Truncate, loguru::Verbosity_MAX);
	//loguru::add_file("readable.log", loguru::Append, loguru::Verbosity_INFO);

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

