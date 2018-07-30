#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aClr;
layout (location = 3) in vec2 aTexcoord;

//switch between wireframe and meshframe
layout (location = 4) in vec3 aBC;

uniform mat4 world_transform;
uniform mat4 view_transform;
uniform mat4 project_transform;

out vec4 outClr;
out vec2 outTexcoord;
out vec3 outBC;

out vec4 world_position;
out vec4 world_normal;

void main()
{
	gl_Position = project_transform * view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	outClr = vec4(aClr.z, aClr.y, aClr.x, aClr.w) / 255.f;
	outTexcoord = aTexcoord;
	outBC = aBC;

	world_position = world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	world_normal = transpose(inverse(world_transform)) * aNormal;
	//world_normal = (world_normal + vec4(2.f, 2.f, 2.f, 2.f)) * 0.5f;
}