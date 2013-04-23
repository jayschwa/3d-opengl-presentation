#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <GL/glfw.h>

// Read file and return contents in buffer
char *readfile(const char *name)
{
	struct stat status;
	stat(name, &status);
	char *buf = (char *)malloc(status.st_size + 1);
	FILE *file = fopen(name, "r");
	fread(buf, 1, status.st_size, file);
	fclose(file);
	buf[status.st_size] = '\0';
	return buf;
}

// Returns compiled GLSL shader read from source file
GLuint loadshader(GLenum type, const char *file)
{
	GLuint shader = glCreateShader(type);

	// Upload vertex shader source code
	GLchar *src = readfile(file);
	GLint len = strlen(src);
	glShaderSource(shader,               // Shader handle
	               1,                    // Number of source code strings
	               (const GLchar**)&src, // Array of source code strings
	               &len);                // Array of lengths of strings
	free(src);

	glCompileShader(shader);

	// Get shader compilation status
	GLint shader_compiled;
	glGetShaderiv(shader,            // Shader handle
	              GL_COMPILE_STATUS, // Value to fetch
	              &shader_compiled); // Variable to store value

	// Print log and abort if compilation failed
	if (shader_compiled != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		GLchar *log = (GLchar *) malloc(len);
		glGetShaderInfoLog(shader, len, &len, log);
		fprintf(stderr, "%s:\n%s", file, log);
		free(log);
		abort();
	}

	return shader;
}

// Loads and links named GLSL shader program
GLuint loadprogram(const char *name)
{
	// Load vertex and fragment shaders
	char *shader_name = (char *)malloc(strlen(name)+strlen(".type.glsl")+1);
	sprintf(shader_name, "%s%s", name, ".vert.glsl");
	GLuint vertex_shader = loadshader(GL_VERTEX_SHADER, shader_name);
	sprintf(shader_name, "%s%s", name, ".frag.glsl");
	GLuint fragment_shader = loadshader(GL_FRAGMENT_SHADER, shader_name);
	free(shader_name);

	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);

	// Get program link status
	GLint program_linked;
	glGetProgramiv(program,          // Program handle
	               GL_LINK_STATUS,   // Value to fetch
	               &program_linked); // Variable to store value

	// Print log and abort if link process failed
	if (program_linked != GL_TRUE) {
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		GLchar *log = (GLchar *) malloc(len);
		glGetProgramInfoLog(program, len, &len, log);
		fprintf(stderr, "%s program:\n%s", name, log);
		free(log);
		abort();
	}

	return program;
}
