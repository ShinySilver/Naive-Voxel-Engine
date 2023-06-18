//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_SERVER_H
#define IVY_SERVER_H


#include <atomic>

namespace server {
    /**
     * Start the server thread
     */
    void start();

    /**
     * Start an interactive server session. Blocking.
     */
    void start_interactive_session();

    /**
     * Stop an interactive server session.
     */
    void stop_interactive_session();

    /**
     * Notify the server to have it stop at the end at the current tick
     */
    void stop();

    /**
     * Wait for the server to stop.
     */
    void join();

    namespace metrics{
        extern std::atomic<double> average_tick_time_ns, current_tps;
    }
};


#endif //IVY_SERVER_H
