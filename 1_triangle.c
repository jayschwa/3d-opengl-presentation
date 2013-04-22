#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "shared_utils.h"

const char *title = "Hello Triangle";

// Shader program handle
GLuint g_program;

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
	glShaderSource(vertex_shader, 1, &src, &len);
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
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS,
		      &vertex_shader_compiled);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS,
		      &fragment_shader_compiled);
	if (vertex_shader_compiled != GL_TRUE) {
		GLint log_len;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_len);
		GLchar *log = (GLchar *) malloc(log_len);
		glGetShaderInfoLog(vertex_shader, log_len, &log_len, log);
		fprintf(stderr, "vertex shader:\n%s", log);
		free(log);
		return false;
	} else if (fragment_shader_compiled != GL_TRUE) {
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

	// Attach shader to program
	glAttachShader(g_program, vertex_shader);
	glAttachShader(g_program, fragment_shader);

	// Link program
	glLinkProgram(g_program);

	// Check program link status
	GLint program_linked;
	glGetProgramiv(g_program, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE) {
		GLint log_len;
		glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &log_len);
		GLchar *log = (GLchar *) malloc(log_len);
		glGetProgramInfoLog(g_program, log_len, &log_len, log);
		fprintf(stderr, "program:\n%s", log);
		free(log);
		return false;
	}

	// Create buffer for vertex positions
	GLuint position_buf;
	glGenBuffers(1, &position_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, position_buf);

	// Upload position data to buffer
	float position_data[] = {  0.0,  0.5,  0.0,   // v0
	                          -0.5, -0.5,  0.0,   // v1
	                           0.5, -0.5,  0.0 }; // v2
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(position_data)*sizeof(position_data[0]), // Size (bytes)
	             position_data,   // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Associate shader's "vertex_position" attribute with this buffer
	GLint position_attr = glGetAttribLocation(g_program, "vertex_position");
	glVertexAttribPointer(position_attr, // Attribute
	                      3,             // Vec3 (XYZ)
	                      GL_FLOAT,      // Data type
	                      GL_FALSE,      // Normalize?
	                      0,             // Stride between vertices
	                      0);            // Offset to first vertex
	glEnableVertexAttribArray(position_attr);

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create buffer for vertex colors
	GLuint color_buf;
	glGenBuffers(1, &color_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, color_buf);

	// Upload position data to buffer
	float color_data[] = {  1.0,  0.0,  0.0,   // v0
	                        0.0,  1.0,  0.0,   // v1
	                        0.0,  0.0,  1.0 }; // v2
	glBufferData(GL_ARRAY_BUFFER, // Where to write data
	             sizeof(color_data)*sizeof(color_data[0]), // Size (bytes)
	             color_data,      // Pointer to data
	             GL_STATIC_DRAW); // How data will be used

	// Associate shader's "vertex_color" attribute with this buffer
	GLint color_attr = glGetAttribLocation(g_program, "vertex_color");
	glVertexAttribPointer(color_attr, // Attribute
	                      3,          // Vec3 (RGB)
	                      GL_FLOAT,   // Data type
	                      GL_FALSE,   // Normalize?
	                      0,          // Stride between vertices
	                      0);         // Offset to first vertex
	glEnableVertexAttribArray(color_attr);

	// Unbind buffer from "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

// Draws the scene
// Called every frame
void sceneDraw()
{
}
