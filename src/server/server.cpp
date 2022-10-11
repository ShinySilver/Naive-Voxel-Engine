//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>

#include "server.h"
#include "../common/world/world.h"
#include "server_networking.h"

namespace server{
    namespace{
        std::atomic_bool shutting_down;
        std::thread s;

        void tick(){
            while (!shutting_down)
            {
                world::tick();
            }
            std::cout << "Server shutting down." << std::endl;
        }
    }

    void start(){
        std::cout << "Server starting..." << std::endl;

		world::init();
        server_networking::init();

        s = std::thread(tick);
    }

    void stop(){
        shutting_down = true;
        server_networking::stop();
    }

    void join(){
        if(s.joinable()){
            s.join();
        }
        server_networking::join();
    }
}
