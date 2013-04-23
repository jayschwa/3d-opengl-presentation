#version 150

uniform sampler2D tex_image;

in vec2 fragment_tex_coords;

out vec4 FragColor;

void main()
{
	FragColor = texture(tex_image, fragment_tex_coords);
}
