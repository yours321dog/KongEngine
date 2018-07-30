#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aClr;
layout (location = 3) in vec2 aTexcoord;

//switch between wireframe and meshframe
layout (location = 4) in vec3 aBC;

//tangent used for normal mapping
layout (location = 5) in vec3 aTangent;
layout (location = 6) in vec3 aBitangent;

uniform mat4 world_transform;
uniform mat4 view_transform;
uniform mat4 project_transform;

out vec4 outClr;
out vec2 outTexcoord;
out vec3 outBC;

out vec4 world_position;
out vec4 world_normal;
out vec4 world_tangent;
out vec4 world_bitangent;

out vec4 light_position;

// normal mapping flag
uniform bool normal_mapping_on;

// shadow mapping flag
uniform bool shadow_on;

// light transform
uniform mat4 light_view_transform;
uniform mat4 light_projection_transform;

void main()
{
	gl_Position = project_transform * view_transform * world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	outClr = vec4(aClr.z, aClr.y, aClr.x, aClr.w) / 255.f;
	outTexcoord = aTexcoord;
	outBC = aBC;

	world_position = world_transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	
	world_normal = world_transform * vec4(aNormal.xyz, 0.0);
	if (normal_mapping_on)
	{
		world_tangent = world_transform * vec4(aTangent, 0.0);
		world_bitangent = world_transform * vec4(aBitangent, 0.0);
	}

	if (shadow_on)
	{
		light_position = light_projection_transform * light_view_transform * world_position;
	}
}