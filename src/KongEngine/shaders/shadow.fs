#version 330 core
out vec4 FragColor;
//layout(location = 0) out float fragmentdepth;

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

in vec4 light_position;

void main()
{
//    FragColor.r = 0.5;
//    FragColor = vec4(light_position.z, 0.0, 0.0, 1.0);
//    FragColor = vec4(vec3(0.5), 1.0);
    FragColor = vec4(light_position.z / light_position.w, 0.0, 0.0, 1.0);
    gl_FragDepth = light_position.z / light_position.w;
//    fragmentdepth = 0.5;
}