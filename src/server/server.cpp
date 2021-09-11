//
// Created by silverly on 21/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>
//#include "../common/world.h"

#include "server.h"

namespace server{
    namespace{
        std::atomic_bool shutting_down;
        std::thread s;

        void tick(){
            while (!shutting_down)
            {

            }
            std::cout << "Server shutting down." << std::endl;
        }
    }

    void start(){
        std::cout << "Server starting." << std::endl;
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