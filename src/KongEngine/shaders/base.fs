#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;

uniform sampler2D texture0;

uniform bool texture0_on;

void main()
{
    if (texture0_on)
    {
        FragColor = texture(texture0, outTexcoord);
    }
    else
    {
        FragColor = outClr;
    }
    //FragColor = vec4(1.0f, 0.f, 0.f, 1.f);
} 