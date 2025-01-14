#include "Common.glsl"

// From https://github.com/google/filament

#define MEDIUMP_FLT_MAX 65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

float D_GGX(float roughness, float NoH, const vec3 n, const vec3 h) {
    vec3 NxH = cross(n, h);
    float a = NoH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

float V_SmithGGXCorrelatedFast(float NoV, float NoL, float roughness) {
    float a = roughness;
    float GGXV = NoL * (NoV * (1.0 - a) + a);
    float GGXL = NoV * (NoL * (1.0 - a) + a);
    return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(const vec3 f0, float f90, float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

vec3 F_Schlick(float u, vec3 f0) { 
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0); 
}

float F_Schlick(float u, float f0, float f90) { 
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0); 
}

float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

float ComputeMicroShadowing(float NoL, float visibility) {
    // Chan 2018, "Material Advances in Call of Duty: WWII"
    float aperture = inversesqrt(1.0 - min(visibility, 0.9999));
    float microShadow = saturate(NoL * aperture);
    return microShadow * microShadow;
}

float Fd_Lambert() { return 1.0 / PI; }

float Diffuse(float roughness, float NoV, float NoL, float LoH) {
    #if QUALITY_LOW
    return Fd_Lambert();
    #else
    return Fd_Burley(roughness, NoV, NoL, LoH);
    #endif
}

vec3 ComputeF0(const vec4 baseColor, float metallic, float reflectance) {
    return baseColor.rgb * metallic + (reflectance * (1.0 - metallic));
}

#if QUALITY_LOW
// min roughness such that (MIN_PERCEPTUAL_ROUGHNESS^4) > 0 in fp16 (i.e. 2^(-14/4), rounded up)
#define MIN_PERCEPTUAL_ROUGHNESS 0.089
#define MIN_ROUGHNESS 0.007921
#else
#define MIN_PERCEPTUAL_ROUGHNESS 0.045
#define MIN_ROUGHNESS 0.002025
#endif

#define MIN_N_DOT_V 1e-4

float clampNoV(float NoV) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return max(NoV, MIN_N_DOT_V);
}

vec3 ComputeDiffuseColor(const vec4 baseColor, float metallic) {
    return baseColor.rgb * (1.0 - metallic);
}

vec3 computeF0(const vec4 baseColor, float metallic, float reflectance) {
    return baseColor.rgb * metallic + (reflectance * (1.0 - metallic));
}

float computeDielectricF0(float reflectance) { 
    return 0.16 * reflectance * reflectance; 
}

float computeMetallicFromSpecularColor(const vec3 specularColor) { 
    return max3(specularColor); 
}

float computeRoughnessFromGlossiness(float glossiness) { 
    return 1.0 - glossiness; 
}

float perceptualRoughnessToRoughness(float perceptualRoughness) { 
    return perceptualRoughness * perceptualRoughness;
}

float roughnessToPerceptualRoughness(float roughness) { 
    return sqrt(roughness);
}

float iorToF0(float transmittedIor, float incidentIor) { 
    return sq((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
}

float f0ToIor(float f0) {
    float r = sqrt(f0);
    return (1.0 + r) / (1.0 - r);
}

vec3 f0ClearCoatToSurface(const vec3 f0) {
    // Approximation of iorTof0(f0ToIor(f0), 1.5)
    // This assumes that the clear coat layer has an IOR of 1.5
    #if QUALITY_LOW
    return saturate(f0 * (f0 * 0.526868 + 0.529324) - 0.0482256);
    #else
    return saturate(f0 * (f0 * (0.941892 - 0.263008 * f0) + 0.346479) - 0.0285998);
    #endif
}

//------------------------------------------------------------------------------
// Specular BRDF dispatch
//------------------------------------------------------------------------------

float distribution(float roughness, float NoH, const vec3 h, const vec3 normal) { 
    return D_GGX(roughness, NoH, normal, h);
}

float visibility(float roughness, float NoV, float NoL) {
    #if QUALITY_LOW
    return V_SmithGGXCorrelatedFast(roughness, NoV, NoL);
    #else
    return V_SmithGGXCorrelated(roughness, NoV, NoL);
    #endif
}

vec3 fresnel(const vec3 f0, float LoH) {
    #if QUALITY_LOW
    return F_Schlick(LoH, f0);// f90 = 1.0
    #else
    float f90 = saturate(dot(f0, vec3(50.0 * 0.33)));
    return F_Schlick(f0, f90, LoH);
    #endif
}
