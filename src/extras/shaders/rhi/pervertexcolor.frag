#version 450

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 viewProjectionMatrix;
  mat4 inverseViewMatrix;
  mat4 inverseProjectionMatrix;
  mat4 inverseViewProjectionMatrix;
  mat4 viewportMatrix;
  mat4 inverseViewportMatrix;
  vec4 textureTransformMatrix;
  vec3 eyePosition;
  float aspectRatio;
  float gamma;
  float exposure;
  float time;
};

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 mvp;
  mat4 inverseModelViewProjectionMatrix;
};

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

layout(std140, binding = 2) uniform qt3d_light_uniforms {
  uniform Light lights[MAX_LIGHTS];
  uniform int lightCount;
  uniform int envLightCount;
};

void adsModel(const in vec3 worldPos,
              const in vec3 worldNormal,
              const in vec3 worldView,
              const in float shininess,
              out vec3 diffuseColor,
              out vec3 specularColor)
{
    diffuseColor = vec3(0.0);
    specularColor = vec3(0.0);

    // We perform all work in world space
    vec3 n = normalize(worldNormal);
    vec3 s = vec3(0.0);

    for (int i = 0; i < lightCount; ++i) {
        float att = 1.0;
        float sDotN = 0.0;

        if (lights[i].type != TYPE_DIRECTIONAL) {
            // Point and Spot lights

            // Light position is already in world space
            vec3 sUnnormalized = lights[i].position - worldPos;
            s = normalize(sUnnormalized); // Light direction

            // Calculate the attenuation factor
            sDotN = dot(s, n);
            if (sDotN > 0.0) {
                if (lights[i].constantAttenuation != 0.0
                 || lights[i].linearAttenuation != 0.0
                 || lights[i].quadraticAttenuation != 0.0) {
                    float dist = length(sUnnormalized);
                    att = 1.0 / (lights[i].constantAttenuation +
                                 lights[i].linearAttenuation * dist +
                                 lights[i].quadraticAttenuation * dist * dist);
                }

                // The light direction is in world space already
                if (lights[i].type == TYPE_SPOT) {
                    // Check if fragment is inside or outside of the spot light cone
                    if (degrees(acos(dot(-s, lights[i].direction))) > lights[i].cutOffAngle)
                        sDotN = 0.0;
                }
            }
        } else {
            // Directional lights
            // The light direction is in world space already
            s = normalize(-lights[i].direction);
            sDotN = dot(s, n);
        }

        // Calculate the diffuse factor
        float diffuse = max(sDotN, 0.0);

        // Calculate the specular factor
        float specular = 0.0;
        if (diffuse > 0.0 && shininess > 0.0) {
            float normFactor = (shininess + 2.0) / 2.0;
            vec3 r = reflect(-s, n);   // Reflection direction in world space
            specular = normFactor * pow(max(dot(r, worldView), 0.0), shininess);
        }

        // Accumulate the diffuse and specular contributions
        diffuseColor += att * lights[i].intensity * diffuse * lights[i].color;
        specularColor += att * lights[i].intensity * specular * lights[i].color;
    }
}

vec4 phongFunction(const in vec4 ambient,
                   const in vec4 diffuse,
                   const in vec4 specular,
                   const in float shininess,
                   const in vec3 worldPosition,
                   const in vec3 worldView,
                   const in vec3 worldNormal)
{
    // Calculate the lighting model, keeping the specular component separate
    vec3 diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, worldView, shininess, diffuseColor, specularColor);

    // Combine spec with ambient+diffuse for final fragment color
    vec3 color = (ambient.rgb + diffuseColor) * diffuse.rgb
               + specularColor * specular.rgb;

    return vec4(color, diffuse.a);
}

void main()
{
    vec3 worldView = normalize(eyePosition - worldPosition);
    fragColor = phongFunction(color, color, vec4(0.0), 0.0, worldPosition, worldView, worldNormal);
}
