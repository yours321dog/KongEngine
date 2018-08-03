#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;

in vec4 outClr;
in vec2 outTexcoord;
in vec3 outBC;

in vec4 world_position;
in vec4 world_normal;
in vec4 world_tangent;
in vec4 world_bitangent;

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

// wireframe control flags
uniform int wireframe_on;

uniform Material material;

vec3 CalculateNormal()
{
    vec3 normal_direction = world_normal.xyz;
    if (texture1_on && normal_mapping_on)
    {
        vec3 N = normalize(normal_direction);
        vec3 T = normalize(world_tangent.xyz - dot(normal_direction, world_tangent.xyz) * normal_direction);
        vec3 B = cross(N,T);
        mat3 TBN = mat3(T, B, N);

//        mat3 TBN = transpose(mat3(world_tangent.xyz, world_bitangent.xyz, normal_direction));
        normal_direction = TBN * normalize(texture(texture1, outTexcoord).rgb * 2.0 - 1.0);
    }

    return normal_direction;
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
        gDiffuse = mix(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.0), EdgeFactor());
    }
    else // mesh mode
    {
        if (texture0_on)
        {
            float mipmap_level = MipMapLevel(outTexcoord * textureSize(texture0, 0));
            gDiffuse = textureLod(texture0, outTexcoord, mipmap_level);
            //FragColor = texture(texture0, outTexcoord);
        }
        else
        {
            gDiffuse = material.diffuse;
        }
    }

    gPosition = world_position;
    gNormal = vec4(CalculateNormal(), 1.0);
//    gNormal = CalculateNormal();

//    gNormal = vec4(0.5, 0.0, 0.0, 1.0);
//    gDiffuse = vec4(0.0, 0.5, 0.0, 1.0);
//    gPosition = vec4(0.0, 0.0, 0.5, 1.0);
//    gPosition = world_position;
} 