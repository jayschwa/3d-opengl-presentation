#version 420

in vec3 fragment_color;

out vec4 gl_FragColor;

void main()
{
	gl_FragColor = vec4(fragment_color, 1.0);
}
