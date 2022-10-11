//
// Created by silverly on 21/11/2021.
//

#ifndef IVY_WORKER_H
#define IVY_WORKER_H


#include <atomic>
#include <thread>
#include <functional>

class Worker {
public:
    /**
	 * every sec if main worker, check camera, then check if there is any unloaded chunks to add to
	 * the queue otherwise, load entities from the queue, and add them to the client "activate"
	 * queue
     */
    Worker(std::string label, std::function<void()> target);

    void stop();
    void join();

private:
    void run();

    static int worker_count;
    int worker_id;
    std::string label;
    std::function<void()> target_tick;
    std::atomic_bool should_stop;
    std::thread worker_thread;
};

#endif //IVY_WORKER_H

