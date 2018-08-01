#version 330 core
out vec4 FragColor;

in vec4 outClr;
in vec2 outTexcoord;
in vec3 outBC;

in vec4 world_position;
in vec4 world_normal;
in vec4 world_tangent;
in vec4 world_bitangent;

in vec4 light_position;

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
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform bool texture0_on;
uniform bool texture1_on;

// normal mapping flag
uniform bool normal_mapping_on;

// lights control flags
uniform bool light_on;
uniform Light light0;
uniform bool light0_on;

// wireframe control flags
uniform int wireframe_on;

uniform vec4 cam_position;

uniform Material material;

// shadow mapping
uniform sampler2D texture4;
uniform bool shadow_on;

// shadow PCF
vec2 poisson_dick[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float CaluateShadowFactor()
{
    vec2 shadow_uv = light_position.xy / light_position.w * 0.5 + 0.5;
//    shadow_uv.y = 1.0 - shadow_uv.y;
    float closest_depth = texture(texture4, shadow_uv).x;

    float bias = 0.0005f;
    float shadow = 0.2f;
    float light_depth = light_position.z / light_position.w;
    if (light_position.z / light_position.w - bias <= closest_depth)
    {
        shadow = 1.0f;
    }
    else
    {
        for (int i=0;i<4;i++)
        {
            if ( texture(texture4, shadow_uv + poisson_dick[i] / 700.0f ).x  >= light_depth - bias )
            {
                shadow += 0.2f;
            }
        }
    }

    return shadow;
}

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
        
        light_direction = normalize(light_direction);
        vec3 view_direction = normalize(cam_position.xyz - world_position.xyz);
        vec3 normal_direction = world_normal.xyz;
        if (texture1_on && normal_mapping_on)
        {
            vec3 N = normalize(normal_direction);
            vec3 T = normalize(world_tangent.xyz - dot(normal_direction, world_tangent.xyz) * normal_direction);
            vec3 B = cross(N,T);
            mat3 TBN = transpose(mat3(T, B, N));

//            mat3 TBN = transpose(mat3(world_tangent.xyz, world_bitangent.xyz, normal_direction));
            view_direction = TBN * view_direction;
            light_direction = TBN * light_direction;
            normal_direction = normalize(texture(texture1, outTexcoord).rgb * 2.0 - 1.0);
        }

        // diffuse factor
        float diffuse_factor = dot(light_direction, normal_direction);

        // ignore back face
        if (diffuse_factor > 0.f)
        {
//            vec3 view_direction = normalize(cam_position.xyz - world_position.xyz);
            vec3 reflect_direction = reflect(-light_direction, normal_direction);
            float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);

            res_diffuse = diffuse_factor * (light.diffuse * material.diffuse);
            res_specular = specular_factor * (light.specular * material.specular);
        }

        // calculate shadow factor
        float shadow_factor_val = 1.f;
        if (shadow_on)
        {
            shadow_factor_val = CaluateShadowFactor();
        }

        return res_ambient + shadow_factor_val * light_attenuation * (res_diffuse + res_specular);
//        return res_diffuse;
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
            vec3 light_color = CalculateLight(light0, light0_on).xyz;
            //light_color += CalculateLight(light1, light1_on);
            //light_color += CalculateLight(light2, light2_on);
            //light_color += CalculateLight(light3, light3_on);
            FragColor *= vec4(light_color, 1.0);
        }
    }
//    FragColor = vec4(1.0f, 0.f, 0.f, 1.f);
//    vec2 shadow_uv = light_position.xy * 0.5 + 0.5;
////    shadow_uv.y = 1.0 - shadow_uv.y;
//    float closest_depth = texture(texture4, shadow_uv).x;
//    FragColor = vec4(closest_depth, 0, 0, 1.f);
} 