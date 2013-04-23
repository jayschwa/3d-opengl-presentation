#include <stdbool.h>

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <GL/glfw.h>

#include "shared.h"

const char *title = "Hello Triangle";

GLuint g_main_program;     // Shader program handle
GLuint g_main_indices_buf; // Vertex indices handle
GLuint g_main_indices_len; // Number of vertex indices
GLuint g_main_vao_state;   // Attribute state handle

// Initializes the scene
// Called once at the start
bool sceneInit()
{
	GLint attribute;   // Vertex attribute handle
	GLuint vertex_buf; // Vertex buffer handle

	g_main_program = loadprogram(__FILE__);

	// Create buffer for vertex data
	glGenBuffers(1, &vertex_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);

	// Upload vertex data to buffer
	float vertex_data[] = {
		// X     Y     Z     R     G     B
		 0.0,  0.8,  0.0,  1.0,  0.0,  0.0,   // v0: red top,
		-0.8, -0.8,  0.0,  0.0,  1.0,  0.0,   // v1: green lower left
		 0.8, -0.8,  0.0,  0.0,  0.0,  1.0 }; // v2: blue lower right
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(vertex_data) * sizeof(vertex_data[0]), // Size (bytes)
	             vertex_data,     // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Create "vertex array" state object
	glGenVertexArrays(1, &g_main_vao_state);

	// Bind state object so it sees and retains the following attribute state
	glBindVertexArray(g_main_vao_state);

	// Define layout of shader's "vertex_position" attribute
	attribute = glGetAttribLocation(g_main_program, "vertex_position");
	glVertexAttribPointer(attribute,      // Attribute handle
	                      3,              // Vec3 (XYZ)
	                      GL_FLOAT,       // Data type
	                      GL_FALSE,       // Normalize?
	                      24,             // Stride between positions
	                      (GLvoid *)0);   // Offset to first position
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Define layout of shader's "vertex_color" attribute
	attribute = glGetAttribLocation(g_main_program, "vertex_color");
	glVertexAttribPointer(attribute,      // Attribute handle
	                      3,              // Vec3 (RGB)
	                      GL_FLOAT,       // Data type
	                      GL_FALSE,       // Normalize?
	                      24,             // Stride between colors
	                      (GLvoid *)12);  // Offset to first color
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Unbind state object
	glBindVertexArray(0);

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create buffer for index (to vertex) data
	glGenBuffers(1, &g_main_indices_buf);
	char main_index_data[] = { 0, 1, 2 }; // Vertex indices
	g_main_indices_len = sizeof(main_index_data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_main_indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, // Where to write data
	             g_main_indices_len,      // Size (bytes)
	             main_index_data,         // Pointer to data
	             GL_STATIC_DRAW);         // How data will be used
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

// Draws the scene
// Called every frame
void sceneDraw()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Use main program for drawing
	glUseProgram(g_main_program);

	// Restore attribute state
	glBindVertexArray(g_main_vao_state);

	// Bind vertex indices buffer to "mount point"
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_main_indices_buf);

	// Draw triangles from GL_ELEMENT_ARRAY_BUFFER data
	glDrawElements(GL_TRIANGLES,       // Draw mode
	               g_main_indices_len, // Number of elements
	               GL_UNSIGNED_BYTE,   // Element data type
	               0);                 // Offset to first index

	// Unmount vertex indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Clear attribute state
	glBindVertexArray(0);
}
