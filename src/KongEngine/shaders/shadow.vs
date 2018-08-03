#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 world_transform;

uniform mat4 light_projection_transform;
uniform mat4 light_view_transform;

out vec4 light_position;

void main()
{
	gl_Position = light_projection_transform * light_view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	light_position = gl_Position;
}