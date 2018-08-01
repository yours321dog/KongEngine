#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
	gl_Position = project_transform * view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}