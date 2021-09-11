//
// Created by silverly on 21/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <thread>
#include <../common/world.h>

#include "server.h"

namespace server{
    namespace{
        World world;
        std::atomic_bool shutting_down;
    }
    void tick(World &world){
        while (!shutting_down)
        {

        }
        std::cout("Server shut down.")
    }
}