#version 330 core
out vec4 FragColor;
//layout(location = 0) out float fragmentdepth;


void main()
{
//    FragColor.r = 0.5;
//    FragColor = vec4(light_position.z, 0.0, 0.0, 1.0);
//    FragColor = vec4(vec3(0.5), 1.0);
    FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
//    fragmentdepth = 0.5;
}