#include <stdbool.h>
#include <math.h>

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <GL/glfw.h>

#include "shared.h"

const char *title = "3D Space";

extern const char tex_data[];

const float identity[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};
float g_view_pitch = 0;
float g_view_yaw = 0;
float g_view_position[3] = { 1, 0, 0 };

float g_cube_pitch = 0;
float g_cube_yaw = 0;
float g_cube_direction[3] = { 1, 0, 0 };

void mouseClick(int key, int pressed) {
	static int x, y;
	if (pressed == 1) {
		glfwGetMousePos(&x, &y);
		glfwDisable(GLFW_MOUSE_CURSOR);
		glfwSetMousePos(0, 0);
	} else {
		glfwEnable(GLFW_MOUSE_CURSOR);
		glfwSetMousePos(x, y);
	}
}
void mouseMove(int x, int y)
{
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == 1) {
		g_cube_pitch += (float)y / 300;
		if (g_cube_pitch <= -M_PI_2) {
			g_cube_pitch = -M_PI_2 + 0.01 * M_PI_2;
		} else if (g_cube_pitch >= M_PI_2) {
			g_cube_pitch = M_PI_2 - 0.01 * M_PI_2;
		}
		g_cube_yaw += (float)x / 300;
		g_cube_direction[0] = cosf(g_view_yaw) * cosf(g_view_pitch);
		g_cube_direction[1] = sinf(g_view_pitch);
		g_cube_direction[2] = sinf(g_view_yaw) * cosf(g_view_pitch);
		glfwSetMousePos(0, 0);
	}
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == 1) {
		g_view_pitch += (float)y / 300;
		if (g_view_pitch <= -M_PI_2) {
			g_view_pitch = -M_PI_2 + 0.01 * M_PI_2;
		} else if (g_view_pitch >= M_PI_2) {
			g_view_pitch = M_PI_2 - 0.01 * M_PI_2;
		}
		g_view_yaw += (float)x / 300;
		g_view_position[0] = cosf(g_view_yaw) * cosf(g_view_pitch);
		g_view_position[1] = sinf(g_view_pitch);
		g_view_position[2] = sinf(g_view_yaw) * cosf(g_view_pitch);
		glfwSetMousePos(0, 0);
	}
}

GLuint g_axis_program;
GLuint g_axis_vao_state;
GLint  g_axis_u_view_position;

GLuint g_main_program;   // Shader program handle
GLuint g_main_vao_state; // Attribute state handle

GLint g_main_u_view_position; // View position handle
GLint g_main_u_tex_image;     // Texture sampler handle

GLuint g_texture; // Texture handle

// Initializes the scene
// Called once at the start
bool sceneInit()
{
	glfwSetMousePosCallback(mouseMove);
	glfwSetMouseButtonCallback(mouseClick);

	glEnable(GL_CULL_FACE);  // Don't draw back side of triangles
	glEnable(GL_DEPTH_TEST); // Don't draw triangles behind other triangles

	GLint attribute;   // Vertex attribute handle
	GLuint vertex_buf; // Vertex buffer handle

/** Axes **********************************************************************/

	g_axis_program = loadprogram("axis");

	// Get uniform locations
	g_axis_u_view_position = glGetUniformLocation(g_axis_program, "view_position");

	// Setup axis program's vertex data buffer
	glGenBuffers(1, &vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
	float axis_data[] = {
		// X     Y     Z 
		 0.0,  0.0,  0.0,   // v0: origin
		 1.0,  0.0,  0.0,   // v1: x-axis
		 0.0,  1.0,  0.0,   // v2: y-axis
		 0.0,  0.0,  1.0 }; // v3: z-axis
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(axis_data) * sizeof(axis_data[0]), // Size (bytes)
	             axis_data,       // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Setup axis program's position attribute
	glGenVertexArrays(1, &g_axis_vao_state);
	glBindVertexArray(g_axis_vao_state);
	attribute = glGetAttribLocation(g_axis_program, "vertex_position");
	glVertexAttribPointer(attribute,      // Attribute handle
	                      3,              // Vec3 (XYZ)
	                      GL_FLOAT,       // Data type
	                      GL_FALSE,       // Normalize?
	                      0,              // Stride between positions
	                      0);             // Offset to first position
	glEnableVertexAttribArray(attribute); // Enable attribute
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

/** Cube **********************************************************************/

	g_main_program = loadprogram(__FILE__);

	// Get uniform locations
	g_main_u_view_position = glGetUniformLocation(g_main_program, "view_position");
	g_main_u_tex_image = glGetUniformLocation(g_main_program, "tex_image");

	// Create buffer for vertex data
	glGenBuffers(1, &vertex_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);

	// Upload vertex data to buffer
	float vertex_data[] = {
		// X     Y     Z     U     V
		// Front face
		-0.5,  0.5,  0.5,  0.0,  0.0,   // v0: upper left
		-0.5, -0.5,  0.5,  0.0,  1.0,   // v1: lower left
		 0.5,  0.5,  0.5,  1.0,  0.0,   // v2: upper right
		 0.5, -0.5,  0.5,  1.0,  1.0,   // v3: lower right
		// Back face
		 0.5,  0.5, -0.5,  0.0,  0.0,   // v4: upper left
		 0.5, -0.5, -0.5,  0.0,  1.0,   // v5: lower left
		-0.5,  0.5, -0.5,  1.0,  0.0,   // v6: upper right
		-0.5, -0.5, -0.5,  1.0,  1.0,   // v7: lower right
		// Left face
		-0.5,  0.5, -0.5,  0.0,  0.0,   // v8:  upper left
		-0.5, -0.5, -0.5,  0.0,  1.0,   // v9:  lower left
		-0.5,  0.5,  0.5,  1.0,  0.0,   // v10: upper right
		-0.5, -0.5,  0.5,  1.0,  1.0,   // v11: lower right
		// Right face
		 0.5,  0.5,  0.5,  0.0,  0.0,   // v12: upper left
		 0.5, -0.5,  0.5,  0.0,  1.0,   // v13: lower left
		 0.5,  0.5, -0.5,  1.0,  0.0,   // v14: upper right
		 0.5, -0.5, -0.5,  1.0,  1.0,   // v15: lower right
		// Top face
		-0.5,  0.5, -0.5,  0.0,  0.0,   // v16: upper left
		-0.5,  0.5,  0.5,  0.0,  1.0,   // v17: lower left
		 0.5,  0.5, -0.5,  1.0,  0.0,   // v18: upper right
		 0.5,  0.5,  0.5,  1.0,  1.0,   // v19: lower right
		// Bottom face
		-0.5, -0.5,  0.5,  0.0,  0.0,   // v20: upper left
		-0.5, -0.5, -0.5,  0.0,  1.0,   // v21: lower left
		 0.5, -0.5,  0.5,  1.0,  0.0,   // v22: upper right
		 0.5, -0.5, -0.5,  1.0,  1.0 }; // v23: lower right

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
	                      20,             // Stride between positions
	                      0);             // Offset to first position
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Define layout of shader's "vertex_tex_coords" attribute
	attribute = glGetAttribLocation(g_main_program, "vertex_tex_coords");
	glVertexAttribPointer(attribute,      // Attribute handle
	                      2,              // Vec2 (UV)
	                      GL_FLOAT,       // Data type
	                      GL_FALSE,       // Normalize?
	                      20,             // Stride between tex coords
	                      12);            // Offset to first tex coord
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Unbind state object
	glBindVertexArray(0);

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create texture object
	glGenTextures(1, &g_texture);

	// Bind texture to "mount point"
	glBindTexture(GL_TEXTURE_2D, g_texture);

	// Upload pixel data to texture
	glTexImage2D(GL_TEXTURE_2D,    // Where to write data
	             0,                // Level of detail
	             GL_RGBA,          // Internal format
	             140,              // Width
	             140,              // Height
	             0,
	             GL_RGBA,          // Format
	             GL_UNSIGNED_BYTE, // Pixel component data type
	             tex_data);        // Pointer to pixel data

	// Texture distance filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbind texture from "mount point"
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

// Draws the scene
// Called every frame
void sceneDraw()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw axes
	glUseProgram(g_axis_program);
	glUniform3fv(g_axis_u_view_position, 1, g_view_position);
	glBindVertexArray(g_axis_vao_state);
	char axes[] = { 0, 1, 0, 2, 0, 3 };
	glDrawElements(GL_LINES, sizeof(axes), GL_UNSIGNED_BYTE, axes);
	glBindVertexArray(0);

	// Use main program for drawing
	glUseProgram(g_main_program);

	// Write to uniforms
	glUniform3fv(g_main_u_view_position, 1, g_view_position);

	// Restore state
	glBindVertexArray(g_main_vao_state);

	// Bind texture to "mount point" on texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture);

	// Tell tex_image sampler to read from texture unit 0
	glUniform1i(g_main_u_tex_image, 0);

	// Draw cube faces
	char indices[] = { 0,  1,  2,  3,  2,  1,   // Front face
	                   4,  5,  6,  7,  6,  5,   // Back face
	                   8,  9, 10, 11, 10,  9,   // Left face
	                  12, 13, 14, 15, 14, 13,   // Right face
	                  16, 17, 18, 19, 18, 17,   // Top face
	                  20, 21, 22, 23, 22, 21 }; // Bottom face

	glDrawElements(GL_TRIANGLES,     // Draw mode
	               sizeof(indices),  // Number of elements
	               GL_UNSIGNED_BYTE, // Element data type
	               indices);         // Pointer to elements

	// Unbind texture from "mount point"
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear state
	glBindVertexArray(0);
}
