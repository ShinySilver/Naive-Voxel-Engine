//
// Created by silverly on 21/11/2021.
//

#include <loguru.hpp>

#include "worker.h"

#include <thread>

int Worker::worker_count = 0;

Worker::Worker(std::string label, std::function<void()> target) {
	this->label = label;
    this->target_tick = target;
    worker_id = worker_count++;
    worker_thread = std::thread(&Worker::run, this);
}

void Worker::stop() {
    should_stop.store(true);
}

void Worker::join() {
    if(worker_thread.joinable()){
        worker_thread.join();
    }
}

void Worker::run() {
	loguru::set_thread_name(this->label.c_str());
    LOG_S(1) << "Created worker [" << std::to_string(worker_id) << "]";
    while(!should_stop.load()){
        target_tick();
    }
    LOG_S(1) << "Worker [" << worker_id << "] stopped";
}
