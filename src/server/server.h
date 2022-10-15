//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_SERVER_H
#define IVY_SERVER_H


namespace server {
    /**
     * Start the server thread
     */
    void start();

    /**
     * Notify the server to have it stop at the end at the current tick
     */
    void stop();

    /**
     * Wait for the server to stop.
     */
    void join();
};


#endif //IVY_SERVER_H
