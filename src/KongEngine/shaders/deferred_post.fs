#version 330 core
out vec4 FragColor;
//layout(location = 0) out float fragmentdepth;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D texture3;

uniform bool test_on;

uniform vec2 window_size;

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

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;
    float shininess;
};

uniform Material material;

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform int lights_num;

uniform vec4 cam_position;

vec4 CalculateLight()
{
    int max_lights = min(NR_LIGHTS, lights_num);
    vec2 texture_uv =  gl_FragCoord.xy / window_size;
    vec4 res_light = vec4(0, 0, 0, 0);

    vec4 world_position = texture(position_tex, texture_uv);
    vec4 world_normal = texture(normal_tex, texture_uv);
    vec4 diffuse_color = texture(diffuse_tex, texture_uv);

    for (int i = 0; i < max_lights; i++)
    {
        vec4 res_ambient = vec4(0.f, 0.f, 0.f, 1.f);
        vec4 res_diffuse = vec4(0.f, 0.f, 0.f, 1.f);
        vec4 res_specular = vec4(0.f, 0.f, 0.f, 1.f);
        float light_attenuation;
        vec3 light_direction;
        if(lights[i].position.w == 0.0) 
        {
            // it is a directional lights[i].
            // get the direction by converting to vec3 (ignore W) and negate it
            light_direction = lights[i].position.xyz;
            light_attenuation = 1.f;
        }
        else 
        {
            // NOT a directional lights[i]
            light_direction = lights[i].position.xyz - world_position.xyz;
            float dist = length(light_direction);
            light_attenuation = 1.f / (lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);

            // cone restriction
            if (lights[i].exponent > 0.000001f)
            {
                float spot_attenuation = dot(normalize(-light_direction), normalize(lights[i].direction.xyz));
                float light_direction_angle = degrees(acos(spot_attenuation));
                if (light_direction_angle > lights[i].cutoff)
                {
                    light_attenuation = 0.f;
                    //light_attenuation *= pow(spot_attenuation, lights[i].exponent);
                }
                else
                {
                    light_attenuation *= pow(spot_attenuation, lights[i].exponent);
                    //light_attenuation = 0.f;
                }
            }
        }
        // ambient color
        res_ambient = lights[i].ambient * material.ambient;
        
        light_direction = normalize(light_direction);
        vec3 view_direction = normalize(cam_position.xyz - world_position.xyz);
        vec3 normal_direction = world_normal.xyz;

        // diffuse factor
        float diffuse_factor = dot(light_direction, normal_direction);

        // ignore back face
        if (diffuse_factor > 0.f)
        {
//            vec3 view_direction = normalize(cam_position.xyz - world_position.xyz);
            vec3 reflect_direction = reflect(-light_direction, normal_direction);
            float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);

            res_diffuse = diffuse_factor * (lights[i].diffuse * material.diffuse);
            res_specular = specular_factor * (lights[i].specular * material.specular);
        }


        res_light += res_ambient / max_lights + light_attenuation * (res_diffuse + res_specular);
    }

    res_light *= diffuse_color;

    return res_light;
}


void main()
{
//    FragColor.r = 0.5;
//    FragColor = vec4(light_position.z, 0.0, 0.0, 1.0);
//    FragColor = vec4(vec3(0.5), 1.0);
    
    FragColor = CalculateLight();

//    if (test_on)
//        FragColor = texture(diffuse_tex, gl_FragCoord.xy / window_size);
//    else
//        FragColor = texture(position_tex, gl_FragCoord.xy / window_size);
//    fragmentdepth = 0.5;
}