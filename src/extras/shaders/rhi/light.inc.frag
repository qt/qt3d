#pragma include defaultuniforms.inc

const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;

struct Light {
    vec3 position;
    float intensity;
    vec3 color;
    float constantAttenuation;
    vec3 direction;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
    int type;
};

layout(std140, binding = auto) uniform qt3d_light_uniforms {
  Light lights[MAX_LIGHTS];
  int lightCount;
  int envLightCount;
};

// Pre-convolved environment maps
layout(binding = auto) uniform samplerCube envLight_irradiance; // For diffuse contribution
layout(binding = auto) uniform samplerCube envLight_specular; // For specular contribution
