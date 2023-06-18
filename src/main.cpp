#if !SERVER_ONLY
#include "client/client.h"
#endif
#include "server/server.h"
#include <loguru.hpp>

int main(int argc, char** argv) {

	/**
	 * Setting up the logs
	 */
    loguru::g_stderr_verbosity = 3; // 6 for highest verbosity, 2 for standard output
    #if SIMPLIFIED_LOGS
    loguru::g_preamble_date = false;
    loguru::g_preamble_time = false;
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_file = false;
    loguru::g_preamble_thread = false;
    loguru::g_preamble_pipe = false;
    loguru::g_preamble_explain = false;
    #endif
    loguru::init(argc, argv);

    /**
     * Starting the server. Not blocking, unless an interactive session was started too.
     */
    server::start();

    /**
     * Starting the client. Blocking.
     */
    #if !SERVER_ONLY
    client::tick();
    #endif

	/**
	 * Stopping the server after the client closing
	 */
	server::stop();
	server::join();

    return 0;
}

