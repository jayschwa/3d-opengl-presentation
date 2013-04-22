#version 150

uniform sampler2D tex_image;

in vec2 fragment_tex_coords;

out vec4 gl_FragColor;

void main()
{
	gl_FragColor = texture(tex_image, fragment_tex_coords);
}
