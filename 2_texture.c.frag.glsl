#version 150

in vec2 fragment_tex_coords;

out vec4 gl_FragColor;

void main()
{
	gl_FragColor = vec4(fragment_tex_coords, 1.0, 1.0);
}
