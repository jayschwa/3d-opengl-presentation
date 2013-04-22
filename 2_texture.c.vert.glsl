#version 150

in vec3 vertex_position;
in vec2 vertex_tex_coords;

out vec2 fragment_tex_coords;

void main()
{
	gl_Position = vec4(vertex_position, 1.0);
	fragment_tex_coords = vertex_tex_coords;
}
