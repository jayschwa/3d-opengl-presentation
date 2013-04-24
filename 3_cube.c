#include <stdbool.h>
#include <math.h>

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <GL/glfw.h>

#include "shared.h"

const char *title = "3D Space";

extern const char tex_data[];

const float origin_position[3] = { 0, 0, 0 };
const float origin_direction[3] = { 0, 0, 1 };

float g_cube_pitch = 0;
float g_cube_yaw = 0;
float g_cube_position[3] = { 0, 0, 0 };
float g_cube_direction[3] = { 0, 0, 1 };

float g_view_pitch = 0;
float g_view_yaw = 0;
float g_view_distance = 2;
float g_view_position[3] = { 0, 0, 2 };
float g_view_direction[3] = { 0, 0, -1 };

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
	const float max_pitch = M_PI_2 - 0.1 * M_PI_2;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == 1) {
		g_cube_pitch += (float)y / 1000;
		if (g_cube_pitch < -max_pitch) {
			g_cube_pitch = -max_pitch;
		} else if (g_cube_pitch > max_pitch) {
			g_cube_pitch = max_pitch;
		}
		g_cube_yaw -= (float)x / 500;
		g_cube_direction[0] = sinf(g_cube_yaw) * cosf(g_cube_pitch);
		g_cube_direction[1] = sinf(g_cube_pitch);
		g_cube_direction[2] = cosf(g_cube_yaw) * cosf(g_cube_pitch);
		glfwSetMousePos(0, 0);
	}
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == 1) {
		g_view_pitch -= (float)y / 1000;
		if (g_view_pitch < -max_pitch) {
			g_view_pitch = -max_pitch;
		} else if (g_view_pitch > max_pitch) {
			g_view_pitch = max_pitch;
		}
		g_view_yaw += (float)x / 500;
		g_view_position[0] = sinf(g_view_yaw) * cosf(g_view_pitch);
		g_view_position[1] = sinf(g_view_pitch);
		g_view_position[2] = cosf(g_view_yaw) * cosf(g_view_pitch);
		int i;
		for (i=0; i<3; i++) {
			g_view_direction[i] = -g_view_position[i];
			g_view_position[i] *= g_view_distance;
		}
		glfwSetMousePos(0, 0);
	}
}

void mouseWheel(int w)
{
	int i;
	for (i=0; i<3; i++) {
		g_view_position[i] /= g_view_distance;
	}
	g_view_distance -= (float)w / 100;
	for (i=0; i<3; i++) {
		g_view_position[i] *= g_view_distance;
	}
	glfwSetMouseWheel(0);
}

float g_projection_matrix[16] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

void resize(int width, int height)
{
	glViewport(0, 0, width, height);

	float fov = 60;
	float near = 0.1;
	float far = 100;

	float aspect = (float)width / (float)height;
	float top = near * tanf(fov * M_PI / 360.0);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	g_projection_matrix[0]  = (2.0 * near) / (right - left);
	g_projection_matrix[5]  = (2.0 * near) / (top - bottom);
	g_projection_matrix[10] = (-far - near) / (far - near);
	g_projection_matrix[11] = -1.0;
	g_projection_matrix[14] = (-2.0 * near * far) / (far - near);
}

GLuint g_axis_program;     // Shader program handle
GLuint g_axis_indices_buf; // Vertex indices handle
GLuint g_axis_indices_len; // Number of vertex indices
GLuint g_axis_vao_state;   // Attribute state handle

GLint  g_axis_u_model_position;
GLint  g_axis_u_model_direction;
GLint  g_axis_u_view_position;
GLint  g_axis_u_view_direction;
GLint  g_axis_u_view2projection;
GLint  g_axis_u_dotted;

GLuint g_main_program;     // Shader program handle
GLuint g_main_indices_buf; // Vertex indices handle
GLuint g_main_indices_len; // Number of vertex indices
GLuint g_main_vao_state;   // Attribute state handle

GLint  g_main_u_model_position;
GLint  g_main_u_model_direction;
GLint  g_main_u_view_position;
GLint  g_main_u_view_direction;
GLint  g_main_u_view2projection;
GLint  g_main_u_tex_image; // Texture sampler handle

GLuint g_texture; // Texture handle

// Initializes the scene
// Called once at the start
bool sceneInit()
{
	glfwSetMousePosCallback(mouseMove);
	glfwSetMouseButtonCallback(mouseClick);
	glfwSetMouseWheelCallback(mouseWheel);
	glfwSetWindowSizeCallback(resize);

	glEnable(GL_CULL_FACE);  // Don't draw back side of triangles
	glEnable(GL_DEPTH_TEST); // Don't draw triangles behind other triangles

	GLint attribute;   // Vertex attribute handle
	GLuint vertex_buf; // Vertex buffer handle

/** Axes **********************************************************************/

	g_axis_program = loadprogram("axis");

	// Get uniform locations
	g_axis_u_model_position = glGetUniformLocation(g_axis_program, "model_position");
	g_axis_u_model_direction = glGetUniformLocation(g_axis_program, "model_direction");
	g_axis_u_view_position = glGetUniformLocation(g_axis_program, "view_position");
	g_axis_u_view_direction = glGetUniformLocation(g_axis_program, "view_direction");
	g_axis_u_view2projection = glGetUniformLocation(g_axis_program, "view2projection");
	g_axis_u_dotted = glGetUniformLocation(g_axis_program, "dotted");

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
	                      (GLvoid *)0);   // Offset to first position
	glEnableVertexAttribArray(attribute); // Enable attribute
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &g_axis_indices_buf);
	char axis_index_data[] = { 0, 1, 0, 2, 0, 3 };
	g_axis_indices_len = sizeof(axis_index_data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_axis_indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, // Where to write data
	             g_axis_indices_len,      // Size (bytes)
	             axis_index_data,         // Pointer to data
	             GL_STATIC_DRAW);         // How data will be used
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

/** Cube **********************************************************************/

	g_main_program = loadprogram(__FILE__);

	// Get uniform locations
	g_main_u_model_position = glGetUniformLocation(g_main_program, "model_position");
	g_main_u_model_direction = glGetUniformLocation(g_main_program, "model_direction");
	g_main_u_view_position = glGetUniformLocation(g_main_program, "view_position");
	g_main_u_view_direction = glGetUniformLocation(g_main_program, "view_direction");
	g_main_u_view2projection = glGetUniformLocation(g_main_program, "view2projection");
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
	                      (GLvoid *)0);   // Offset to first position
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Define layout of shader's "vertex_tex_coords" attribute
	attribute = glGetAttribLocation(g_main_program, "vertex_tex_coords");
	glVertexAttribPointer(attribute,      // Attribute handle
	                      2,              // Vec2 (UV)
	                      GL_FLOAT,       // Data type
	                      GL_FALSE,       // Normalize?
	                      20,             // Stride between tex coords
	                      (GLvoid *)12);  // Offset to first tex coord
	glEnableVertexAttribArray(attribute); // Enable attribute

	// Unbind state object
	glBindVertexArray(0);

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create buffer for index (to vertex) data
	glGenBuffers(1, &g_main_indices_buf);
	char main_index_data[] = { 0,  1,  2,  3,  2,  1,   // Front face
	                           4,  5,  6,  7,  6,  5,   // Back face
	                           8,  9, 10, 11, 10,  9,   // Left face
	                          12, 13, 14, 15, 14, 13,   // Right face
	                          16, 17, 18, 19, 18, 17,   // Top face
	                          20, 21, 22, 23, 22, 21 }; // Bottom face
	g_main_indices_len = sizeof(main_index_data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_main_indices_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, // Where to write data
	             g_main_indices_len,      // Size (bytes)
	             main_index_data,         // Pointer to data
	             GL_STATIC_DRAW);         // How data will be used
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	// Draw world space axes
	glUseProgram(g_axis_program);
	glUniform3fv(g_axis_u_model_position, 1, origin_position);
	glUniform3fv(g_axis_u_model_direction, 1, origin_direction);
	glUniform3fv(g_axis_u_view_position, 1, g_view_position);
	glUniform3fv(g_axis_u_view_direction, 1, g_view_direction);
	glUniformMatrix4fv(g_axis_u_view2projection, 1, false, g_projection_matrix);
	glUniform1ui(g_axis_u_dotted, false);
	glBindVertexArray(g_axis_vao_state);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_axis_indices_buf);
	glDrawElements(GL_LINES, g_axis_indices_len, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Draw cube model space axes
	glUseProgram(g_axis_program);
	glUniform3fv(g_axis_u_model_position, 1, g_cube_position);
	glUniform3fv(g_axis_u_model_direction, 1, g_cube_direction);
	glUniform3fv(g_axis_u_view_position, 1, g_view_position);
	glUniform3fv(g_axis_u_view_direction, 1, g_view_direction);
	glUniformMatrix4fv(g_axis_u_view2projection, 1, false, g_projection_matrix);
	glUniform1ui(g_axis_u_dotted, true);
	glBindVertexArray(g_axis_vao_state);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_axis_indices_buf);
	glDrawElements(GL_LINES, g_axis_indices_len, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Use main program for drawing
	glUseProgram(g_main_program);

	// Write to uniforms
	glUniform3fv(g_main_u_model_position, 1, g_cube_position);
	glUniform3fv(g_main_u_model_direction, 1, g_cube_direction);
	glUniform3fv(g_main_u_view_position, 1, g_view_position);
	glUniform3fv(g_main_u_view_direction, 1, g_view_direction);
	glUniformMatrix4fv(g_main_u_view2projection, 1, false, g_projection_matrix);

	// Restore attribute state
	glBindVertexArray(g_main_vao_state);

	// Bind texture to "mount point" on texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture);

	// Tell tex_image sampler to read from texture unit 0
	glUniform1i(g_main_u_tex_image, 0);

	// Bind vertex indices buffer to "mount point"
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_main_indices_buf);

	// Draw triangles from GL_ELEMENT_ARRAY_BUFFER data
	glDrawElements(GL_TRIANGLES,       // Draw mode
	               g_main_indices_len, // Number of elements
	               GL_UNSIGNED_BYTE,   // Element data type
	               0);                 // Offset to first index

	// Unbind vertex indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind texture from "mount point"
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear attribute state
	glBindVertexArray(0);
}
