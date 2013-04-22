#include <stdbool.h>
#include <GL/gl.h>

const char *title = "Textured Square";

extern const char tex_data[];

GLuint g_main_program;   // Shader program handle
GLuint g_main_vao_state; // Attribute state handle

GLint g_main_u_tex_image; // Texture sampler handle

GLuint g_texture; // Texture handle

// Initializes the scene
// Called once at the start
bool sceneInit()
{
	GLint attribute;   // Vertex attribute handle
	GLuint vertex_buf; // Vertex buffer handle

	g_main_program = loadprogram(__FILE__);

	// Get uniform locations
	g_main_u_tex_image = glGetUniformLocation(g_main_program, "tex_image");

	// Create buffer for vertex data
	glGenBuffers(1, &vertex_buf);

	// Bind buffer to "mount point"
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);

	// Upload vertex data to buffer
	float vertex_data[] = {
		// X     Y     Z     U     V
		-0.8,  0.8,  0.0,  0.0,  0.0,   // v0: upper left
		-0.8, -0.8,  0.0,  0.0,  1.0,   // v1: lower left
		 0.8,  0.8,  0.0,  1.0,  0.0,   // v2: upper right
		 0.8, -0.8,  0.0,  1.0,  1.0 }; // v3: lower right
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
	glClear(GL_COLOR_BUFFER_BIT);

	// Use main program for drawing
	glUseProgram(g_main_program);

	// Restore state
	glBindVertexArray(g_main_vao_state);

	// Bind texture to "mount point" on texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture);

	// Tell tex_image sampler to read from texture unit 0
	glUniform1i(g_main_u_tex_image, 0);

	// Draw triangles
	char indices[] = { 0, 1, 2,      // Upper left triangle
	                   3, 2, 1 };    // Lower right triangle
	glDrawElements(GL_TRIANGLES,     // Draw mode
	               sizeof(indices),  // Number of elements
	               GL_UNSIGNED_BYTE, // Element data type
	               indices);         // Pointer to elements

	// Unbind texture from "mount point"
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear state
	glBindVertexArray(0);
}
