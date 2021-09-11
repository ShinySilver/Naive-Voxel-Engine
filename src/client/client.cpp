//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "client.h"

namespace client{
    namespace{
		// process all input: query GLFW whether relevant keys are pressed/released this frame and 
		// react accordingly
		// -----------------------------------------------------------------------------------------
		void processInput(GLFWwindow *window)
        {
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                std::cout << "User pressed Escape. Client will now exit." << std::endl;
                glfwSetWindowShouldClose(window, true);
            }
        }
    }
    void tick(GLFWwindow *window){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        std::cout << "Client ticking!" << std::endl;
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            processInput(window);

            // render
            // ------
            glClear(GL_COLOR_BUFFER_BIT);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
}
