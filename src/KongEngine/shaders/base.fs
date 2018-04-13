#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;

uniform sampler2D texture0;

void main()
{
    FragColor = texture(texture0, outTexcoord);
} 