#include <stdio.h>
#include <stdlib.h>
#include <GL/glfw.h>

extern char *title;

int main(void)
{
	// Initialize the GLFW library
	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed\n");
		return EXIT_FAILURE;
	}
	// Use OpenGL 3.x
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);

	// Do not use deprecated legacy functions
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create window and its OpenGL context
	if (!glfwOpenWindow(640, 480, 8, 8, 8, 0, 24, 0, GLFW_WINDOW)) {
		fprintf(stderr, "glfwOpenWindow() failed\n");
		return EXIT_FAILURE;
	}
	glfwSetWindowTitle(title);

	// Initialize scene
	if (!sceneInit()) {
		fprintf(stderr, "sceneInit() failed\n");
		return EXIT_FAILURE;
	}
	// Loop until the user closes the window
	while (glfwGetWindowParam(GLFW_OPENED)) {
		// Draw scene
		sceneDraw();

		// Swap front and back buffers and process events
		glfwSwapBuffers();
	}

	return EXIT_SUCCESS;
}
