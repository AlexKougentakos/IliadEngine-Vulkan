#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

struct PointLight {
    vec4 position; // xyz: position, w: unused
    vec4 color;    // xyz: color, w: intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

// PBR material textures
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicMap;
layout(set = 1, binding = 3) uniform sampler2D roughnessMap;
layout(set = 1, binding = 4) uniform sampler2D aoMap; // Optional

layout(push_constant) uniform Push 
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

// Constants
const float PI = 3.14159265359;

// Normal Distribution Function (NDF)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry Function
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel Equation
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    // Fixed normal pointing upwards in world space
    vec3 N = vec3(0.0, 0.0, 1.0);
    
    // Ambient lighting
    vec3 ambient = ubo.ambientLightColor.rgb * vec3(1.0, 1.0, 1.0); // White ambient
    vec3 lighting = ambient;
    
    // Diffuse lighting from the first point light
    if (ubo.numLights > 0) {
        PointLight light = ubo.pointLights[0];
        vec3 L = normalize(light.position.xyz - fragPosWorld);
        float NdotL = max(dot(N, L), 0.0);
        
        // Compute attenuation (quadratic falloff)
        float distance = length(light.position.xyz - fragPosWorld);
        float attenuation = 1.0 / (distance * distance + 0.001); // Prevent division by zero
        
        // Radiance (color * intensity * attenuation)
        vec3 radiance = light.color.xyz * light.color.w * attenuation;
        
        // Diffuse component (white albedo)
        vec3 diffuse = vec3(1.0) * radiance * NdotL;
        
        lighting += diffuse;
    }
    
    // Remove gamma correction
    vec3 color = lighting;
    
    outColor = vec4(color, 1.0);
}

