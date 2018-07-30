#version 330 core
out vec4 FragColor;

struct Light
{
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation;
    float exponent;
    float cutoff;
    float padding1;
    float padding2;
};

uniform Light light0;

void main()
{
    FragColor.r = gl_Position.z;
}