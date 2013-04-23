#version 150

uniform bool dotted;

in vec3 fragment_position;
flat in vec3 fragment_color;

out vec4 FragColor;

void main()
{
	if (dotted && mod(round(fragment_position * 50), 2) == 0) {
		discard;
	}
	FragColor = vec4(fragment_color, 1.0);
}
