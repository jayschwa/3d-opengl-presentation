#version 150

uniform vec3 view_position;

in vec3 vertex_position;

flat out vec3 fragment_color;

mat4 rotmat(in vec3 dir)
{
	vec3 Z = normalize(dir);
	vec3 X = normalize(cross(Z, vec3(0,1,0)));
	vec3 Y = cross(X, Z);
	return transpose(mat4(
		vec4(X,       0),
		vec4(Y,       0),
		vec4(Z,       0),
		vec4(vec3(0), 1)));
}

void main()
{
	mat4 view_matrix = rotmat(-view_position);
	gl_Position = view_matrix * vec4(vertex_position, 1.0);
	fragment_color = vertex_position;
}
