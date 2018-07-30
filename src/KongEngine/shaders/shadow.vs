#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 world_transform;
uniform mat4 view_transform;
uniform mat4 project_transform;

void main()
{
	gl_Position = project_transform * view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}