#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 0) out vec3 worldPosition;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

void main()
{
    // Transform position, normal, and tangent to world coords
    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

    // Calculate vertex position in clip coordinates
    gl_Position = modelViewProjection * vec4(worldPosition, 1.0);
}
