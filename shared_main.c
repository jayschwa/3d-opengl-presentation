#include <stdio.h>
#include <stdlib.h>

#include "shared.h"

GLFWwindow *window = NULL;

int main(void)
{
	// Initialize the GLFW library
	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed\n");
		return EXIT_FAILURE;
	}
	// Use OpenGL 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Do not use deprecated legacy functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create window and its OpenGL context
	window = glfwCreateWindow(640, 480, title, NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwOpenWindow() failed\n");
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	// Initialize scene
	if (!sceneInit()) {
		fprintf(stderr, "sceneInit() failed\n");
		return EXIT_FAILURE;
	}
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		// Draw scene
		sceneDraw();

		// Swap front and back buffers and process events
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}

	return EXIT_SUCCESS;
}
