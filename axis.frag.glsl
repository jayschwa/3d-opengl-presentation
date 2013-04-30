#version 150

uniform bool dotted;

in float axis_position;
flat in vec3 fragment_color;

out vec4 FragColor;

void main()
{
	if (dotted && mod(round(axis_position * 50), 2) == 0) {
		discard;
	}
	FragColor = vec4(fragment_color, 1.0);
}
