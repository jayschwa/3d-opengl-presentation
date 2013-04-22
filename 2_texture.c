#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "shared_utils.h"

const char *title = "Hello Triangle";

// Shader program handle
GLuint g_program;
GLuint g_vao_state;

// Initializes the scene
// Called once at the start
bool sceneInit()
{
	// Create shader objects
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Upload vertex shader source code
	GLchar *src = readfile(__FILE__ ".vert.glsl");
	GLint len = strlen(src);
	glShaderSource(vertex_shader, // Shader handle
	               1,             // Number of source code strings
	               &src,          // Array of source code strings
	               &len);         // Array of lengths of strings
	free(src);

	// Upload fragment shader source code
	src = readfile(__FILE__ ".frag.glsl");
	len = strlen(src);
	glShaderSource(fragment_shader, 1, &src, &len);
	free(src);

	// Compile shaders
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	// Check shader compilation status
	GLint vertex_shader_compiled, fragment_shader_compiled;
	glGetShaderiv(vertex_shader,            // Shader handle
	              GL_COMPILE_STATUS,        // Value to fetch
	              &vertex_shader_compiled); // Variable to store value
	glGetShaderiv(fragment_shader,
	              GL_COMPILE_STATUS,
	              &fragment_shader_compiled);
	if (vertex_shader_compiled != GL_TRUE) {
		// Print error log if compilation failed
		GLint log_len;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_len);
		GLchar *log = (GLchar *) malloc(log_len);
		glGetShaderInfoLog(vertex_shader, log_len, &log_len, log);
		fprintf(stderr, "vertex shader:\n%s", log);
		free(log);
		return false;
	} else if (fragment_shader_compiled != GL_TRUE) {
		// Print error log if compilation failed
		GLint log_len;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_len);
		GLchar *log = (GLchar *) malloc(log_len);
		glGetShaderInfoLog(fragment_shader, log_len, &log_len, log);
		fprintf(stderr, "fragment shader:\n%s", log);
		free(log);
		return false;
	}
	// Create program object
	g_program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(g_program, vertex_shader);
	glAttachShader(g_program, fragment_shader);

	// Link program
	glLinkProgram(g_program);

	// Check program link status
	GLint program_linked;
	glGetProgramiv(g_program,        // Program handle
	               GL_LINK_STATUS,   // Value to fetch
	               &program_linked); // Variable to store value
	if (program_linked != GL_TRUE) {
		// Print error log if link process failed
		GLint log_len;
		glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &log_len);
		GLchar *log = (GLchar *) malloc(log_len);
		glGetProgramInfoLog(g_program, log_len, &log_len, log);
		fprintf(stderr, "program:\n%s", log);
		free(log);
		return false;
	}

	// Create "vertex array" state object
	glGenVertexArrays(1, &g_vao_state);

	// Bind state object so it sees and retains the following state changes
	glBindVertexArray(g_vao_state);

	// Create buffer for vertex positions
	GLuint position_buf;
	glGenBuffers(1, &position_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, position_buf);

	// Upload position data to buffer
	float position_data[] = { -0.8, -0.8,  0.0,   // v0: lower left
	                          -0.8,  0.8,  0.0,   // v1: upper left
	                           0.8, -0.8,  0.0,   // v2: lower right
	                           0.8,  0.8,  0.0 }; // v3: upper right
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(position_data)*sizeof(position_data[0]), // Size (bytes)
	             position_data,   // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Associate shader's "vertex_position" attribute with this buffer
	GLint position_attr = glGetAttribLocation(g_program, "vertex_position");
	glVertexAttribPointer(position_attr, // Attribute handle
	                      3,             // Vec3 (XYZ)
	                      GL_FLOAT,      // Data type
	                      GL_FALSE,      // Normalize?
	                      0,             // Stride between vertices
	                      0);            // Offset to first vertex
	glEnableVertexAttribArray(position_attr); // Enable attribute

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create buffer for vertex texture coordinates
	GLuint tex_coord_buf;
	glGenBuffers(1, &tex_coord_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buf);

	// Upload position data to buffer
	float tex_coord_data[] = {  0.0,  0.0,   // v0: lower left
	                            0.0,  1.0,   // v1: upper left
	                            1.0,  0.0,   // v2: lower right
	                            1.0,  1.0 }; // v3: upper right
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(tex_coord_data)*sizeof(tex_coord_data[0]), // Size (bytes)
	             tex_coord_data,  // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Associate shader's "vertex_color" attribute with this buffer
	GLint tex_coord_attr = glGetAttribLocation(g_program, "vertex_tex_coords");
	glVertexAttribPointer(tex_coord_attr,  // Attribute handle
	                      2,               // Vec2 (UV)
	                      GL_FLOAT,        // Data type
	                      GL_FALSE,        // Normalize?
	                      0,               // Stride between vertices
	                      0);              // Offset to first vertex
	glEnableVertexAttribArray(tex_coord_attr); // Enable attribute

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind state object
	glBindVertexArray(0);

	return true;
}

// Draws the scene
// Called every frame
void sceneDraw()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Use program for drawing
	glUseProgram(g_program);

	// Restore state
	glBindVertexArray(g_vao_state);

	// Draw triangle
	char indices[] = { 1, 0, 2,      // Bottom left triangle
	                   2, 3, 1 };    // Upper right triangle
	glDrawElements(GL_TRIANGLES,     // Draw mode
	               sizeof(indices),  // Number of elements
	               GL_UNSIGNED_BYTE, // Element data type
	               indices);         // Pointer to elements

	// Clear state
	glBindVertexArray(0);
}
