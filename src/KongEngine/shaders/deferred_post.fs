#version 330 core
out vec4 FragColor;
//layout(location = 0) out float fragmentdepth;

in vec2 tex_uv;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D shadow_tex;

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

// shadow control flag
uniform bool shadow_on;

// light camera transform
uniform mat4 light_view_transform;
uniform mat4 light_projection_transform;

// shadow PCF
vec2 poisson_dick[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float CalculateShadowFactor()
{
    if (!shadow_on)
        return 1.f;

    vec2 texture_uv =  tex_uv;
    vec4 world_position = texture(position_tex, texture_uv);
    vec4 lightspace_position = light_projection_transform * light_view_transform * world_position;

    vec2 shadow_uv = lightspace_position.xy / lightspace_position.w * 0.5 + 0.5;

//    shadow_uv.y = 1.0 - shadow_uv.y;
    float closest_depth = texture(shadow_tex, shadow_uv).x;

    float bias = 0.0005f;
    float shadow = 0.2f;
    float light_depth = lightspace_position.z / lightspace_position.w;
    if (lightspace_position.z / lightspace_position.w - bias <= closest_depth)
    {
        shadow = 1.0f;
    }
    else
    {
        for (int i=0;i<4;i++)
        {
            if ( texture(shadow_tex, shadow_uv + poisson_dick[i] / 700.0f ).x  >= light_depth - bias )
            {
                shadow += 0.2f;
            }
        }
    }

    return shadow;
}

vec4 CalculateLight()
{
    int max_lights = min(NR_LIGHTS, lights_num);
    vec2 texture_uv = tex_uv;
    vec4 res_light = vec4(0, 0, 0, 0);

    vec4 world_position = texture(position_tex, texture_uv);
    vec4 world_normal = texture(normal_tex, texture_uv);
    vec4 diffuse_color = texture(diffuse_tex, texture_uv);

    float shadow_factor = CalculateShadowFactor();

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


        res_light += res_ambient / max_lights + shadow_factor * light_attenuation * (res_diffuse + res_specular);
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
//    FragColor = vec4(0.5, 0.0, 0.0, 1.0);

//    if (test_on)
//        FragColor = texture(diffuse_tex, gl_FragCoord.xy / window_size);
//    else
//        FragColor = texture(position_tex, gl_FragCoord.xy / window_size);
//    fragmentdepth = 0.5;
}