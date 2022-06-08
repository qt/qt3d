// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 450 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoord;
layout(location=3) in vec4 vertexTangent;

layout(location=0) out vec3 lightDir;
layout(location=1) out vec3 viewDir;
layout(location=2) out vec2 texCoord;


layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 uncorrectedProjectionMatrix;
  mat4 clipCorrectionMatrix;
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
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

layout(std140, binding = 2) uniform extras_uniforms {
    float texCoordScale;
    vec3 ka;            // Ambient reflectivity
    vec3 ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
    float opacity;      // Alpha channel
    vec3 lightPosition;
    vec3 lightIntensity;
};

void main()
{
    // Pass through texture coordinates
    texCoord = vertexTexCoord * texCoordScale;

    // Transform position, normal, and tangent to eye coords
    vec3 normal = normalize(modelNormalMatrix * vertexNormal);
    vec3 tangent = normalize(modelNormalMatrix * vertexTangent.xyz);
    vec3 position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));

    // Calculate binormal vector
    vec3 binormal = normalize(cross(normal, tangent));

    // Construct matrix to transform from eye coords to tangent space
    mat3 tangentMatrix = mat3 (
        tangent.x, binormal.x, normal.x,
        tangent.y, binormal.y, normal.y,
        tangent.z, binormal.z, normal.z);

    // Transform light direction and view direction to tangent space
    vec3 s = lightPosition - position;
    lightDir = normalize(tangentMatrix * vec3(viewMatrix * vec4(s, 1.0)));

    vec3 v = -position;
    viewDir = normalize(tangentMatrix * v);

    // Calculate vertex position in clip coordinates
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
