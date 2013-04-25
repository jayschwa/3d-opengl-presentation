#version 150

uniform vec3 view_position;

uniform sampler2D tex_image;

in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_tex_coords;

out vec4 FragColor;

const vec3 ambient_light = vec3(0.1, 0.1, 0.1);

const vec3 light_origin = vec3(1);
const vec3 light_color = vec3(1);
const float light_radius = 10;

void main()
{
	vec3 light = ambient_light;

	// diffuse light
	float light_dist = distance(fragment_position, light_origin);
	float falloff = (light_radius - light_dist) / light_radius;
	falloff = pow(max(falloff, 0), 2);
	float angle = -dot(fragment_normal, normalize(fragment_position - light_origin));
	angle = max(angle, 0);
	light += light_color * falloff * angle;

	// specular light
	vec3 view_angle = normalize(fragment_position - view_position);
	vec3 reflect_angle = normalize(reflect(fragment_position - light_origin, fragment_normal));
	angle = -dot(view_angle, reflect_angle);
	angle = pow(max(angle, 0), 60);
	light += light_color * falloff * angle;

	FragColor = texture(tex_image, fragment_tex_coords);
	FragColor.rgb *= light;
}
