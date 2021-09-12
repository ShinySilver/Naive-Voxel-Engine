//
// Created by silverly on 20/05/2021.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "client.h"
#include "../common/world.h"

namespace client{

// process all input: query GLFW whether relevant keys are pressed/release this frame and react
// accordingly
// --------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		std::cout << "User pressed Escape. Client will now exit." << std::endl;
		glfwSetWindowShouldClose(window, true);
	}
}

void tick(GLFWwindow *window){

	// for now preload everything before render loop
	std::cout << "Preloading entities...\n";
	for(auto& entity : world::get_entities()) {
		entity->preload();
	}
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	std::cout << "Client ticking!" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);
		auto mat = glm::scale(glm::mat4(1), glm::vec3(0.1));

		// render
		// ------
		glClear(GL_COLOR_BUFFER_BIT);
		for(auto& entity : world::get_entities()) {
			entity->draw(mat);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
}
