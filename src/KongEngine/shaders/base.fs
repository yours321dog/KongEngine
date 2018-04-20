#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;

in vec3 world_position;
in vec3 world_normal;

struct Light
{
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 sepcular;
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

uniform sampler2D texture0;
uniform bool texture0_on;

uniform Light light0;
uniform bool light0_on;

uniform vec4 cam_position;

uniform Material material;

vec4 CalculateLight(Light light, bool light_on)
{
    if (!light_on)
    {
        return vec4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        vec4 res_ambient(0.f, 0.f, 0.f, 1.f);
        vec4 res_diffuse(0.f, 0.f, 0.f, 1.f);
        vec4 res_specular(0.f, 0.f, 0.f, 1.f);
        float light_attenuation;
        vec3 light_direction;
        if(lightPosition.w == 0.0) 
        {
          // it is a directional light.
          // get the direction by converting to vec3 (ignore W) and negate it
          light_direction = -light.position.xyz;
          light_attenuation = 1.f;
        }
        else 
        {
          // NOT a directional light
          light_diretion = light.position.xyz - world_position;
          float dist = length(light_direction);
          light_attenuation = 1.f / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);

          // cone restriction
          float light_direction_angle = degrees(acos(dot(-light_direction, normalize(light.direction))));
          if (light_direction_angle > light.cutoff)
          {
            light_attenuation = 0.f;
          }
        }
        // ambient color
        res_ambient = light.ambient * material.ambient;
        
        // diffuse factor
        light_direction = normalize(light_direction);
        float diffuse_factor = dot(light_direction, world_normal);

        // ignore back face
        if (diffuse_factor > 0.f)
        {
            vec3 view_direction = normalize(cam_position - world_position);
            vec3 reflect_direction = reflect(-light_direction, world_normal);
            flaot specular_factor = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);

            res_diffuse = diffuse_factor * (light.diffuse * material.diffuse);
            res_specular = specular_factor * (light.specular * material.specular);
        }

        return res_ambient + light_attenuation * (res_diffuse + res_specular);
    }
}

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