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

layout(push_constant) uniform Push {
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
    // Retrieve material properties from textures
    vec3 albedo = pow(texture(albedoMap, fragUv).rgb, vec3(2.2)); // Convert from sRGB to linear space
    float metallic = texture(metallicMap, fragUv).r;
    float roughness = texture(roughnessMap, fragUv).r;
    float ao = texture(aoMap, fragUv).r; // Optional

    // Normal mapping
    vec3 N = texture(normalMap, fragUv).rgb;
    N = N * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
    N = normalize(N);

    // Transform normal to world space
    N = normalize(mat3(push.normalMatrix) * N);

    // View vector
    vec3 V = normalize(ubo.invView[3].xyz - fragPosWorld);

    // Reflectance at normal incidence (F0)
    vec3 F0 = vec3(0.04); // Default for non-metals
    F0 = mix(F0, albedo, metallic);

    // Lighting calculations
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        vec3 L = normalize(light.position.xyz - fragPosWorld);
        vec3 H = normalize(V + L);
        float distance = length(light.position.xyz - fragPosWorld);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.color.xyz * light.color.w * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;

        // kS is specular reflectance
        vec3 kS = F;
        // kD is diffuse reflectance (energy conservation)
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        // Lambertian diffuse
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient lighting (using AO map if available)
    vec3 ambient = vec3(0.03) * albedo * ao * ubo.ambientLightColor.w;

    vec3 color = ambient + Lo;

    // HDR tonemapping (optional)
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}
