//
// Created by silverly on 21/05/2021.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <loguru.hpp>

#include "server.h"
#include "../common/world/world.h"

#include <atomic>
#include <thread>

namespace server{
    namespace{
        std::atomic_bool shutting_down;
        std::thread s;

        void tick(){
			loguru::set_thread_name("server_thread");
			LOG_S(INFO) << "Server started!";
            while (!shutting_down)
            {
                // World.tick?
                //
            }
            LOG_S(INFO) << "Server shutting down";
        }
    }

    void start(){
        LOG_S(INFO) << "Server starting...";

		// for now itializing world at server start
		world::init();

		shutting_down = false;
        s = std::thread(tick);
    }

    void stop(){
        shutting_down = true;
    }

    void join(){
        if(s.joinable()){
            s.join();
        }
    }
}
