#version 450

layout(location = 0) in vec3 texCoord0;
layout(location = 0) out vec4 fragColor;


// Gamma correction

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

layout(std140, binding = 2) uniform qt3d_morph_uniforms {
  float gammaStrength;
};

layout(binding = 3) uniform samplerCube skyboxTexture;

vec3 gammaCorrect(const in vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    vec4 baseColor = texture(skyboxTexture, texCoord0);
    vec4 gammaColor = vec4(gammaCorrect(baseColor.rgb), 1.0);
    // This is an odd way to enable or not gamma correction,
    // but this is a way to avoid branching until we can generate shaders
    fragColor = mix(baseColor, gammaColor, gammaStrength);
}
