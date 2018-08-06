#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 tex_uv;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	tex_uv = aPos.xy * 0.5 + 0.5;
}