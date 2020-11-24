#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 2) in vec3 particlePosition;
layout(location = 3) in vec3 particleColor;

layout (location = 0) out VertexBlock
{
    flat vec3 color;
    vec3 pos;
    vec3 normal;
} v_out;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

void main(void)
{
    vec4 pos = vec4(vertexPosition.xyz, 1.0) + vec4(particlePosition, 0.0);
    gl_Position = modelViewProjection * pos;
    v_out.pos = vec4(modelViewMatrix * pos).xyz;
    v_out.normal = normalize(modelNormalMatrix * vertexNormal);
    v_out.color = mix(particleColor * 0.2, particleColor, smoothstep(0.5, 0.8, abs(v_out.normal).z));
}
