#version 150

uniform vec3 model_position;
uniform vec3 model_direction;

uniform vec3 view_position;
uniform vec3 view_direction;

in vec3 vertex_position;
in vec2 vertex_tex_coords;

out vec2 fragment_tex_coords;

mat4 posmat(in vec3 pos)
{
	mat4 mat = mat4(1); // identity matrix
	mat[3] = vec4(pos, 1);
	return mat;
}

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
	mat4 model2world = rotmat(model_direction) * posmat(model_position);
	mat4 world2view = rotmat(-view_position);
	gl_Position = world2view * vec4(vertex_position, 1.0);
	fragment_tex_coords = vertex_tex_coords;
}
