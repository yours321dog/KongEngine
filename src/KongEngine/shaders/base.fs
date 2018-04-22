#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;

in vec4 world_position;
in vec4 world_normal;

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

uniform sampler2D texture0;
uniform bool texture0_on;

uniform bool light_on;
uniform Light light0;
uniform bool light0_on;

uniform vec4 cam_position;

uniform Material material;

vec4 CalculateLight(Light light, bool light_n_on)
{
    if (!light_n_on)
    {
        return vec4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        vec4 res_ambient = vec4(0.f, 0.f, 0.f, 1.f);
        vec4 res_diffuse = vec4(0.f, 0.f, 0.f, 1.f);
        vec4 res_specular = vec4(0.f, 0.f, 0.f, 1.f);
        float light_attenuation;
        vec3 light_direction;
        if(light.position.w == 0.0) 
        {
            // it is a directional light.
            // get the direction by converting to vec3 (ignore W) and negate it
            light_direction = light.position.xyz;
            light_attenuation = 1.f;
        }
        else 
        {
            // NOT a directional light
            light_direction = light.position.xyz - world_position.xyz;
            float dist = length(light_direction);
            light_attenuation = 1.f / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);

            // cone restriction
            if (light.exponent > 0.000001f)
            {
                float spot_attenuation = dot(normalize(-light_direction), normalize(light.direction.xyz));
                float light_direction_angle = degrees(acos(spot_attenuation));
                if (light_direction_angle > light.cutoff)
                {
                    light_attenuation = 0.f;
                    //light_attenuation *= pow(spot_attenuation, light.exponent);
                }
                else
                {
                    light_attenuation *= pow(spot_attenuation, light.exponent);
                    //light_attenuation = 0.f;
                }
            }
        }
        // ambient color
        res_ambient = light.ambient * material.ambient;
        
        // diffuse factor
        light_direction = normalize(light_direction);
        float diffuse_factor = dot(light_direction, world_normal.xyz);

        // ignore back face
        if (diffuse_factor > 0.f)
        {
            vec3 view_direction = normalize(cam_position.xyz - world_position.xyz);
            vec3 reflect_direction = reflect(-light_direction, world_normal.xyz);
            float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);

            res_diffuse = diffuse_factor * (light.diffuse * material.diffuse);
            res_specular = specular_factor * (light.specular * material.specular);
        }

        return res_ambient + light_attenuation * (res_diffuse + res_specular);
        //return res_ambient + (res_diffuse + res_specular);
//        vec3 view_direction = normalize(world_position.xyz - cam_position.xyz);
//        vec3 reflect_direction = reflect(-light_direction, world_normal.xyz);
//        float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);
        //return  vec4(material.shininess / 128.f, 0.f, 0.f, 1.f);
        //return vec4(0.4, 0.4, 0.4, 1.0);
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

    if (light_on)
    {
        vec4 light_color = CalculateLight(light0, light0_on);
        //light_color += CalculateLight(light1, light1_on);
        //light_color += CalculateLight(light2, light2_on);
        //light_color += CalculateLight(light3, light3_on);
        FragColor *= light_color;
    }
    //FragColor = vec4(1.0f, 0.f, 0.f, 1.f);
    //FragColor = vec4(world_normal.xyz, 1.f);
} 