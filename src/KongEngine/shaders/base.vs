#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aClr;
layout (location = 2) in vec2 aTexcoord;

uniform mat4 world_transform;
uniform mat4 view_transform;
uniform mat4 project_transform;

out vec4 outClr;
out vec2 outTexcoord;

void main()
{
	gl_Position = project_transform * view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	outClr = vec4(aClr.z, aClr.y, aClr.x, aClr.w) / 255.f;
	outTexcoord = aTexcoord;
}