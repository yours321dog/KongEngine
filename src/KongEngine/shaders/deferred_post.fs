#version 330 core
out vec4 FragColor;
//layout(location = 0) out float fragmentdepth;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D texture3;

uniform bool test_on;

void main()
{
//    FragColor.r = 0.5;
//    FragColor = vec4(light_position.z, 0.0, 0.0, 1.0);
    FragColor = vec4(vec3(0.5), 1.0);
//    if (test_on)
//        FragColor = texture(diffuse_tex, gl_FragCoord.xy * 0.5 + 0.5);
//    else
//        FragColor = texture(position_tex, gl_FragCoord.xy * 0.5 + 0.5);
//    fragmentdepth = 0.5;
}