const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
struct Light {
    int type;
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
};


layout(std140, binding = auto) uniform qt3d_light_uniforms {
  uniform Light lights[MAX_LIGHTS];
  uniform int lightCount;
  uniform int envLightCount;
};

// Pre-convolved environment maps
layout(binding = auto) uniform samplerCube envLight_irradiance; // For diffuse contribution
layout(binding = auto) uniform samplerCube envLight_specular; // For specular contribution
