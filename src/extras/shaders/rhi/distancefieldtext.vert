#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

layout(location = 0) out vec2 texCoord;
layout(location = 1) out float zValue;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 mvp;
  mat4 inverseModelViewProjectionMatrix;
};

void main()
{
    texCoord = vertexTexCoord;
    zValue = vertexPosition.z;

    gl_Position = mvp * vec4(vertexPosition.xy, 0.0, 1.0);
}

