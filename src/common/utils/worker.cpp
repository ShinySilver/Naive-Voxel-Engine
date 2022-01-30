//
// Created by silverly on 21/11/2021.
//

#include "worker.h"
#include <iostream>
#include <thread>

int Worker::worker_count = 0;

Worker::Worker(std::string label, std::function<void()> target) {
    this->label = label;
    this->target_tick = target;
    worker_id = worker_count++;
    worker_thread = std::thread(&Worker::tick, this);
}

void Worker::stop() {
    should_stop.store(true);
}

void Worker::join() {
    if(worker_thread.joinable()){
        worker_thread.join();
    }
}

void Worker::tick() {
    std::cout << "Created worker with label \"" << label << "\" and id " 
			  << std::to_string(worker_id) << "\n";
    while(!should_stop.load()){
        target_tick();
    }
    std::cout << "Worker with label \"" << label << "\" and id " << worker_id << " stopped\n";
}
