#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;
in vec3 outBC;

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

//render mode enum
const int ERM_MESH      = 0x00000000;
const int ERM_WIREFRAME = 0x00000001;
const int ERM_BOTH      = 0x00000002;

// texture control flags
uniform sampler2D texture0;
uniform bool texture0_on;

// lights control flags
uniform bool light_on;
uniform Light light0;
uniform bool light0_on;

// wireframe control flags
uniform int wireframe_on;

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
    }
}

// Does not take into account GL_TEXTURE_MIN_LOD/GL_TEXTURE_MAX_LOD/GL_TEXTURE_LOD_BIAS,
// nor implementation-specific flexibility allowed by OpenGL spec
float MipMapLevel(in vec2 texture_coordinate) // in texel units
{
    vec2  dx_vtc        = dFdx(texture_coordinate);
    vec2  dy_vtc        = dFdy(texture_coordinate);
    float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
    float mml = 0.5 * log2(delta_max_sqr);
    return max( 0, mml ); // Thanks @Nims
}

// Smooth and constant the wireframe edge
float EdgeFactor(){
    vec3 d = fwidth(outBC);
    vec3 a3 = smoothstep(vec3(0.0), d*1.5, outBC);
    return min(min(a3.x, a3.y), a3.z);
}

void main()
{
    // wireframe mode
    if (wireframe_on == ERM_WIREFRAME)
    {
        FragColor.rgba = mix(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.0), EdgeFactor());
    }
    else // mesh mode
    {
        if (texture0_on)
        {
            float mipmap_level = MipMapLevel(outTexcoord * textureSize(texture0, 0));
            FragColor = textureLod(texture0, outTexcoord, mipmap_level);
            //FragColor = texture(texture0, outTexcoord);
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
    }
    //FragColor = vec4(1.0f, 0.f, 0.f, 1.f);
    //FragColor = vec4(world_normal.xyz, 1.f);
} 